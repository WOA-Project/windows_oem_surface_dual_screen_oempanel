// I/O queues, the in-caller-context probe and the top level IOCTL dispatch.

#include "SdpDriver.h"
#include "Queue.tmh"

//C_ASSERT(sizeof(QDI_DisplayUtil_FunctionTableType) == 0x50);

// Copies the Graphics KMD interface table into both panel contexts
NTSTATUS
Qdi_OEMInitFunctionTable(_In_ PDEVICE_CONTEXT Device, _In_reads_bytes_(Length) PUCHAR Table, _In_ ULONG Length)
{
    if (Device == NULL || Table == NULL || Length != sizeof(QDI_DisplayUtil_FunctionTableType))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_INTERFACE, "Invalid QDI interface table (length %u)", Length);
        return STATUS_INVALID_PARAMETER;
    }

    for (ULONG i = 0; i < SDP_PANEL_COUNT; i++)
    {
        RtlCopyMemory(&Device->Panels[i].DisplayUtil, Table, sizeof(QDI_DisplayUtil_FunctionTableType));
    }
    return STATUS_SUCCESS;
}

// IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE: remember the display utility table and
// register the panel callbacks of both panels with the Graphics KMD
NTSTATUS
OemDisplay_InitFunctionTable(_In_ PDEVICE_CONTEXT Device, _In_reads_bytes_(Length) PUCHAR Table, _In_ ULONG Length)
{
    QDI_Panel_FunctionTableType panelTable;
    RtlZeroMemory(&panelTable, sizeof(panelTable));

    if (Length == 0 || Device == NULL || Table == NULL)
    {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    NTSTATUS status = Qdi_OEMInitFunctionTable(Device, Table, Length);
    if (!NT_SUCCESS(status))
    {
        return STATUS_UNSUCCESSFUL;
    }

    panelTable.uPanelFunctionTableVersion = SDP_QDI_PANEL_API_VERSION;
    panelTable.QDI_Panel_Init = OEMPanel_Init;
    panelTable.QDI_Panel_SetMode = OEMPanel_SetMode;
    panelTable.QDI_Panel_Term = OEMPanel_Term;
    panelTable.QDI_Panel_SetPower = OEMPanel_SetPower;
    panelTable.QDI_Panel_SetBacklight = OEMPanel_SetBacklight;
    panelTable.QDI_Panel_Event = OEMPanel_HandleEvent;
    panelTable.QDI_Panel_GetPropertyConfig = OEMPanel_GetPropertyConfig;
    panelTable.QDI_Panel_SetBrightness = OemPanel_SetBrightnessNits;

    for (ULONG i = 0; i < Device->PanelCount; i++)
    {
        PPANEL_CONTEXT panel = &Device->Panels[i];

        if (i == 0)
        {
            panel->DisplayId = QDI_DISPLAY_PRIMARY;
        }
        else if (i == 1)
        {
            panel->DisplayId = QDI_DISPLAY_SECONDARY;
        }
        else
        {
            panel->DisplayId = QDI_DISPLAY_NONE;
        }

        QDI_ErrorType qdiStatus =
            panel->DisplayUtil.QDI_DisplayUtil_Register(panel->DisplayId, panel, &panelTable, 0);
        if (qdiStatus != QDI_STATUS_OK)
        {
            return STATUS_UNSUCCESSFUL;
        }

        // Hold the panel in reset until the power sequence runs.
        if (panel->ResetGpio.ExplicitLevels)
        {
            ToggleResetLine(&panel->ResetGpio, SDP_RESET_GPIO_ASSERT);
        }
    }

    return status;
}

// Panel slot of the display id stored in a calibration blob
NTSTATUS
ExtractPanelIDFromColorRequest(_In_ PVOID Blob, _In_ SIZE_T Length, _Out_ PULONG PanelIndex)
{
    if (Blob == NULL || Length < 0x1C)
    {
        return STATUS_INVALID_PARAMETER;
    }

    *PanelIndex = ConvertQDIDisplayIDtoDisplayIndex(((SDP_CAL_HEADER*)Blob)->DisplayId);
    return STATUS_SUCCESS;
}

// Default queue plus one manual queue per panel for pending status requests
NTSTATUS
OemPanelDriverQueueInitialize(_In_ WDFDEVICE Device)
{
    PDEVICE_CONTEXT context = DeviceGetContext(Device);

    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);
    queueConfig.EvtIoDeviceControl = OemPanelDriverEvtIoDeviceControl;

    NTSTATUS status = WdfIoQueueCreate(Device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &context->DefaultQueue);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate (default) failed %!STATUS!", status);
        return status;
    }

    SdpGetQueueContext(context->DefaultQueue);

    for (ULONG i = 0; i < context->PanelCount; i++)
    {
        WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);
        queueConfig.PowerManaged = WdfFalse;

        status = WdfIoQueueCreate(Device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &context->Panels[i].StatusQueue);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate (status %u) failed %!STATUS!", i, status);
        }
    }

    return status;
}

// IOCTL_SDP_UPDATE_FRAMEBUFFER passes a user pointer inside a buffered
// request, so the pointed-to pixels are probed and locked here, in the caller's
// context
VOID
OnIoInCallerContext(_In_ WDFDEVICE Device, _In_ WDFREQUEST Request)
{
    NTSTATUS status;
    WDF_REQUEST_PARAMETERS parameters;

    WDF_REQUEST_PARAMETERS_INIT(&parameters);
    WdfRequestGetParameters(Request, &parameters);

    if (parameters.Type == WdfRequestTypeDeviceControl)
    {
        if (parameters.Parameters.DeviceIoControl.IoControlCode == IOCTL_SDP_UPDATE_FRAMEBUFFER)
        {
            if (parameters.Parameters.DeviceIoControl.InputBufferLength < sizeof(SDP_FRAMEBUFFER_UPDATE))
            {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            else
            {
                SDP_FRAMEBUFFER_UPDATE* update = NULL;
                status = WdfRequestRetrieveInputBuffer(Request, sizeof(SDP_FRAMEBUFFER_UPDATE), (PVOID*)&update, NULL);

                                if (NT_SUCCESS(status) && update->Data != NULL)
                {
                    WDFMEMORY memory;
                    status = WdfRequestProbeAndLockUserBufferForRead(
                        Request, update->Data, (size_t)update->Height * update->Stride, &memory);
                    if (NT_SUCCESS(status))
                    {
                        update->Data = WdfMemoryGetBuffer(memory, NULL);
                        status = WdfDeviceEnqueueRequest(Device, Request);
                    }
                }
            }
        }
        else
        {
            status = WdfDeviceEnqueueRequest(Device, Request);
        }
    }
    else
    {
        status = WdfDeviceEnqueueRequest(Device, Request);
    }

    if (!NT_SUCCESS(status))
    {
        WdfRequestComplete(Request, status);
    }
}

// Top level IOCTL dispatch
VOID
OemPanelDriverEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode)
{
    PDEVICE_CONTEXT device = DeviceGetContext(WdfIoQueueGetDevice(Queue));
    BOOLEAN completeRequest = TRUE;
    PVOID inputBuffer = NULL;
    PVOID outputBuffer = NULL;
    size_t inputLength = 0;
    size_t outputLength = 0;
    ULONG bytesReturned = 0;
    NTSTATUS status = STATUS_SUCCESS;

    if (InputBufferLength != 0)
    {
        status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &inputBuffer, &inputLength);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveInputBuffer failed %!STATUS!", status);
            goto Complete;
        }
    }

    if (OutputBufferLength != 0)
    {
        status = WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &outputBuffer, &outputLength);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveOutputBuffer failed %!STATUS!", status);
            goto Complete;
        }
    }

    if (IoControlCode == IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE)
    {
        status = OemDisplay_InitFunctionTable(device, inputBuffer, (ULONG)inputLength);
        device->State = NT_SUCCESS(status) ? SDP_DEVICE_STATE_INTERFACE_SET : SDP_DEVICE_STATE_NONE;
    }
    else if (IoControlCode == IOCTL_SDP_LOAD_CALIBRATION)
    {
        ULONG panelIndex = SDP_INVALID_PANEL_INDEX;

        status = ExtractPanelIDFromColorRequest(inputBuffer, inputLength, &panelIndex);
        if (!NT_SUCCESS(status) || panelIndex == SDP_INVALID_PANEL_INDEX)
        {
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Invalid calibration blob %!STATUS!", status);
        }
        else
        {
            PPANEL_CONTEXT panel = &device->Panels[panelIndex];

            status = ProcessColorPayload(panel, inputBuffer, inputLength);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "ProcessColorPayload failed %!STATUS!", status);
            }
            else
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionFlushProperties | SDP_ACTION_FLAG_USER, NULL, NULL);
                if (!NT_SUCCESS(status))
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Flushing the calibration failed %!STATUS!", status);
                }
            }
        }
    }
    else if (IoControlCode == IOCTL_SDP_APPLY_THERMAL_LIMIT)
    {
        // Input: { ULONG DisplayId, ULONG ThrottleValue }.
        PULONG input = (PULONG)inputBuffer;
        SDP_TEMP_RESULT result = {0};
        UCHAR useAllPanels = TRUE;

        ULONG panelIndex = ConvertQDIDisplayIDtoDisplayIndex(input[0]);
        if (panelIndex == SDP_INVALID_PANEL_INDEX)
        {
            status = STATUS_INVALID_PARAMETER;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Invalid display id %u", input[0]);
        }
        else
        {
            PPANEL_CONTEXT panel = &device->Panels[panelIndex];

            TraceEvents(
                TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "Thermal limit request: display %u value %u", input[0], input[1]);

            TraceLoggingWrite(
                g_SdpTraceLoggingProvider,
                "TEMPERATURE_COLOR_COMPENSATION",
                TraceLoggingLevel(SDP_TL_LEVEL_VERBOSE),
                TraceLoggingKeyword(SDP_KEYWORD_THERMAL),
                TraceLoggingUInt32(input[0], "MonitorID "),
                TraceLoggingUInt32(input[1], "ThrottleValue "));

                        status = AddDisplayActionRequestSynchronous(panel, DisplayActionTemperatureCompensation | SDP_ACTION_FLAG_USER, input, &result);
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
    }
    else if (IoControlCode == IOCTL_SDP_WAIT_PANEL_STATE)
    {
        if (InputBufferLength == sizeof(SDP_PANEL_STATE_NOTIFICATION))
        {
            ULONG panelIndex = ConvertQDIDisplayIDtoDisplayIndex(*(PULONG)inputBuffer);
            if (panelIndex == SDP_INVALID_PANEL_INDEX)
            {
                status = STATUS_INVALID_PARAMETER;
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Invalid display id %u", *(PULONG)inputBuffer);
            }
            else
            {
                // The status thread completes the request when the panel state changes.
                status = WdfRequestForwardToIoQueue(Request, device->Panels[panelIndex].StatusQueue);
                completeRequest = !NT_SUCCESS(status);
                if (completeRequest)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestForwardToIoQueue failed %!STATUS!", status);
                }
            }
        }
        else
        {
            bytesReturned = 0;
            status = STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        status = HandleTools_Ioctl(
            device,
            IoControlCode,
            (PSDP_IOCTL_HEADER)inputBuffer,
            inputLength,
            outputBuffer,
            outputLength,
            &bytesReturned);
    }

Complete:
    if (completeRequest)
    {
        WdfRequestCompleteWithInformation(Request, status, bytesReturned);
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Request forwarded to the status queue");
    }
}
