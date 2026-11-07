// Action dispatcher executed on the display worker thread.

#include "SdpDriver.h"
#include "ActionDispatch.tmh"

// Names of the actions for the event log
PCWSTR
GetDisplayActionName(_In_ LONG Action)
{
    if (Action == DisplayActionExitThread)
    {
        return L"ExitDisplayThread";
    }

    switch (Action)
    {
        case DisplayActionNone: return L"DisplayActionNone";
        case DisplayActionOn: return L"DisplayActionON";
        case DisplayActionOff: return L"DisplayActionOFF";
        case DisplayActionInitSequence: return L"InitSequence";
        case DisplayActionTermSequence: return L"TermSeqeunce";     // sic
        case DisplayActionDisplayId: return L"DisplayID";
        case DisplayActionSetBrightness: return L"SetBrightness";
        case DisplayActionDriverInit: return L"DriverInit";
        case DisplayActionDriverTerminate: return L"DriverTerminate";
        case DisplayActionSetMode: return L"SetMode";
        case DisplayActionPower: return L"Power";
        case DisplayActionPrePower: return L"PrePower";
        case DisplayActionPostPower: return L"PostPower";
        case DisplayActionDsiSendCmd: return L"DSISendCmd";
        case DisplayActionDsiReadCmd: return L"DSIReadCmd";
        case DisplayActionToggleReset: return L"ToggleReset";
        case DisplayActionDriverLoaded: return L"DriverLoaded";
        case DisplayActionDisplayReset: return L"DisplayReset";
        case DisplayActionFrameBufferInfo: return L"FrameBuferInfo";       // sic
        case DisplayActionUpdateFrameBuffer: return L"UpdateFrameBuffer";
        case DisplayActionFrameBufferClear: return L"FrameBufferClear";
        case DisplayActionGetDisplayPropertyInfo: return L"GetDisplayPropertyInfo";
        case DisplayActionSetDisplayPropertyInfo: return L"SetDisplayPropertyInfo";
        case DisplayActionSetPixelReg: return L"SetPixel Reg";
        case DisplayActionGetBrightness: return L"GetBrightness";
        case DisplayActionGetMfgInfo: return L"GetMfg Info";
        case DisplayActionGetPowerMode: return L"GetPowerMode";
        case DisplayActionSelfTest: return L"SelfTest";
        default: return L"Invalid Action";
    }
}

// Names of the QDI panel events used in traces
PCWSTR
GetQDIEventName(_In_ ULONG Event)
{
    switch (Event)
    {
        case QDI_PANEL_EVENT_POST_POWER: return L"QDI_PANEL_EVENT_POST_POWER";
        case QDI_PANEL_EVENT_UPDATE_REQUEST: return L"QDI_PANEL_EVENT_UPDATE_REQUEST";
        case QDI_PANEL_EVENT_INIT_SEQUENCE: return L"QDI_PANEL_EVENT_INIT_SEQUENCE";
        case QDI_PANEL_EVENT_TERM_SEQUENCE: return L"QDI_PANEL_EVENT_TERM_SEQUENCE";
        case QDI_PANEL_EVENT_RESET_START: return L"QDI_PANEL_EVENT_RESET_START";
        case QDI_PANEL_EVENT_RESET_COMPLETE: return L"QDI_PANEL_EVENT_RESET_COMPLETE";
        case QDI_PANEL_EVENT_HIST_DATA_AVAIL: return L"QDI_PANEL_EVENT_HIST_DATA_AVAIL";
        case QDI_PANEL_EVENT_PRE_POWER: return L"QDI_PANEL_EVENT_PRE_POWER";
        case QDI_PANEL_EVENT_DRIVER_LOADED: return L"QDI_PANEL_EVENT_DRIVER_LOADED";
        case QDI_PANEL_EVENT_VSYNC_IRQ: return L"QDI_PANEL_EVENT_VSYNC_IRQ";
        case QDI_PANEL_EVENT_VSYNC_ENABLED: return L"QDI_PANEL_EVENT_VSYNC_ENABLED";
        case QDI_PANEL_EVENT_VSYNC_DISABLED: return L"QDI_PANEL_EVENT_VSYNC_DISABLED";
        default: return L"UNKNOWN_QDI_EVENT";
    }
}

// The panel that is not Panel
static PPANEL_CONTEXT
GetAlternatePanelContext(_In_ PPANEL_CONTEXT Panel)
{
    PDEVICE_CONTEXT device = DeviceGetContext(Panel->Device);

    if (Panel->DisplayId == QDI_DISPLAY_PRIMARY)
    {
        return &device->Panels[1];
    }
    if (Panel->DisplayId == QDI_DISPLAY_SECONDARY)
    {
        return &device->Panels[0];
    }

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No peer panel for display %u", Panel->DisplayId);
    return NULL;
}

static NTSTATUS
GetFrameBufferInfo(_In_ PPANEL_CONTEXT Panel)
{
    QDI_SurfaceAttrType surface;
    NTSTATUS status = STATUS_SUCCESS;

    if (Panel->DisplayUtil.QDI_DisplayUtil_LinearFrameBufferInfo != NULL)
    {
        QDI_ErrorType qdiStatus =
            Panel->DisplayUtil.QDI_DisplayUtil_LinearFrameBufferInfo(Panel->DisplayId, &surface, 0);

        // Only the packed / RGB formats are exposed to the clients.
        if (qdiStatus == QDI_STATUS_OK && (INT)surface.ePixelFormat > 0 &&
            (surface.ePixelFormat < 0xC || (surface.ePixelFormat > 0x18 && surface.ePixelFormat < 0x1B)))
        {
            Panel->FbWidth = (USHORT)surface.uWidthInPixel;
            Panel->FbHeight = (USHORT)surface.uHeightInPixel;
            Panel->FbStride = (USHORT)surface.uPlanes.sRgb.iStrideInByte;
            Panel->FbBuffer = NULL;
            status = STATUS_SUCCESS;
        }
    }
    return status;
}
static VOID
FillFrameBufferInfo(_In_ PPANEL_CONTEXT Panel, _Out_ SDP_FRAMEBUFFER_UPDATE* Info)
{
    Info->Width = Panel->FbWidth;
    Info->Height = Panel->FbHeight;
    Info->Stride = Panel->FbStride;
    Info->Data = Panel->FbBuffer;
}

static NTSTATUS
SetFrameBufferUpdate(_In_ PPANEL_CONTEXT Panel, _In_ SDP_FRAMEBUFFER_UPDATE* Update)
{
    if (Update == NULL || Update->Data == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->FbHeight < Update->Height || Panel->FbWidth < Update->Width || Update->Stride != Panel->FbStride)
    {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    Panel->DisplayUtil.QDI_DisplayUtil_LinearFrameBufferUpdate(
        Panel->DisplayId, Update->Data, Update->Width, Update->Height, Update->Stride, 0);
    return STATUS_SUCCESS;
}

// Scales a request to the lowest nits / percent allowed by both panels and the
// thermal interface and programs it.
static NTSTATUS
SdpApplyBrightnessNits(_In_ PPANEL_CONTEXT Panel, _In_ ULONG* Nits, _In_opt_ UCHAR* AllPanelsFlag)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG dbv = 0;
    UCHAR allPanels = 0;
    PDEVICE_CONTEXT device = DeviceGetContext(Panel->Device);
    PPANEL_CONTEXT peer = GetAlternatePanelContext(Panel);

    if (Panel->ThermalMaxNits < Nits[0])
    {
        TraceEvents(
            TRACE_LEVEL_INFORMATION, TRACE_THREAD, "Nits %u limited to %u", Nits[0], Panel->ThermalMaxNits);
        Nits[0] = Panel->ThermalMaxNits;
    }

    if (AllPanelsFlag != NULL)
    {
        allPanels = *AllPanelsFlag;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_THREAD,
        "SetBrightnessNits %u (transition %u ms) all panels %s",
        Nits[0],
        Nits[1],
        allPanels ? "TRUE" : "FALSE");

    // Lowest requested nits and thermal cap of both panels.
    ULONG requested = min(Panel->RequestedNits, peer->RequestedNits);
    ULONG capNits = min(Panel->ThermalMaxNits, peer->ThermalMaxNits);
    ULONG limitNits = min(requested, capNits);
    ULONG limitPercent = min(Panel->ThermalPercent, peer->ThermalPercent);
    ULONG percent = min(device->ThermalPercent, limitPercent);

    if (allPanels == 0)
    {
        if (percent != 0)
        {
            Nits[0] = (percent * Nits[0]) / 100;
        }

        if (Panel->Ops.NitsToDbv != NULL)
        {
            Panel->Ops.NitsToDbv(Panel, Nits[0], &dbv);
        }

        if (Panel->Ops.SetDbv != NULL)
        {
            Panel->LastNitsScaled = Nits[0];
            Panel->Brightness = dbv;
            status = Panel->Ops.SetDbv(Panel, dbv);
        }
    }
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_THREAD, "Applying %u nits at %u percent to all panels", limitNits, percent);

        for (ULONG i = 0; i < device->PanelCount; i++)
        {
            PPANEL_CONTEXT target = &device->Panels[i];
            ULONG scaled = limitNits;

            if (percent != 0)
            {
                scaled = (percent * limitNits) / 100;
            }

                        if (target->Ops.NitsToDbv != NULL)
            {
                target->Ops.NitsToDbv(Panel, scaled, &dbv);
            }

            if (target->Ops.SetDbv != NULL)
            {
                target->LastNitsScaled = scaled;
                target->Brightness = dbv;
                status = Panel->Ops.SetDbv(target, dbv);
            }
        }
    }
    return status;
}

// Runs one action The status is also stored in *Node->Status
// when the requester waits for it, and failures are reported to the event log
// and as a TraceLogging event.
NTSTATUS
ProcessDisplayAction(_In_ PPANEL_CONTEXT Panel, _Inout_ SDP_ACTION_NODE* Node)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG newState = Panel->State;
    ULONG byteValue = 0;
    PLONG resultPointer = Node->Status;

    BOOLEAN fromUser = (LONG)Node->Action < 0;
    if (fromUser)
    {
        Node->Action &= ~SDP_ACTION_FLAG_USER;
    }
    Node->Action &= ~SDP_ACTION_FLAG_QDI;

    TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_THREAD, "Processing action 0x%x", Node->Action);

    if (Node->Action == DisplayActionExitThread)
    {
        if (Panel->Ops.ResetSequences != NULL)
        {
            Panel->Ops.ResetSequences(Panel);
        }
    }
    else
    {
        switch (Node->Action)
        {
            case DisplayActionNone:
                TraceEvents(TRACE_LEVEL_WARNING, TRACE_THREAD, "DisplayActionNone received");
                break;

            case DisplayActionOn:
            {
                UCHAR reset = (Node->Arg1 != NULL) ? *(UCHAR*)Node->Arg1 : 0;
                if (reset == 0)
                {
                    status = SendDisplayOnOffAction(Panel, TRUE);
                }
                else
                {
                    ForcePanelReInit(Panel);
                }
                break;
            }

            case DisplayActionOff:
            {
                UCHAR sleep = (Node->Arg1 != NULL) ? *(UCHAR*)Node->Arg1 : 0;
                if (sleep == 0)
                {
                    status = SendDisplayOnOffAction(Panel, FALSE);
                    Panel->DisplayOff = TRUE;
                }
                else if (Panel->Ops.SendTermSequence == NULL)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No term sequence handler");
                    status = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    Panel->PanelAsleep = TRUE;
                    Panel->DisplayOff = TRUE;
                    status = Panel->Ops.SendTermSequence(Panel);
                }
                break;
            }

            case DisplayActionInitSequence:
                if (Panel->Ops.SendInitSequence == NULL)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No init sequence handler");
                    status = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    status = Panel->Ops.SendInitSequence(Panel);
                }

                if (NT_SUCCESS(status))
                {
                    KeAcquireGuardedMutex(&Panel->Lock);
                    Panel->PanelAsleep = FALSE;
                    Panel->DisplayOff = FALSE;
                    KeReleaseGuardedMutex(&Panel->Lock);
                    newState = SDP_PANEL_STATE_BRIGHTNESS;
                }
                break;

            case DisplayActionTermSequence:
                if (Panel->Ops.SendTermSequence == NULL)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No term sequence handler");
                    status = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    status = Panel->Ops.SendTermSequence(Panel);
                }

                if (NT_SUCCESS(status))
                {
                    KeAcquireGuardedMutex(&Panel->Lock);
                    Panel->PanelAsleep = TRUE;
                    Panel->DisplayOff = TRUE;
                    KeReleaseGuardedMutex(&Panel->Lock);
                    newState = SDP_PANEL_STATE_OFF;
                }
                break;

            case DisplayActionDisplayId:
                                break;

            case DisplayActionSetBrightness:
                byteValue = *(UCHAR*)Node->Arg1;
                if (Panel->Ops.SetDbv == NULL)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No brightness handler");
                    status = STATUS_UNSUCCESSFUL;
                }
                else
                {
                    if (byteValue != 0)
                    {
                        Panel->Brightness = byteValue;
                    }
                    status = Panel->Ops.SetDbv(Panel, byteValue);
                }

                if (NT_SUCCESS(status))
                {
                    newState = SDP_PANEL_STATE_BRIGHTNESS;
                }
                break;

            case DisplayActionDriverInit:
                if (!Panel->InfoInitialized)
                {
                    Qdi_OemGetPanelID(Panel, (PULONG)Node->Arg1);
                    Init_PanelActionTable(Panel);

                    if (Panel->Ops.SetCaps != NULL && (Panel->Caps & SDP_CAPS_NORMAL_MODE) != 0)
                    {
                        Panel->Ops.SetCaps(Panel);
                    }
                    if (Panel->Ops.InitPanelInfo != NULL)
                    {
                        Panel->Ops.InitPanelInfo(Panel);
                    }
                    if (Panel->Ops.BuildSequences != NULL)
                    {
                        Panel->Ops.BuildSequences(Panel);
                    }
                }

                status = StartDisplayInitAction(Panel, (QDI_Panel_AttrType*)Node->Arg1);
                if (NT_SUCCESS(status))
                {
                    newState = SDP_PANEL_STATE_INITIALIZED;
                }
                break;

            case DisplayActionDriverTerminate:
                newState = SDP_PANEL_STATE_OFF;
                break;

            case DisplayActionSetMode:
                status = FillPanelConfiguration((QDI_Panel_AttrType*)Node->Arg1, &Panel->Info, Panel);
                if (NT_SUCCESS(status))
                {
                    newState = SDP_PANEL_STATE_MODE_SET;
                }
                break;

            case DisplayActionPower:
                Panel->PowerOn = (*(UCHAR*)Node->Arg1 != 0);
                newState = (*(UCHAR*)Node->Arg1 == 0) ? SDP_PANEL_STATE_POWER_OFF : SDP_PANEL_STATE_POWER_ON;
                break;

            case DisplayActionPrePower:
                newState = SDP_PANEL_STATE_OFF;
                break;

            case DisplayActionPostPower:
                if (*(UCHAR*)Node->Arg1 == 0)
                {
                    newState = SDP_PANEL_STATE_POWER_OFF;
                    status = ToggleResetLine(&Panel->ResetGpio, SDP_RESET_GPIO_ASSERT);
                    KeSetEvent(&Panel->PanelOffEvent, 0, FALSE);
                }
                else
                {
                    newState = SDP_PANEL_STATE_POWER_ON;
                    KeClearEvent(&Panel->PanelOffEvent);
                    status = ToggleResetLine(&Panel->ResetGpio, SDP_RESET_GPIO_DEASSERT);
                }
                break;

            case DisplayActionDsiSendCmd:
                if (fromUser)
                {
                    status = ProcessIoctlWriteBuffer(Panel, (PUCHAR)Node->Arg1, (PULONG)Node->Arg2);
                }
                break;

            case DisplayActionDsiReadCmd:
                if (fromUser)
                {
                    status = ProcessIoctlReadBuffer(Panel, (SDP_DSI_READ_REQUEST*)Node->Arg1, (PUCHAR)Node->Arg2);
                }
                break;

            case DisplayActionToggleReset:
                status = ToggleResetLine(&Panel->ResetGpio, SDP_RESET_GPIO_PULSE);
                KeAcquireGuardedMutex(&Panel->Lock);
                Panel->PanelAsleep = TRUE;
                Panel->DisplayOff = TRUE;
                KeReleaseGuardedMutex(&Panel->Lock);
                newState = SDP_PANEL_STATE_RESET_DONE;
                break;

            case DisplayActionDriverLoaded:
                newState = SDP_PANEL_STATE_RESET_DONE;
                break;

            case DisplayActionDisplayReset:
                                break;

            case DisplayActionFrameBufferInfo:
                status = GetFrameBufferInfo(Panel);
                if (NT_SUCCESS(status))
                {
                    FillFrameBufferInfo(Panel, (SDP_FRAMEBUFFER_UPDATE*)Node->Arg1);
                }
                break;

            case DisplayActionUpdateFrameBuffer:
                status = SetFrameBufferUpdate(Panel, (SDP_FRAMEBUFFER_UPDATE*)Node->Arg1);
                break;

            case DisplayActionFrameBufferClear:
                RtlZeroMemory(&Panel->FbWidth, 0x10);
                break;

            case DisplayActionGetDisplayPropertyInfo:
            case DisplayActionSetDisplayPropertyInfo:
                                break;

            case DisplayActionSetPixelReg:
                byteValue = *(UCHAR*)Node->Arg1;
                if (Panel->Ops.SetPixelReg != NULL)
                {
                    UCHAR value = (UCHAR)byteValue;
                    status = Panel->Ops.SetPixelReg(Panel, &value);
                }
                break;

            case DisplayActionGetBrightness:
                if (Panel->Ops.GetBrightness != NULL)
                {
                    status = Panel->Ops.GetBrightness(Panel, (UCHAR*)Node->Arg1);
                }
                break;

            case DisplayActionGetMfgInfo:
                if (Node->Arg1 != NULL)
                {
                    if (Panel->Ops.GetMfgInfo != NULL)
                    {
                        status = Panel->Ops.GetMfgInfo(Panel, (ULONG*)Node->Arg1);
                    }
                    if (NT_SUCCESS(status))
                    {
                        RtlCopyMemory(Panel->MfgInfo, Node->Arg1, sizeof(Panel->MfgInfo));
                    }
                }
                break;

            case DisplayActionGetPowerMode:
                if (Node->Arg1 != NULL && Panel->Ops.GetPowerMode != NULL)
                {
                    status = Panel->Ops.GetPowerMode(Panel, (CHAR*)Node->Arg1, (UCHAR*)Node->Arg2);
                }
                break;

            case DisplayActionSelfTest:
                if (Node->Arg1 != NULL && Panel->Ops.GetDiagnostic != NULL)
                {
                    status = Panel->Ops.GetDiagnostic(Panel, (UCHAR*)Node->Arg1);
                }
                break;

            case DisplayActionFlushProperties:
            {
                SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)Panel->Calibration;

                if ((calibration->DirtyFlags &
                     (SDP_CAL_DIRTY_COLOR_MATRIX | SDP_CAL_DIRTY_INVERSE_GAMMA | SDP_CAL_DIRTY_LINEAR_GAMMA |
                      SDP_CAL_DIRTY_GAMUT_MAPPING | SDP_CAL_DIRTY_DITHERING)) != 0)
                {
                    QDI_ErrorType qdiStatus =
                        Panel->DisplayUtil.QDI_DisplayUtil_SetProperty(Panel->DisplayId, &Panel->Properties, 0);
                    if (qdiStatus != QDI_STATUS_OK)
                    {
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "QDI SetProperty failed %u", qdiStatus);
                        status = STATUS_UNSUCCESSFUL;
                    }
                }

                if ((calibration->DirtyFlags & SDP_CAL_DIRTY_BRIGHTNESS) != 0)
                {
                    ULONG nits = calibration->Brightness.Nits;
                    ULONG transition = calibration->Brightness.TransitionMs;

                    if (Panel->Ops.SetDbv == NULL)
                    {
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No brightness handler");
                        status = STATUS_UNSUCCESSFUL;
                    }
                    else
                    {
                        Panel->Brightness = nits;
                        Panel->Brightness2 = transition;
                        status = Panel->Ops.SetDbv(Panel, nits);
                    }
                }

                calibration->DirtyFlags = 0;
                RtlZeroMemory(&Panel->Properties, sizeof(Panel->Properties));
                break;
            }

            case DisplayActionCommitProperties:
            {
                QDI_Panel_CommitConfigType commit = {0};
                QDI_Display_PropertyConfigType properties;

                RtlZeroMemory(&properties, sizeof(properties));
                properties.pCommitConfig = &commit;

                QDI_ErrorType qdiStatus = Panel->DisplayUtil.QDI_DisplayUtil_SetProperty(Panel->DisplayId, &properties, 0);
                if (qdiStatus != QDI_STATUS_OK)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "QDI commit failed %u", qdiStatus);
                    status = STATUS_UNSUCCESSFUL;
                }
                break;
            }

            case DisplayActionDualPanelSyncTest:
            {
                PPANEL_CONTEXT peer = GetAlternatePanelContext(Panel);
                ULONG* result = (ULONG*)Node->Arg1;

                Panel->VsyncMeasure = TRUE;
                peer->VsyncMeasure = TRUE;
                DelayMilliseconds(1000);

                if (result != NULL && peer != NULL && Panel->Ops.DualPanelSyncTest != NULL)
                {
                    status = Panel->Ops.DualPanelSyncTest(Panel, peer, result);
                }

                Panel->VsyncMeasure = FALSE;
                peer->VsyncMeasure = FALSE;
                break;
            }

            case DisplayActionSetBrightnessNits:
                status = SdpApplyBrightnessNits(Panel, (ULONG*)Node->Arg1, (UCHAR*)Node->Arg2);
                break;

            case DisplayActionEsdReset:
                ForcePanelReInit(Panel);
                break;

            case DisplayActionReserved41:
                                break;

            case DisplayActionInitPanelAttr:
                status = StartDisplayInitAction(Panel, (QDI_Panel_AttrType*)Node->Arg1);
                break;

            case DisplayActionTemperatureCompensation:
            {
                ULONG temperature = *(ULONG*)Node->Arg1;
                SDP_TEMP_RESULT* result = (SDP_TEMP_RESULT*)Node->Arg2;

                if ((Panel->Caps & SDP_CAPS_NO_TEMP_COMPENSATION) == 0)
                {
                    if (Panel->Ops.TemperatureCompensate != NULL)
                    {
                        Panel->Ops.TemperatureCompensate(Panel, temperature, result);
                    }
                    if (result->Percent != 0)
                    {
                        Panel->ThermalPercent = result->Percent;
                    }
                }
                else
                {
                    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_THREAD, "Temperature compensation disabled");
                }
                break;
            }

            case DisplayActionApplyAlEnhancement:
            {
                SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)Panel->Calibration;
                QDI_ErrorType qdiStatus = QDI_STATUS_OK;

                if ((calibration->DirtyFlags & SDP_CAL_DIRTY_AL_ENHANCEMENT) == 0)
                {
                    if ((calibration->DirtyFlags & SDP_CAL_DIRTY_AMBIENT_LEVEL) != 0)
                    {
                        Panel->Properties.pAmbientLightLevel = &calibration->AmbientLightLevel;
                    }
                }
                else
                {
                    calibration->AlEnable.bALEnhancementEnable = calibration->AlRequest.Enable;
                    calibration->AlCalibration.uCalibrationA = calibration->AlRequest.CalibrationA;
                    calibration->AlCalibration.uCalibrationB = calibration->AlRequest.CalibrationB;
                    calibration->AlCalibration.uCalibrationC = calibration->AlRequest.CalibrationC;
                    calibration->AlCalibration.uCalibrationD = calibration->AlRequest.CalibrationD;
                    calibration->AlCalibration.uMaxIterations = calibration->AlRequest.MaxIterations;
                    calibration->AlCalibration.uStrengthLimit = calibration->AlRequest.StrengthLimit;
                    calibration->AlCalibration.uTFilterControl = calibration->AlRequest.TFilterControl;

                    Panel->Properties.pALEnhancementCalibration = &calibration->AlCalibration;
                    Panel->Properties.pALEnhancementEnableType = &calibration->AlEnable;
                    Panel->Properties.pAmbientLightLevel = &calibration->AmbientLightLevel;
                }

                qdiStatus = Panel->DisplayUtil.QDI_DisplayUtil_SetProperty(Panel->DisplayId, &Panel->Properties, 0);
                if (qdiStatus != QDI_STATUS_OK)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "QDI SetProperty (AL enhancement) failed %u", qdiStatus);
                    status = STATUS_UNSUCCESSFUL;
                }
                break;
            }

            default:
                status = STATUS_INVALID_PARAMETER;
                break;
        }
    }

    if (newState != Panel->State)
    {
        Panel->State = newState;
    }

    if (resultPointer != NULL)
    {
        *resultPointer = status;
        if (!NT_SUCCESS(status))
        {
            PCWSTR actionName = GetDisplayActionName((LONG)Node->Action);

            Utility_EventLogEntryWrite(Panel->Device, SDP_MSG_DISPLAY_ACTION_FAILED, status, 0, 0, NULL, 1, &actionName, 1);

            TraceLoggingWrite(
                g_SdpTraceLoggingProvider,
                "ProcessDisplayActionFailed",
                TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
                TraceLoggingHexUInt32(Node->Action, "DisplayAction"));
        }
    }
    return status;
}
