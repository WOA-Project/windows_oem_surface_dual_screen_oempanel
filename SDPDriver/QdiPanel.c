// QDI_Panel_FunctionTableType callbacks registered with the Graphics KMD.
// Every callback validates its arguments and turns the call into a display
// action that is executed on the panel's worker thread.

#include "SdpDriver.h"
#include "QdiPanel.tmh"

// Start/End timing events of the QDI callbacks (keyword SDP_KEYWORD_PERF).
#define SDP_TRACE_PERF(EventName, Panel)                    \
    TraceLoggingWrite(                                      \
        g_SdpTraceLoggingProvider,                          \
        EventName,                                          \
        TraceLoggingLevel(SDP_TL_LEVEL_VERBOSE),          \
        TraceLoggingKeyword(SDP_KEYWORD_PERF),              \
        TraceLoggingUInt32((Panel)->DisplayId, "MonitorID "), \
        TraceLoggingUInt64(UsFromBootPrecise(), "uSec"))

// QDI_Panel_Init
QDI_ErrorType
OEMPanel_Init(_In_ QDI_HandleType Handle, _In_ QDI_Panel_AttrType* PanelConfig)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    QDI_ErrorType result = QDI_STATUS_OK;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_Init %p %p", Handle, PanelConfig);

    if (panel == NULL || PanelConfig == NULL)
    {
        result = QDI_STATUS_BAD_PARAM;
    }
    else
    {
        NTSTATUS status = AddDisplayActionRequestSynchronous(panel, DisplayActionDriverInit | SDP_ACTION_FLAG_QDI, PanelConfig, NULL);
        if (!NT_SUCCESS(status))
        {
            result = QDI_STATUS_FAILED;
        }
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_Init %p %p failed %u", Handle, PanelConfig, result);
    }
    return result;
}

// QDI_Panel_SetMode
QDI_ErrorType
OEMPanel_SetMode(_In_ QDI_HandleType Handle, _In_ QDI_Panel_AttrType* PanelConfig)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    QDI_ErrorType result = QDI_STATUS_OK;

    if (panel == NULL || PanelConfig == NULL)
    {
        result = QDI_STATUS_BAD_PARAM;
    }
    else
    {
        SDP_TRACE_PERF("QDIOEMSetModeStart", panel);

        if (panel->HasCalibration != 0)
        {
            NTSTATUS status =
                AddDisplayActionRequestSynchronous(panel, DisplayActionInitPanelAttr | SDP_ACTION_FLAG_QDI, PanelConfig, NULL);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "Applying the panel attributes failed %!STATUS!", status);
            }
        }

        NTSTATUS status = AddDisplayActionRequestSynchronous(panel, DisplayActionSetMode | SDP_ACTION_FLAG_QDI, PanelConfig, NULL);
        if (!NT_SUCCESS(status))
        {
            result = QDI_STATUS_FAILED;
        }
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_SetMode failed %u", result);
    }

        if (panel != NULL)
    {
        SDP_TRACE_PERF("QDIOEMSetModeEnd", panel);
    }
    return result;
}

// QDI_Panel_SetPower
QDI_ErrorType
OEMPanel_SetPower(_In_ QDI_HandleType Handle, _In_ bool32 PowerOn)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    QDI_ErrorType result = QDI_STATUS_OK;
    ULONG powerOn = PowerOn;

    if (panel == NULL)
    {
        result = QDI_STATUS_BAD_PARAM;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_SetPower: NULL handle (power %u)", PowerOn);
    }
    else
    {
        SDP_TRACE_PERF("QDIOEMSetPowerStart", panel);

        NTSTATUS status = AddDisplayActionRequestSynchronous(panel, DisplayActionPower | SDP_ACTION_FLAG_QDI, &powerOn, NULL);
        if (!NT_SUCCESS(status))
        {
            result = QDI_STATUS_FAILED;
        }
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_SetPower %p %u failed %u", Handle, PowerOn, result);
    }

    if (panel != NULL)
    {
        SDP_TRACE_PERF("QDIOEMSetPowerEnd", panel);
    }
    return result;
}

// QDI_Panel_SetBacklight: a non zero percentage re-applies the stored level
QDI_ErrorType
OEMPanel_SetBacklight(_In_ QDI_HandleType Handle, _In_ uint32 Level)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    QDI_ErrorType result = QDI_STATUS_OK;

    if (panel == NULL || Level > 100)
    {
        result = QDI_STATUS_BAD_PARAM;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_SetBacklight %p %u invalid", Handle, Level);
    }
    else
    {
        PDEVICE_CONTEXT device = DeviceGetContext(panel->Device);

        //UCHAR level = (Level == 0) ? 0 : (UCHAR)panel->Brightness;
        UCHAR level = (Level == 0) ? 0 : (UCHAR)((Level * 255) / 100); // For some reason QCDXKM doesnt call set nits

        for (ULONG i = 0; i < device->PanelCount; i++)
        {
            NTSTATUS status = AddDisplayActionRequestSynchronous(&device->Panels[i], DisplayActionSetBrightness | SDP_ACTION_FLAG_QDI, &level, NULL);
            if (!NT_SUCCESS(status))
            {
                result = QDI_STATUS_FAILED;
                break;
            }
        }
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_SetBacklight %p %u failed %u", Handle, Level, result);
    }
    return result;
}

// QDI_Panel_Term
QDI_ErrorType
OEMPanel_Term(_In_ QDI_HandleType Handle)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    QDI_ErrorType result = QDI_STATUS_OK;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_Term %p", Handle);

    if (panel == NULL)
    {
        result = QDI_STATUS_BAD_PARAM;
    }
    else
    {
        NTSTATUS status = AddDisplayActionRequestSynchronous(panel, DisplayActionDriverTerminate | SDP_ACTION_FLAG_QDI, NULL, NULL);
        if (!NT_SUCCESS(status))
        {
            result = QDI_STATUS_FAILED;
        }
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI_Panel_Term failed %u", result);
    }
    return result;
}

// QDI_Panel_GetPropertyConfig: nothing to report
QDI_ErrorType
OEMPanel_GetPropertyConfig(_In_ QDI_HandleType Handle, _In_ QDI_Display_PropertyConfigType* PropertyData)
{
    QDI_ErrorType result = QDI_STATUS_OK;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_GetPropertyConfig %p", Handle);

    if (Handle == NULL || PropertyData == NULL)
    {
        result = QDI_STATUS_BAD_PARAM;
    }

    if (result != QDI_STATUS_OK)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_GetPropertyConfig failed %u", result);
    }
    return result;
}

// QDI_Panel_SetBrightness: brightness in milli-nits with a transition time
QDI_ErrorType
OemPanel_SetBrightnessNits(_In_ QDI_HandleType Handle, _In_ uint32 MilliNits, _In_ uint32 TransitionTimeMs)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    PDEVICE_CONTEXT device = DeviceGetContext(panel->Device);
    QDI_ErrorType result = QDI_STATUS_OK;

    if (device->BrightnessOverride)
    {
        // A user mode override (IOCTL_SDP_SET_BRIGHTNESS_OVERRIDE) owns the backlight.
        TraceEvents(TRACE_LEVEL_WARNING, TRACE_QDI, "SetBrightness ignored, override active");
        return QDI_STATUS_OK;
    }

        if (panel != NULL && panel->Ops.IsNitsValid != NULL && panel->Ops.IsNitsValid(panel, MilliNits))
    {
        ULONG request[2];

        panel->RequestedNits = MilliNits;
        request[0] = MilliNits;
        request[1] = TransitionTimeMs;

        NTSTATUS status =
            AddDisplayActionRequestSynchronous(panel, DisplayActionSetBrightnessNits | SDP_ACTION_FLAG_QDI, request, NULL);
        if (!NT_SUCCESS(status))
        {
            result = QDI_STATUS_FAILED;
        }

        TraceLoggingWrite(
            g_SdpTraceLoggingProvider,
            "SetBrightnessNits",
            TraceLoggingLevel(SDP_TL_LEVEL_INFO),
            TraceLoggingHexUInt32(MilliNits, "Nits"),
            TraceLoggingHexUInt32(TransitionTimeMs, "TransitionTime in ms"),
            TraceLoggingUInt32(panel->DisplayId, "PANEL NUMBER"));
        return result;
    }

    result = QDI_STATUS_BAD_PARAM;
    TraceEvents(TRACE_LEVEL_WARNING, TRACE_QDI, "SetBrightness %u mnits invalid", MilliNits);

    TraceLoggingWrite(
        g_SdpTraceLoggingProvider,
        "InvalidSetBrightnessNits",
        TraceLoggingLevel(SDP_TL_LEVEL_WARNING),
        TraceLoggingHexUInt32(MilliNits, "Nits"),
        TraceLoggingUInt32(panel->DisplayId, "PANEL NUMBER"));
    return result;
}

// QDI_Panel_Event Maps the events to worker actions and
// tracks the vsync period.
QDI_ErrorType
OEMPanel_HandleEvent(_In_ QDI_HandleType Handle, _In_ uint32 Events, _In_opt_ void* EventData, _In_ uint32 EventDataSize)
{
    UNREFERENCED_PARAMETER(EventDataSize);

    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Handle;
    PDEVICE_CONTEXT device = (panel != NULL) ? DeviceGetContext(panel->Device) : NULL;
    QDI_ErrorType result = QDI_STATUS_OK;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG displayId = (panel != NULL) ? panel->DisplayId : 0;

    // The frequent events are not traced.
    if (Events != QDI_PANEL_EVENT_VSYNC_IRQ && Events != QDI_PANEL_EVENT_UPDATE_REQUEST &&
        Events != QDI_PANEL_EVENT_VSYNC_ENABLED && Events != QDI_PANEL_EVENT_VSYNC_DISABLED)
    {
        TraceEvents(
            TRACE_LEVEL_INFORMATION, TRACE_QDI, "QDI_Panel_Event %p display %u event 0x%x", Handle, displayId, Events);

        TraceLoggingWrite(
            g_SdpTraceLoggingProvider,
            "QDIOEMEvent",
            TraceLoggingLevel(SDP_TL_LEVEL_INFO),
            TraceLoggingHexUInt32(Events, "QDI_EVENT"),
            TraceLoggingUInt32(displayId, "PANEL NUMBER"));
    }

    if (panel == NULL || device == NULL)
    {
        return QDI_STATUS_BAD_PARAM;
    }

    panel->LastEvent = Events;

    switch (Events)
    {
        case QDI_PANEL_EVENT_PRE_POWER:
            SDP_TRACE_PERF("QDIOEMEventPrePowerStart", panel);
            TraceEvents(
                TRACE_LEVEL_INFORMATION, TRACE_QDI, "PRE_POWER (power %u)", *(PULONG)EventData);
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionPrePower | SDP_ACTION_FLAG_QDI, EventData, NULL);
            SDP_TRACE_PERF("QDIOEMEventPrePowerEnd", panel);
            break;

        case QDI_PANEL_EVENT_UPDATE_REQUEST:
            panel->UpdateRequestCount++;
            break;

        case QDI_PANEL_EVENT_INIT_SEQUENCE:
            SDP_TRACE_PERF("QDIOEMEventInitSeqStart", panel);

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionInitSequence | SDP_ACTION_FLAG_QDI, NULL, NULL);

            if (panel->EsdRecoveryPending)
            {
                KeAcquireGuardedMutex(&panel->Lock);
                panel->EsdRecoveryPending = FALSE;
                KeReleaseGuardedMutex(&panel->Lock);
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionCommitProperties | SDP_ACTION_FLAG_QDI, NULL, NULL);
            }

            // Restore the brightness after the panel was re-initialised.
            if (panel->Ops.SetDbv != NULL && panel->Brightness != 0)
            {
                panel->Ops.SetDbv(panel, panel->Brightness);
            }

            WdfTimerStart(device->EsdTimer, WDF_REL_TIMEOUT_IN_MS(SDP_ESD_POLL_INTERVAL_MS));
            SDP_TRACE_PERF("QDIOEMEventInitSeqEnd", panel);
            break;

        case QDI_PANEL_EVENT_TERM_SEQUENCE:
            SDP_TRACE_PERF("QDIOEMEventTermSeqStart", panel);

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionTermSequence | SDP_ACTION_FLAG_QDI, NULL, NULL);

            if (panel->PanelAsleep == TRUE)
            {
                WdfTimerStop(device->EsdTimer, FALSE);
            }
            SDP_TRACE_PERF("QDIOEMEventTermSeqEnd", panel);
            break;

        case QDI_PANEL_EVENT_POST_POWER:
            SDP_TRACE_PERF("QDIOEMEventPostPowerStart", panel);
            TraceEvents(
                TRACE_LEVEL_INFORMATION, TRACE_QDI, "POST_POWER (power %u)", *(PULONG)EventData);
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionPostPower | SDP_ACTION_FLAG_QDI, EventData, NULL);
            SDP_TRACE_PERF("QDIOEMEventPostPowerEnd", panel);
            break;

        case QDI_PANEL_EVENT_DRIVER_LOADED:
            device->State = SDP_DEVICE_STATE_DRIVER_LOADED;
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionDriverLoaded | SDP_ACTION_FLAG_QDI, NULL, NULL);
            break;

        case QDI_PANEL_EVENT_VSYNC_IRQ:
            if (panel->VsyncMeasure && panel->VsyncEnabled == TRUE)
            {
                if (panel->VsyncSampleCount == 0)
                {
                    panel->VsyncFirstTime = UsFromBootPrecise();
                    panel->VsyncLastTime = panel->VsyncFirstTime;
                    panel->VsyncSampleCount++;
                    RtlZeroMemory(panel->VsyncDeltas, sizeof(panel->VsyncDeltas));
                }
                else
                {
                    ULONGLONG previous = panel->VsyncLastTime;
                    panel->VsyncLastTime = UsFromBootPrecise();
                    ULONGLONG delta = panel->VsyncLastTime - previous;

                    if (delta < 14000 || delta > 20000)
                    {
                        // Not a plausible frame period (50..71 Hz): restart the measurement.
                        panel->VsyncSampleCount = 0;
                        panel->VsyncLastDelta = (ULONG)(panel->VsyncLastTime - previous);
                        RtlZeroMemory(panel->VsyncDeltas, sizeof(panel->VsyncDeltas));
                    }
                    else
                    {
                        if (panel->VsyncLastTime < previous)
                        {
                            panel->VsyncSampleCount = 0;
                            panel->VsyncLastDelta = (ULONG)(panel->VsyncLastTime - previous);
                            RtlZeroMemory(panel->VsyncDeltas, sizeof(panel->VsyncDeltas));
                        }
                        else
                        {
                            if (panel->VsyncSampleTarget < 6)
                            {
                                panel->VsyncDeltas[panel->VsyncSampleCount - 1] = (ULONG)delta;
                            }
                            panel->VsyncSampleCount++;
                        }

                        if (panel->VsyncSampleTarget < panel->VsyncSampleCount)
                        {
                            ULONGLONG total = panel->VsyncLastTime - panel->VsyncFirstTime;
                            panel->VsyncPeriodUs = (ULONG)(total / panel->VsyncSampleTarget);
                            panel->VsyncSampleCount = 0;
                        }
                    }
                }
            }
            break;

        case QDI_PANEL_EVENT_VSYNC_ENABLED:
            panel->VsyncEnabled = TRUE;
            panel->VsyncSampleCount = 0;
            RtlZeroMemory(panel->VsyncDeltas, sizeof(panel->VsyncDeltas));
            break;

        case QDI_PANEL_EVENT_VSYNC_DISABLED:
            panel->VsyncEnabled = FALSE;
            panel->VsyncSampleCount = 0;
            break;

        default:
            break;
    }

    if (!NT_SUCCESS(status))
    {
        PCWSTR eventName = GetQDIEventName(Events);

        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QDI, "QDI event 0x%x failed %!STATUS!", Events, status);

        Utility_EventLogEntryWrite(panel->Device, SDP_MSG_QDI_EVENT_FAILED, status, 0, 0, NULL, 1, &eventName, 1);

        TraceLoggingWrite(
            g_SdpTraceLoggingProvider,
            "QDIOEMEvent",
            TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
            TraceLoggingHexUInt32(Events, "QDI_EVENT"),
            TraceLoggingUInt32(panel->DisplayId, "PANEL NUMBER"));
    }
    return result;
}
