// Panel identification and the translation of the panel description into
// the QDI_Panel_AttrType handed to the Graphics KMD.

#include "SdpDriver.h"
#include "PanelConfig.tmh"

// Splits the id word of the QDI panel attributes (id in the high, version in
// the low half) and picks a usable id
NTSTATUS
Qdi_OemGetPanelID(_Inout_ PPANEL_CONTEXT Panel, _In_ ULONG* PanelIdWord)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Panel == NULL || PanelIdWord == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_INTERFACE, "Qdi_OemGetPanelID: invalid parameter");
    }
    else
    {
        if (*PanelIdWord == 0)
        {
            *PanelIdWord = 0x34160A00;
        }

        Panel->PanelId = *PanelIdWord >> 16;
        Panel->PanelVersion = *PanelIdWord & 0xFFFF;

        if (Panel->PanelId != SDP_PANEL_ID_3416 && Panel->PanelId != SDP_PANEL_ID_INVALID)
        {
            PDEVICE_CONTEXT device = DeviceGetContext(Panel->Device);

            // Panels that disagree on the id: follow the primary panel.
            if (device->Panels[0].PanelId != device->Panels[1].PanelId)
            {
                status = STATUS_SUCCESS;
                Panel->PanelId = device->Panels[0].PanelId;
            }
        }
    }

    if (!NT_SUCCESS(status) || Panel->PanelId == SDP_PANEL_ID_INVALID)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_INTERFACE, "Unknown panel id 0x%x", Panel->PanelId);
    }
    return status;
}

// Installs the operation table of the panel (only 0x3406 is known)
VOID
Init_PanelActionTable(_Inout_ PPANEL_CONTEXT Panel)
{
    const ULONG tableOffset = FIELD_OFFSET(SDP_PANEL_CONTEXT, Reserved1810);
    const ULONG tableSize = FIELD_OFFSET(SDP_PANEL_CONTEXT, EsdRecoveryPending) - tableOffset;

    C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, EsdRecoveryPending) - FIELD_OFFSET(SDP_PANEL_CONTEXT, Reserved1810) == 0x158);

    TraceLoggingWrite(
        g_SdpTraceLoggingProvider,
        "InitPanelActionTable",
        TraceLoggingLevel(SDP_TL_LEVEL_WARNING),
        TraceLoggingHexUInt32(Panel->PanelId, "PANELID"));

    // Clears the sequence heads as well.
    RtlZeroMemory((PUCHAR)Panel + tableOffset, tableSize);

    if (Panel->PanelId == SDP_PANEL_ID_3416)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_PANEL, "Installing the action table of panel 0x%x", Panel->PanelId);

        Panel->Ops.SetDbv = Set_Meta_Brightness;
        Panel->Ops.GetBrightness = Get_Meta_Brightness;
        Panel->Ops.BuildSequences = Init_PreAllocated_META2Ev3_Sequence;
        Panel->Ops.ResetSequences = Free_PreAllocated_META_Sequence;
        Panel->Ops.InitPanelInfo = FillMeta2PanelProperties;
        Panel->Ops.GetMfgInfo = Get_Meta2_Mfg_Info;
        Panel->Ops.SendInitSequence = Send_Meta2_Init_Sequence;
        Panel->Ops.SendTermSequence = Send_Meta_Terminate_Sequence;
        Panel->Ops.IsNitsValid = Validate_Meta_Brightness;
        Panel->Ops.NitsToDbv = GetDBV_Meta_BrightnessNits;
        Panel->Ops.GetDiagnostic = Get_Meta2_SelfDiagnostic_Info;
        Panel->Ops.DualPanelSyncTest = Get_Meta2_MiscDiagnostic_Info;
        Panel->Ops.GetPowerMode = Get_Meta2_PowerMode;
        Panel->Ops.SetPixelReg = Set_Meta2_SetPixelRegistration;
        Panel->Ops.TemperatureCompensate = Set_Meta2_Throttle;
        Panel->Ops.SetCaps = SetMeta2ManufacturingMode;
        Panel->Ops.ThermalThrottle = Set_PassiveCooling_Meta2;
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "No action table for panel id 0x%x", Panel->PanelId);
    }
}

// Default brightness description: ranges in milli-nits
VOID
FillBrightnessSpecificSettings(_Out_ QDI_Panel_BrightnessConfigType* Config)
{
    static const QDI_Panel_BrightnessConfigType defaults = {
        0x85259,    // uMaxLuminance
        10000,      // uMinLuminance
        9,          // uNumRanges
        {
            {10000, 18000, 100, 0},
            {0x46B4, 0xAF58, 0xB5, 0},
            {45000, 0x15E94, 0x1C4, 0},
            {0x15F90, 0x248E2, 0x389, 0},
            {0x249F0, 0x35916, 0x5E5, 0},
            {0x35B60, 0x492DC, 0x8A7, 0},
            {0x493E0, 0x63CD8, 0xBCE, 1},
            {0x64190, 0x7945A, 0x1022, 1},
            {0x7A120, 0x85259, 0x13B1, 1},
        },
    };

    RtlCopyMemory(Config, &defaults, sizeof(*Config));
}

// Default ambient light enhancement tuning
VOID
FillApicalSettings(_Out_ SDP_AL_ENHANCEMENT_REQUEST* Request)
{
    RtlZeroMemory(Request, sizeof(*Request));
    Request->Enable = 0;
    Request->MaxIterations = 0x40;
    Request->CalibrationA = 0x12;
    Request->CalibrationB = 0x5F;
    Request->CalibrationC = 0;
    Request->CalibrationD = 0;
    Request->StrengthLimit = 0xFF;
    Request->TFilterControl = 5;
}

// Default description of the 1350x1800 DSC panel on the given display
NTSTATUS
FillMeta2PanelProperties(_In_ PPANEL_CONTEXT Panel)
{
    SDP_PANEL_INFO* info = &Panel->Info;

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->DisplayId == QDI_DISPLAY_PRIMARY)
    {
        info->PhysConnect = QDI_DISPLAY_CONNECT_PRIMARY_DSI_CMD;
        info->VsyncSelect = 0;
    }
    else if (Panel->DisplayId == QDI_DISPLAY_SECONDARY)
    {
        info->PhysConnect = QDI_DISPLAY_CONNECT_SECONDARY_DSI_CMD;
        info->VsyncSelect = 1;
    }
    else
    {
        return STATUS_INVALID_PARAMETER;
    }

    info->DataLanes = 4;
    info->PanelId = SDP_PANEL_ID_3416;
    info->Reserved04 = 0;
    info->Reserved08 = 0;
    info->Reserved0C = 0;
    info->Reserved10 = 0;
    info->Reserved14 = 1;
    info->IsVideoMode = 0;
    info->LaneRemap = 0;
    info->LP11AtInit = 1;
    info->PhyDcdcMode = 0;
    info->DisableEotp = 0;
    info->DscEnable = 1;
    info->DscProfileId = 6;
    info->DscSliceWidth = 675;
    info->DscSliceHeight = 900;
    info->DscMajorVersion = 1;
    info->DscMinorVersion = 1;
    info->DscScrVersion = 0;
    info->BitClockOverride = 1;
    info->BitClockFrequency = 500000000;
    info->BitClkScaleOverride = 0;
    info->BitClkScalePercent = 10;

    if (Panel->DisplayId == QDI_DISPLAY_PRIMARY)
    {
        info->ControllerIndex = 0;
        info->ControllerMask = 1;
    }
    else if (Panel->DisplayId == QDI_DISPLAY_SECONDARY)
    {
        info->ControllerIndex = 1;
        info->ControllerMask = 2;
    }

    info->Width = 1350;
    info->Height = 1800;
    info->Reserved5C = 0;
    info->Reserved68 = 115;
    info->Reserved69 = 145;
    info->RefreshRate = 60;
    info->Reserved6C = 1000;
    info->DedicatedTePin = 1;
    info->VsyncStartPos = 2;
    info->VsyncContinueLines = 4000;
    info->VsyncStartLineDivisor = 4;
    info->VsyncPercentVariance = 0x630000;
    RtlZeroMemory(info->Reserved88, sizeof(info->Reserved88));
    info->ClockHsForceRequest = QDI_DSI_ClockLane_ForceHS;
    info->Reserved1[0] = 0x28;
    info->Reserved1[1] = 0x28;
    info->Reserved1[2] = 0x28;
    info->PanelParam0 = 0x40;
    info->PanelParam1 = info->PanelParam0 / 2 + 1;
    info->PowerSavingOverride = ((Panel->Caps & SDP_CAPS_POWERSAVE_LEVEL0) != 0) ? QDI_PANEL_POWERSAVING_INFO_LEVEL0 : 0;
    info->EnterUlps = 1;
    info->DsiFlags = QDI_PANEL_DSI_FLAG_DISABLE_PPS_CMD | QDI_PANEL_DSI_FLAG_DSI_DISABLE_BURST_MODE;
    info->DisplayFlags = QDI_PANEL_FLAG_FORCE_DISPLAY_REINIT | QDI_PANEL_FLAG_DISABLE_SBC | QDI_PANEL_FLAG_DISABLE_HW_CURSOR;
    info->EdidSerial = (Panel->DisplayId == QDI_DISPLAY_PRIMARY) ? 1 : 2;
    info->EdidWeek = 0;
    info->EdidYear = 0;

    Panel->ResetGpio.PostDelayUs = 10000;
    Panel->ResetGpio.PreDelayUs = 0;
    Panel->ResetGpio.HoldUs = 10000;
    Panel->ResetGpio.ActiveHigh = FALSE;
    Panel->ResetGpio.ExplicitLevels = TRUE;
    Panel->InfoInitialized = TRUE;

    FillBrightnessSpecificSettings(&Panel->BrightnessConfig);
    FillApicalSettings(&((SDP_CALIBRATION_BUFFER*)Panel->Calibration)->AlRequest);
    Panel->HasCalibration = 0;

    return STATUS_SUCCESS;
}

// Fills the QDI panel attributes from the panel description, the DSC / DSI
// settings and the calibration data
NTSTATUS
FillPanelConfiguration(_Out_ QDI_Panel_AttrType* Attr, _In_ const SDP_PANEL_INFO* Info, _In_ PPANEL_CONTEXT Panel)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Attr == NULL || Info == NULL || Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    QDI_Panel_DSIAttrType* dsi = &Attr->uAttrs.sDsi;
    SDP_CALIBRATION_BUFFER* calibration = (SDP_CALIBRATION_BUFFER*)Panel->Calibration;

    Attr->ePhysConnect = Info->PhysConnect;
    if (Attr->ePhysConnect == 0)
    {
        status = STATUS_BAD_DATA;
    }

    Attr->iVisHeightPx = Info->Height;
    Attr->iVisWidthPx = Info->Width;
    Attr->eColorFormat = QDI_PIXEL_FORMAT_RGB_888_24BPP;
    Attr->eColorOrder = QDI_COLOR_ORDERING_RGB;
    Attr->eColorComponentsPacking = QDI_COLOR_COMPONENTS_PACKING_TIGHT;
    Attr->eColorComponentAlignment = QDI_COLOR_COMPONENT_ALIGNMENT_LSB;
    Attr->eInterfaceMode = (Info->IsVideoMode == 0) ? QDI_PANEL_INTERFACE_COMMAND : QDI_PANEL_INTERFACE_ACTIVE;

    dsi->eColorFormat = QDI_DSICOLOR_RGB_888_24BPP;
    dsi->bDisableEotpAfterHSXfer = Info->DisableEotp;
    dsi->uRefreshRate = (uint32)Info->RefreshRate << 16;
    if (Info->BitClockOverride)
    {
        dsi->uBitClockFrequency = Info->BitClockFrequency;
    }
    if (Info->BitClkScaleOverride)
    {
        dsi->uBitClkScalePercent = Info->BitClkScalePercent;
    }
    dsi->uDataLaneNum = Info->DataLanes;
    dsi->uControllerMapping[0] = (uint8)Info->ControllerIndex;
    dsi->uControllerMappingMask = Info->ControllerMask;
    dsi->uLaneRemap = (QDI_DSILaneMapType)Info->LaneRemap;
    dsi->bPacketTransferHS = 0;
    dsi->eClockHSForceRequest = (QDI_DSI_ClockLaneType)Info->ClockHsForceRequest;
    dsi->bPhyDCDCMode = Info->PhyDcdcMode;
    dsi->bLP11AtInit = Info->LP11AtInit;
    Attr->sDSCConfig.uDSCProfileID = Info->DscProfileId;

    if (Info->IsVideoMode == 0)
    {
        // Command mode panel: no video timing, DSC and tearing effect setup instead.
        Attr->sActiveTiming.iHsyncPulseWidthDclk = 0;
        Attr->sActiveTiming.iHsyncBackPorchDclk = 0;
        Attr->sActiveTiming.iHsyncFrontPorchDclk = 0;
        Attr->sActiveTiming.iHsyncSkewDclk = 0;
        Attr->sActiveTiming.iHLeftBorderDClk = 0;
        Attr->sActiveTiming.iHRightBorderDClk = 0;
        Attr->sActiveTiming.iVsyncPulseWidthLines = 0;
        Attr->sActiveTiming.iVsyncFrontPorchLines = 0;
        Attr->sActiveTiming.iVsyncBackPorchLines = 0;
        Attr->sActiveTiming.iVTopBorderLines = 0;
        Attr->sActiveTiming.iVBottomBorderLines = 0;

        Attr->sDSCConfig.bDSCEnable = Info->DscEnable;
        Attr->sDSCConfig.uDSCProfileID = Info->DscProfileId;
        Attr->sDSCConfig.uDSCMajorVersion = Info->DscMajorVersion;
        Attr->sDSCConfig.uDSCMinorVersion = Info->DscMinorVersion;
        Attr->sDSCConfig.uDSCSliceHeight = Info->DscSliceHeight;
        Attr->sDSCConfig.uDSCSliceWidth = Info->DscSliceWidth;
        Attr->sDSCConfig.uDSCScrVersion = Info->DscScrVersion;

		// The LG / SiliconWorks panels on Surface Duo (1st Gen) will be already turned
		// on and functional when Windows is booting up. Unfortunately, these displays
		// also render frames before the GPU has finished initializing fully. The GPU
		// Driver only sets the DSC Compression Ratio value internally later in the
		// initialization process Causing wrong encoding to be used for a few seconds,
		// which can be both jarring and also send incorrect data to the display mux ic.
		// The documentation clearly states to not touch below value and that it is
		// read only, however, that's not true, the value is not read only And we force
		// it to be correct before the displays get turned on to ensure a smoother
		// transition in the OS.
		Attr->sDSCConfig.uDSCCompressionRatio = 3;

        dsi->bSwapInterface = 0;
        dsi->bUsingTrigger = 0;
        dsi->sTE.bTECheckEnable = 0;
        dsi->sTE.bDedicatedTEPin = Info->DedicatedTePin;
        dsi->sTE.vSyncStartPos = Info->VsyncStartPos;
        dsi->sTE.vSyncContinueLines = Info->VsyncContinueLines;
        dsi->sTE.vSyncStartLineDivisor = Info->VsyncStartLineDivisor;
        dsi->sTE.vSyncPosSafetyMargin = 5;
        dsi->sTE.vSyncBelowSafetyMargin = 5;
        dsi->sTE.vSyncPercentVariance = Info->VsyncPercentVariance;
        dsi->sTE.vSyncSelect = (QDI_ExternalVsyncSelectType)Info->VsyncSelect;

        dsi->bSendHsaHseAfterVsVe = 1;
        dsi->bLowPowerModeInHFP = 0;
        dsi->bLowPowerModeInHBP = 0;
        dsi->bLowPowerModeInHSA = 0;
        dsi->bLowPowerModeInBLLPEOF = 0;
        dsi->bLowPowerModeInBLLP = 0;
    }

    Attr->ePowerSavingOverride = (QDI_Panel_PowerSavingOverrideType)Info->PowerSavingOverride;
    dsi->bEnterULPSPowerDown = Info->EnterUlps;
    dsi->uFlags = (QDI_Panel_DSI_FlagsType)Info->DsiFlags;

    if (dsi->uEscapeClockDivisor != Info->EscClockDivisor)
    {
        dsi->uEscapeClockDivisor = Info->EscClockDivisor;
    }
    if (Info->EscClockFreq != 0)
    {
        dsi->uEscapeClockFreq = Info->EscClockFreq;
    }
    if (Info->DisplayFlags != 0)
    {
        Attr->eDisplayFlags = (QDI_Panel_FlagsType)Info->DisplayFlags;
    }

    Attr->sEDID.SerialNumber = Info->EdidSerial;
    if (Info->EdidWeek != 0)
    {
        Attr->sEDID.WeekOfManufacture = Info->EdidWeek;
    }
    if (Info->EdidYear != 0)
    {
        Attr->sEDID.YearOfManufacture = Info->EdidYear;
    }

    RtlCopyMemory(&Attr->sBrightnessConfig, &Panel->BrightnessConfig, sizeof(Attr->sBrightnessConfig));

    if (calibration->AlRequest.Enable != 0)
    {
        Attr->sALEnhancement.sADConfig.bEnabled = calibration->AlRequest.Enable;
        Attr->sALEnhancement.sADConfig.uMaxIterations = calibration->AlRequest.MaxIterations;
    }

    return status;
}

// Wrapper that also validates the arguments
NTSTATUS
StartDisplayInitAction(_In_ PPANEL_CONTEXT Panel, _Out_ QDI_Panel_AttrType* Attr)
{
    NTSTATUS status;

    if (Panel == NULL || Attr == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "StartDisplayInitAction: invalid parameter");
    }
    else
    {
        status = FillPanelConfiguration(Attr, &Panel->Info, Panel);
    }

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "FillPanelConfiguration failed %!STATUS!", status);
    }
    return status;
}
