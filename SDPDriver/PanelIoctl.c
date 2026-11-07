// Handlers of the SDP_IOCTL_HEADER based IOCTLs

#include "SdpDriver.h"
#include "PanelIoctl.tmh"

// Sets bit Bit and bit 0 of a failure mask
static VOID
FillMetaResetTestResults(_Inout_opt_ PULONG Mask, _In_ ULONG Bit)
{
    if (Mask != NULL)
    {
        *Mask |= (1u << (Bit & 0x1F)) | 1u;
    }
}

// Self test: dual panel sync, power mode before/after a reset, then an ESD
// reset to leave the panel in a defined state.
static NTSTATUS
SdpPanelSelfTest(_In_ PPANEL_CONTEXT Panel, _Out_ SDP_SELF_TEST_RESULT* Result)
{
    ULONG failureMask = 0;
    ULONG syncResult = 0;
    ULONG diagnostic = 0;
    CHAR powerMode = 0;
    ULONG attempt = 0;

    if (Panel->PanelAsleep == TRUE || Panel->DisplayOff == TRUE)
    {
        return STATUS_UNSUCCESSFUL;
    }

    do
    {
        AddDisplayActionRequestSynchronous(Panel, DisplayActionDualPanelSyncTest | SDP_ACTION_FLAG_USER, &syncResult, NULL);
        attempt++;
        if (syncResult == 0)
        {
            break;
        }
    } while (attempt < 5);

    // The diagnostic result is read but the binary does not evaluate it.
    AddDisplayActionRequestSynchronous(Panel, DisplayActionSelfTest | SDP_ACTION_FLAG_USER, &diagnostic, NULL);

    NTSTATUS status = AddDisplayActionRequestSynchronous(Panel, DisplayActionGetPowerMode | SDP_ACTION_FLAG_USER, &powerMode, NULL);
    if ((UCHAR)powerMode != 0x9C || !NT_SUCCESS(status))
    {
        FillMetaResetTestResults(&failureMask, 1);
    }

    status = AddDisplayActionRequestSynchronous(Panel, DisplayActionToggleReset | SDP_ACTION_FLAG_USER, NULL, NULL);
    if (!NT_SUCCESS(status))
    {
        FillMetaResetTestResults(&failureMask, 2);
    }

    status = AddDisplayActionRequestSynchronous(Panel, DisplayActionGetPowerMode | SDP_ACTION_FLAG_USER, &powerMode, NULL);
    if ((UCHAR)powerMode != 0x88 || !NT_SUCCESS(status))
    {
        FillMetaResetTestResults(&failureMask, 3);
    }

    Result->Reserved = 0;
    Result->FailureMask = failureMask;
    Result->DualPanelSyncResult = syncResult;

    AddDisplayActionRequestSynchronous(Panel, DisplayActionEsdReset | SDP_ACTION_FLAG_USER, NULL, NULL);
    return STATUS_SUCCESS;
}

// Dispatches one of the panel IOCTLs. The request starts with an
// SDP_IOCTL_HEADER; the output is staged in a pool buffer and copied to the
// caller at the end.
NTSTATUS
HandleTools_Ioctl(
    _In_ PDEVICE_CONTEXT Device,
    _In_ ULONG IoControlCode,
    _In_ PSDP_IOCTL_HEADER Input,
    _In_ SIZE_T InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PUCHAR Output,
    _In_ SIZE_T OutputBufferLength,
    _Out_ PULONG BytesReturned)
{
    NTSTATUS status = STATUS_SUCCESS;
    PUCHAR payload = NULL;          // copy of the request payload
    PUCHAR staging = NULL;          // output staging buffer

    if (InputBufferLength == 0)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Panel IOCTL without input");
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto Cleanup;
    }

    ULONG panelIndex = ConvertQDIDisplayIDtoDisplayIndex(Input->DisplayId);
    if (panelIndex == SDP_INVALID_PANEL_INDEX)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Invalid display id %u", Input->DisplayId);
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    PPANEL_CONTEXT panel = &Device->Panels[panelIndex];
    ULONG inputSize = Input->InputSize;
    ULONG outputSize = Input->OutputSize;

    if (InputBufferLength < (SIZE_T)inputSize + FIELD_OFFSET(SDP_IOCTL_HEADER, Payload))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Input smaller than announced (%u)", inputSize);
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    if (OutputBufferLength < outputSize)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Output smaller than announced (%u)", outputSize);
        status = STATUS_BUFFER_TOO_SMALL;
        goto Cleanup;
    }

    if (inputSize != 0)
    {
        payload = (PUCHAR)ExAllocatePoolWithTag(PagedPool, inputSize, SDP_POOL_TAG);
        if (payload == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Allocating the input copy failed");
            goto Cleanup;
        }
        RtlCopyMemory(payload, Input->Payload, inputSize);
    }

    if (OutputBufferLength != 0)
    {
        staging = (PUCHAR)ExAllocatePoolWithTag(PagedPool, max(outputSize, 4), SDP_POOL_TAG);
        if (staging == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Allocating the output staging buffer failed");
            goto Cleanup;
        }
        RtlZeroMemory(staging, outputSize);
    }

    switch (IoControlCode)
    {
        case IOCTL_SDP_NOP:
        case IOCTL_SDP_NOP2:
            break;

        case IOCTL_SDP_SET_BRIGHTNESS:
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionSetBrightness | SDP_ACTION_FLAG_USER, payload, NULL);
            break;

        case IOCTL_SDP_SET_DISPLAY_STATE:
        {
            UCHAR state = (inputSize != 0) ? payload[0] : 0;

            if (panel->DisplayOff == TRUE && state == 0)
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionOn | SDP_ACTION_FLAG_USER, &state, NULL);
                if (!NT_SUCCESS(status))
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Display on failed %!STATUS!", status);
                }
            }
            else if (panel->PanelAsleep == TRUE && state == 1)
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionEsdReset | SDP_ACTION_FLAG_USER, NULL, NULL);
            }
            break;
        }

        case IOCTL_SDP_DSI_WRITE:
        {
            ULONG size = inputSize;
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionDsiSendCmd | SDP_ACTION_FLAG_USER, payload, &size);
            break;
        }

        case IOCTL_SDP_DSI_READ:
        {
            if (OutputBufferLength < outputSize || staging == NULL)
            {
                status = STATUS_INVALID_PARAMETER;
                goto Cleanup;
            }

            SDP_DSI_READ_REQUEST request;
            request.Data = (PUCHAR)ExAllocatePoolWithTag(PagedPool, inputSize, SDP_POOL_TAG);
            if (request.Data == NULL)
            {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }

            request.DataSize = inputSize;
            request.ReadSize = outputSize;
            RtlCopyMemory(request.Data, payload, inputSize);
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionDsiReadCmd | SDP_ACTION_FLAG_USER, &request, staging);
            ExFreePoolWithTag(request.Data, SDP_POOL_TAG);
            break;
        }

        case IOCTL_SDP_TOGGLE_RESET:
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionToggleReset | SDP_ACTION_FLAG_USER, staging, NULL);
            break;

        case IOCTL_SDP_GET_BRIGHTNESS:
            if (outputSize != 1)
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            staging[0] = 0;
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionGetBrightness | SDP_ACTION_FLAG_USER, staging, NULL);
            break;

        case IOCTL_SDP_GET_FRAMEBUFFER_INFO:
            if (staging == NULL || OutputBufferLength != sizeof(SDP_FRAMEBUFFER_UPDATE))
            {
                status = STATUS_INVALID_DEVICE_REQUEST;
                goto Cleanup;
            }
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionFrameBufferInfo | SDP_ACTION_FLAG_USER, staging, NULL);
            break;

        case IOCTL_SDP_UPDATE_FRAMEBUFFER:
        {
                        // can only succeed once FbBuffer is set, which nothing does.
            SDP_FRAMEBUFFER_UPDATE update = {0};

            if (panel->FbStride == 0 || panel->FbHeight == 0 || panel->FbWidth == 0)
            {
                status = STATUS_BUFFER_TOO_SMALL;
                goto Cleanup;
            }
            if (panel->FbBuffer == NULL)
            {
                status = STATUS_INVALID_DEVICE_REQUEST;
                goto Cleanup;
            }
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionUpdateFrameBuffer | SDP_ACTION_FLAG_USER, &update, NULL);
            break;
        }

        case IOCTL_SDP_GET_CONTEXT_SIZE:
            if (outputSize < sizeof(ULONG))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                goto Cleanup;
            }
            *(PULONG)staging = sizeof(DEVICE_CONTEXT);
            break;

        case IOCTL_SDP_GET_CONTEXT:
            if (outputSize < sizeof(DEVICE_CONTEXT))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                goto Cleanup;
            }
            RtlCopyMemory(staging, Device, sizeof(DEVICE_CONTEXT));
            break;

        case IOCTL_SDP_MEASURE_VSYNC:
        {
            ULONG durationMs = 0;

            if (payload != NULL && InputBufferLength == 0x30)
            {
                SDP_MEASURE_VSYNC_REQUEST* request = (SDP_MEASURE_VSYNC_REQUEST*)payload;

                panel->VsyncSampleTarget = (request->SampleCount < 6) ? 5 : request->SampleCount;
                durationMs = (request->DurationMs < 0x24A) ? 0x249 : request->DurationMs;
            }

            panel->VsyncMeasure = TRUE;
            DelayMilliseconds(durationMs);
            panel->VsyncMeasure = FALSE;

            if (outputSize >= sizeof(ULONG))
            {
                RtlCopyMemory(staging, &panel->VsyncPeriodUs, sizeof(ULONG));
            }
            else if (outputSize == 0x20)
            {
                                *(PULONG)staging = panel->VsyncPeriodUs;
                RtlCopyMemory(staging + sizeof(ULONG), panel->VsyncDeltas, sizeof(ULONG));
            }
            panel->VsyncSampleTarget = 5;
            break;
        }

        case IOCTL_SDP_GET_DISPLAY_PROPERTIES:
            if (outputSize < 0x58)
            {
                status = STATUS_BUFFER_TOO_SMALL;
                goto Cleanup;
            }
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionGetDisplayPropertyInfo | SDP_ACTION_FLAG_USER, staging, NULL);
            break;

        case IOCTL_SDP_DISPLAY_OFF:
        {
            UCHAR state = (inputSize != 0) ? payload[0] : 0;

            if (panel->PanelAsleep == FALSE)
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionOff | SDP_ACTION_FLAG_USER, &state, NULL);
            }
            break;
        }

        case IOCTL_SDP_GET_PANEL_PARAMS:
            if (outputSize < 2 * sizeof(ULONG))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                goto Cleanup;
            }
            ((PULONG)staging)[0] = panel->Info.PanelParam0;
            ((PULONG)staging)[1] = panel->Info.PanelParam1;
            break;

        case IOCTL_SDP_SET_PIXEL_REG:
            if (inputSize != 1)
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionSetPixelReg | SDP_ACTION_FLAG_USER, payload, NULL);
            break;

        case IOCTL_SDP_GET_MFG_INFO:
            if (outputSize != sizeof(panel->MfgInfo))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionGetMfgInfo | SDP_ACTION_FLAG_USER, staging, NULL);
            break;

        case IOCTL_SDP_SELF_TEST:
            if (outputSize < sizeof(SDP_SELF_TEST_RESULT))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            status = SdpPanelSelfTest(panel, (SDP_SELF_TEST_RESULT*)staging);
            if (!NT_SUCCESS(status))
            {
                goto Cleanup;
            }
            break;

        case IOCTL_SDP_SET_CALIBRATION:
        {
            ULONG blobPanel = ConvertQDIDisplayIDtoDisplayIndex(((SDP_CAL_HEADER*)payload)->DisplayId);
            if (blobPanel > 0xE)
            {
                status = STATUS_INVALID_PARAMETER;
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Calibration blob display id invalid");
                goto Cleanup;
            }

            status = ProcessColorPayload(panel, payload, inputSize);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "ProcessColorPayload failed %!STATUS!", status);
                goto Cleanup;
            }

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionFlushProperties | SDP_ACTION_FLAG_USER, NULL, NULL);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Flushing the calibration failed %!STATUS!", status);
                goto Cleanup;
            }

            ULONG nits = ((SDP_CALIBRATION_BUFFER*)panel->Calibration)->Brightness.Nits;
            if (nits != 0)
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionSetBrightness | SDP_ACTION_FLAG_USER, &nits, NULL);
            }
            break;
        }

        case IOCTL_SDP_ESD_RESET:
            status = AddDisplayActionRequestSynchronous(panel, DisplayActionEsdReset | SDP_ACTION_FLAG_USER, NULL, NULL);
            break;

        case IOCTL_SDP_GET_VERSION:
        {
            if (outputSize != sizeof(SDP_VERSION_INFO))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            SDP_VERSION_INFO* version = (SDP_VERSION_INFO*)staging;
            version->Major = 2;
            version->Minor = 9;
            version->QdiPanelApiVersion = SDP_QDI_PANEL_API_VERSION;
            break;
        }

        case IOCTL_SDP_GET_DSI_ERRORS:
            if (outputSize != 3 * sizeof(ULONG))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }
            RtlCopyMemory(staging, &panel->DsiHostErrors, 3 * sizeof(ULONG));
            break;

        case IOCTL_SDP_GET_PANEL_STATUS:
        {
            if (outputSize != sizeof(SDP_PANEL_STATUS))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            SDP_PANEL_STATUS* report = (SDP_PANEL_STATUS*)staging;
            report->PanelAsleep = panel->PanelAsleep;
            report->DisplayOn = (panel->DisplayOff == FALSE);
            report->VsyncEnabled = panel->VsyncEnabled;
            report->Brightness = panel->Brightness;
            report->LastNitsScaled = panel->LastNitsScaled;
            report->ThermalMaxNits = panel->ThermalMaxNits;
            report->ThermalPercent = panel->ThermalPercent;
            report->VsyncPeriodUs = panel->VsyncPeriodUs;
            report->EsdCount = panel->EsdCount;
            if (panel->PanelBinSize == 0)
            {
                report->HasPanelBin = FALSE;
                RtlZeroMemory(report->PanelBin, sizeof(report->PanelBin));
            }
            else
            {
                report->HasPanelBin = TRUE;
                RtlCopyMemory(report->PanelBin, panel->PanelBin, panel->PanelBinSize);
            }
            report->Temperature = panel->Temperature;
            report->ThermalIndex = panel->ThermalIndex;
            break;
        }

        case IOCTL_SDP_SET_NITS:
        {
            ULONG request[2];

            if (inputSize != sizeof(ULONG))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            request[0] = *(PULONG)payload;
            request[1] = 0;

                        if (!panel->Ops.IsNitsValid(panel, request[0]))
            {
                status = STATUS_INVALID_PARAMETER;
            }
            else
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionSetBrightnessNits | SDP_ACTION_FLAG_QDI, request, NULL);
            }
            break;
        }

        case IOCTL_SDP_SET_AL_ENHANCEMENT:
        {
            if (inputSize < sizeof(SDP_AL_ENHANCEMENT_REQUEST))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)panel->Calibration;
            SDP_AL_ENHANCEMENT_REQUEST* request = (SDP_AL_ENHANCEMENT_REQUEST*)payload;

            calibration->AlRequest = *request;
            calibration->AlRequest.AmbientLevel = request->AmbientLevel & 0xFFFF;
            panel->HasCalibration = 1;
            calibration->DirtyFlags = SDP_CAL_DIRTY_AL_ENHANCEMENT;

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionApplyAlEnhancement | SDP_ACTION_FLAG_QDI, NULL, NULL);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_IOCTL, "Applying the AL enhancement failed %!STATUS!", status);
            }
            else
            {
                status = AddDisplayActionRequestSynchronous(panel, DisplayActionCommitProperties | SDP_ACTION_FLAG_QDI, NULL, NULL);
            }
            break;
        }

        case IOCTL_SDP_SET_DITHERING:
        {
            if (inputSize < sizeof(QDI_Panel_DitheringMatrixType))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)panel->Calibration;
            QDI_Panel_DitheringMatrixType* request = (QDI_Panel_DitheringMatrixType*)payload;

            calibration->Dithering.eDisplayId = (QDI_Display_IDType)panel->DisplayId;
            calibration->Dithering.bEnabled = request->bEnabled & 0xFF;
            calibration->Dithering.eMode = request->eMode;
            RtlCopyMemory(calibration->Dithering.uBitDepth, request->uBitDepth, sizeof(request->uBitDepth));
            RtlCopyMemory(
                calibration->Dithering.uDitheringMatrix, request->uDitheringMatrix, sizeof(request->uDitheringMatrix));

            panel->Properties.pDitheringMatrix = &calibration->Dithering;
            calibration->DirtyFlags |= SDP_CAL_DIRTY_DITHERING;

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionFlushProperties | SDP_ACTION_FLAG_USER, NULL, NULL);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Applying the dithering failed %!STATUS!", status);
            }
            break;
        }

        case IOCTL_SDP_SET_THERMAL_PERCENT:
        {
            ULONG value = *(PULONG)payload;
            SDP_TEMP_RESULT result = {0};
            UCHAR useAllPanels = TRUE;

            if ((LONG)value > 100)
            {
                status = STATUS_INVALID_PARAMETER;
                goto Cleanup;
            }

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionTemperatureCompensation | SDP_ACTION_FLAG_USER, &value, &result);
            if (NT_SUCCESS(status))
            {
                ULONG request[2];

                panel->ThermalMaxNits = result.MaxNits;
                panel->ThermalPercent = result.Percent;

                request[0] = result.MaxNits;
                request[1] = 0;
                status = AddDisplayActionRequestSynchronous(
                    panel, DisplayActionSetBrightnessNits | SDP_ACTION_FLAG_USER, request, &useAllPanels);
            }
            break;
        }

        case IOCTL_SDP_SET_BRIGHTNESS_OVERRIDE:
        {
            if (inputSize != 5)
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            BOOLEAN enable = (payload[0] != 0);
            LONG selector;
            RtlCopyMemory(&selector, payload + 1, sizeof(selector));

            if (selector == 1)
            {
                Device->BrightnessOverride = enable;
            }
            else
            {
                status = STATUS_INVALID_PARAMETER;
            }
            break;
        }

        case IOCTL_SDP_SET_AMBIENT_LEVEL:
        {
            if (inputSize < sizeof(ULONG))
            {
                status = STATUS_INVALID_BUFFER_SIZE;
                goto Cleanup;
            }

            ULONG ambientLevel = *(PULONG)payload;

            SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)panel->Calibration;
            calibration->AlRequest.AmbientLevel = ambientLevel & 0xFFFF;
            calibration->DirtyFlags = SDP_CAL_DIRTY_AMBIENT_LEVEL;

            status = AddDisplayActionRequestSynchronous(panel, DisplayActionApplyAlEnhancement | SDP_ACTION_FLAG_QDI, NULL, NULL);
            break;
        }

        default:
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Unknown panel IOCTL 0x%x", IoControlCode);
            break;
    }

    if (OutputBufferLength != 0)
    {
        *BytesReturned = outputSize;
        RtlCopyMemory(Output, staging, outputSize);
    }

Cleanup:
    if (payload != NULL)
    {
        ExFreePoolWithTag(payload, SDP_POOL_TAG);
    }
    if (staging != NULL)
    {
        ExFreePoolWithTag(staging, SDP_POOL_TAG);
    }
    return status;
}
