// ESD / temperature polling and panel recovery.

#include "SdpDriver.h"
#include "Esd.tmh"

// Sends DCS display off (0x28) or on (0x29) to the panel
NTSTATUS
SendDisplayOnOffAction(_In_ PPANEL_CONTEXT Panel, _In_ BOOLEAN On)
{
    SDP_DSI_NODE sequence;
    RtlZeroMemory(&sequence, sizeof(sequence));

    if (Panel == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "SendDisplayOnOffAction: NULL panel");
        return STATUS_INVALID_PARAMETER;
    }

    AppendDisplayCommand(&sequence, On ? 0x29 : 0x28, FALSE);

    NTSTATUS status = SendSequence(Panel, &sequence);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Sending display %s failed", On ? "on" : "off");
    }
    return status;
}

// Hard resets the panel after an ESD detection stops the poll
// timer, pulses the reset GPIO, asks the Graphics KMD to reset the display and
// drops every queued action.
NTSTATUS
SendReInitalizeCommand(_In_ PPANEL_CONTEXT Panel)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT device = DeviceGetContext(Panel->Device);

    WdfTimerStop(device->EsdTimer, FALSE);
    ToggleResetLine(&Panel->ResetGpio, SDP_RESET_GPIO_PULSE);

    KeAcquireGuardedMutex(&Panel->Lock);
    Panel->EsdRecoveryPending = TRUE;
    KeReleaseGuardedMutex(&Panel->Lock);

    QDI_ErrorType qdiStatus = Panel->DisplayUtil.QDI_DisplayUtil_ResetDisplay(Panel->DisplayId, 0);

    KeAcquireGuardedMutex(&Panel->ActionRing.Lock);
    Panel->ActionRing.WriteIndex = 0;
    Panel->ActionRing.ReadIndex = 0;
    RtlZeroMemory(Panel->ActionRing.Nodes, sizeof(Panel->ActionRing.Nodes));
    KeReleaseGuardedMutex(&Panel->ActionRing.Lock);

    if (qdiStatus != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "QDI_DisplayUtil_ResetDisplay failed %u", qdiStatus);
        status = STATUS_UNSUCCESSFUL;
    }
    return status;
}

// Work item queued by ForcePanelReInit
VOID
ResetWorkItemCallBack(_In_ WDFWORKITEM WorkItem)
{
    PDEVICE_CONTEXT device = DeviceGetContext(WdfWorkItemGetParentObject(WorkItem));
    PWORKITEM_CONTEXT workItem = SdpGetWorkItemContext(WorkItem);

    ULONG index = ConvertQDIDisplayIDtoDisplayIndex(workItem->DisplayId);
    if (index == SDP_INVALID_PANEL_INDEX)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "ESD reset work item for invalid display id %u", workItem->DisplayId);
    }
    else
    {
        SendReInitalizeCommand(&device->Panels[index]);
    }

    WdfObjectDelete(WorkItem);
}

// Defers the ESD recovery to a passive level work item
VOID
ForcePanelReInit(_In_ PPANEL_CONTEXT Panel)
{
    WDF_WORKITEM_CONFIG config;
    WDF_WORKITEM_CONFIG_INIT(&config, ResetWorkItemCallBack);

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WORKITEM_CONTEXT);
    attributes.ParentObject = Panel->Device;

    WDFWORKITEM workItem;
    NTSTATUS status = WdfWorkItemCreate(&config, &attributes, &workItem);
    if (NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_THREAD, "Queueing the ESD reset work item");

        PWORKITEM_CONTEXT context = SdpGetWorkItemContext(workItem);
        context->DisplayId = Panel->DisplayId;
        WdfWorkItemEnqueue(workItem);
    }
}

// Periodic poll (every SDP_ESD_POLL_INTERVAL_MS): refreshes the temperature
// based colour / brightness compensation and verifies the DCS power mode of
// every panel that is on and whose temperature changed
VOID
EsdRefreshTimerCallBack(_In_ WDFTIMER Timer)
{
    PDEVICE_CONTEXT device = DeviceGetContext(WdfTimerGetParentObject(Timer));

    LONG temperatures[SDP_PANEL_COUNT] = {0, 0};
    NTSTATUS temperatureStatus = GetAcpiDsmFuelGaugeTemperatures(WdfTimerGetParentObject(Timer), 0, temperatures);

    UCHAR powerModeOk = TRUE;
    UCHAR useAllPanels = TRUE;
    BOOLEAN esdDetected = FALSE;
    ULONG retries = 0;

    for (ULONG i = 0; i < device->PanelCount; i++)
    {
        PPANEL_CONTEXT panel = &device->Panels[i];

        if (panel->PanelAsleep == TRUE || panel->DisplayOff == TRUE)
        {
            continue;
        }

        if (NT_SUCCESS(temperatureStatus))
        {
            LONG temperature = (i == 0) ? temperatures[0] : temperatures[1];

            if (panel->Temperature == temperature)
            {
                                // below only runs when the temperature changed or the temperature query failed.
                continue;
            }
            SDP_TEMP_RESULT result = {0};

            panel->Temperature = temperature;
            NTSTATUS status = AddDisplayActionRequestSynchronous(
                panel, DisplayActionTemperatureCompensation | SDP_ACTION_FLAG_USER, &temperature, &result);
            if (NT_SUCCESS(status))
            {
                panel->ThermalMaxNits = result.MaxNits;
                panel->ThermalPercent = result.Percent;

                if (result.MaxNits < panel->LastNitsScaled)
                {
                    ULONGLONG nits = result.MaxNits;
                    status = AddDisplayActionRequestSynchronous(
                        panel, DisplayActionSetBrightnessNits | SDP_ACTION_FLAG_USER, &nits, &useAllPanels);
                }
            }
        }

        // Read the DCS power mode back (at most two attempts in total).
        do
        {
            ULONG powerMode = 0;
            NTSTATUS status = AddDisplayActionRequestSynchronous(panel, DisplayActionGetPowerMode, &powerMode, &powerModeOk);
            if (!NT_SUCCESS(status) || powerModeOk == FALSE)
            {
                                if (status != STATUS_TIMEOUT)
                {
                    DelayMilliseconds(500);
                    retries++;
                    esdDetected = TRUE;
                }
            }
            else
            {
                esdDetected = FALSE;
            }
        } while (esdDetected && retries < 2);

        if (esdDetected)
        {
            TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_PANEL, "ESD detected on panel %u", i);

            panel->PanelAsleep = TRUE;
            panel->DisplayOff = TRUE;
            panel->EsdCount++;

            AddDisplayActionRequestSynchronous(panel, DisplayActionEsdReset | SDP_ACTION_FLAG_USER, NULL, NULL);

            TraceLoggingWrite(
                g_SdpTraceLoggingProvider,
                "ESDResetPanel",
                TraceLoggingLevel(SDP_TL_LEVEL_WARNING),
                TraceLoggingUInt32(i, "Panel"),
                TraceLoggingUInt32(panel->EsdCount, "ESD Count"));
        }
    }
}

// Timer callback: poll, then re-arm
VOID
EsdRefreshTimerFn(_In_ WDFTIMER Timer)
{
    EsdRefreshTimerCallBack(Timer);
    WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(SDP_ESD_POLL_INTERVAL_MS));
}
