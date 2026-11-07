// Operations of panel 0x3406 (installed into SDP_PANEL_OPS by
// Init_PanelActionTable).

#include "SdpDriver.h"
#include "PanelOps.tmh"

// Persistent DSI lists of the panel: init, version specific init, terminate and
// brightness. They are built once and re-sent on every power transition.
#define SEQ_INIT        0
#define SEQ_VERSION     1
#define SEQ_TERM        2
#define SEQ_BRIGHTNESS  3

// Builds the persistent lists
NTSTATUS
Init_PreAllocated_META2Ev3_Sequence(_In_ PPANEL_CONTEXT Panel)
{
    NTSTATUS status;

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    PREDEFINED_DISPLAY_SEQ_INIT(&Panel->Sequences[SEQ_INIT]);
    FillMeta2Ev3PanelInitSequence(
        &Panel->Sequences[SEQ_INIT], Panel->DisplayId, Panel->Caps, Panel->PanelBin, Panel->PanelBinSize);

    PREDEFINED_DISPLAY_SEQ_INIT(&Panel->Sequences[SEQ_VERSION]);
    NTSTATUS versionStatus =
        FillMeta2Ev3RevSpecificPanelInitSequence(&Panel->Sequences[SEQ_VERSION], Panel->DisplayId, Panel->PanelVersion);

    PREDEFINED_DISPLAY_SEQ_INIT(&Panel->Sequences[SEQ_TERM]);
    NTSTATUS termStatus = FillMetaPanelTermSeqeuence(&Panel->Sequences[SEQ_TERM]);

    PREDEFINED_DISPLAY_SEQ_INIT(&Panel->Sequences[SEQ_BRIGHTNESS]);
    status = FillMetaDisplaySetBrightnessSequence(&Panel->Sequences[SEQ_BRIGHTNESS]);

        status = versionStatus | termStatus | status;

    if (!NT_SUCCESS(status))
    {
        Free_PreAllocated_DisplaySequence(Panel);
        Panel->SequencesValid = FALSE;
    }
    else
    {
        Panel->SequencesValid = TRUE;
    }
    return status;
}
VOID
Free_PreAllocated_META_Sequence(_In_ PPANEL_CONTEXT Panel)
{
    if (Panel != NULL)
    {
        Free_PreAllocated_DisplaySequence(Panel);
    }
}

// Reads the manufacturer id and the production date of the panel into the
// 7 word result
NTSTATUS
Get_Meta2_Mfg_Info(_In_ PPANEL_CONTEXT Panel, _Out_ ULONG* MfgInfo)
{
    SDP_DSI_NODE head;
    NTSTATUS status;
    PUCHAR buffer = NULL;

    RtlZeroMemory(&head, sizeof(head));

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, 8, SDP_POOL_TAG);
    if (buffer == NULL)
    {
        MfgInfo[0] = 0xFFFF;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    NTSTATUS buildStatus = FillMetaPanelGetId(&head);
    if (!NT_SUCCESS(buildStatus))
    {
        Free_DisplaySequence(head.Next);
    }

    status = ProcessSequence(Panel, &head, buffer, 6);
    if (!NT_SUCCESS(status))
    {
        MfgInfo[0] = 0xFFFF;
    }
    else
    {
        MfgInfo[0] = (buffer[2] << 8) | buffer[3];
        MfgInfo[1] = (buffer[0] << 8) | buffer[1];
        MfgInfo[2] = buffer[4];
    }
    RtlCopyMemory(Panel->MfgInfo, MfgInfo, sizeof(Panel->MfgInfo));

    RtlZeroMemory(&head, sizeof(head));
    status = FillMetaGetGuid(&head);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
    }
    else
    {
        status = ProcessSequence(Panel, &head, (PUCHAR)&MfgInfo[3], 0x10);
        if (!NT_SUCCESS(status))
        {
            RtlCopyMemory(Panel->MfgInfo, MfgInfo, sizeof(Panel->MfgInfo));
        }
    }

    ExFreePoolWithTag(buffer, SDP_POOL_TAG);
    return status;
}
NTSTATUS
Send_Meta2_Init_Sequence(_In_ PPANEL_CONTEXT Panel)
{
    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->Ops.SendInitSequence == NULL || !Panel->SequencesValid)
    {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = SendSequence(Panel, &Panel->Sequences[SEQ_INIT]);
    status |= SendSequence(Panel, &Panel->Sequences[SEQ_VERSION]);
    return status;
}
NTSTATUS
Send_Meta_Terminate_Sequence(_In_ PPANEL_CONTEXT Panel)
{
    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->Ops.SendTermSequence == NULL || !Panel->SequencesValid)
    {
        return STATUS_INVALID_PARAMETER;
    }
    return SendSequence(Panel, &Panel->Sequences[SEQ_TERM]);
}

// Whether the panel can do this brightness (milli-nits)
BOOLEAN
Validate_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _In_ ULONG MilliNits)
{
    UNREFERENCED_PARAMETER(Panel);

    return (MilliNits < SDP_MIN_PANEL_NITS_MILLI || MilliNits > SDP_MAX_PANEL_NITS_MILLI) ? FALSE : TRUE;
}

// Converts milli-nits into the DBV level by linear interpolation in the
// brightness table
NTSTATUS
GetDBV_Meta_BrightnessNits(_In_ PPANEL_CONTEXT Panel, _In_ ULONG MilliNits, _Out_ ULONG* Dbv)
{
    UNREFERENCED_PARAMETER(Panel);

    ULONG dbv = 0;

    SDP_NITS_LUT_ENTRY* NitsToDbvLut = Panel->RuntimeCalibration.Valid ? Panel->RuntimeCalibration.RuntimeLut : g_SW3416_NitsToDbvLut;

    for (ULONG i = 1; i < SDP_NITS_LUT_COUNT; i++)
    {
        if (MilliNits <= NitsToDbvLut[i].MilliNits)
        {
            ULONG nitsSpan = NitsToDbvLut[i].MilliNits - NitsToDbvLut[i - 1].MilliNits;

                        if (nitsSpan != 0)
            {
                dbv = ((NitsToDbvLut[i].Dbv - NitsToDbvLut[i - 1].Dbv) *
                       (MilliNits - NitsToDbvLut[i - 1].MilliNits)) /
                      nitsSpan;
            }
            dbv += NitsToDbvLut[i - 1].Dbv;
            break;
        }
    }

    *Dbv = dbv;
    return STATUS_SUCCESS;
}

// Sends the brightness command with Dbv as level Level 0 also
// turns the display off, the first non zero level turns it back on.
NTSTATUS
Set_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Dbv)
{
    NTSTATUS status;

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->Ops.SetDbv == NULL || !Panel->SequencesValid)
    {
        return STATUS_INVALID_PARAMETER;
    }

    //Panel->Sequences[SEQ_BRIGHTNESS].Next->Data[1] = (UCHAR)Dbv;
    ULONG Dbv10 = (Dbv * 1023) / 255;
    Panel->Sequences[SEQ_BRIGHTNESS].Next->Data[1] = (UCHAR)((Dbv10 >> 8) & 0x03);
    Panel->Sequences[SEQ_BRIGHTNESS].Next->Data[2] = (UCHAR)(Dbv10 & 0xFF);
    status = SendSequence(Panel, &Panel->Sequences[SEQ_BRIGHTNESS]);
    if (NT_SUCCESS(status))
    {
        if (Dbv == 0)
        {
            Panel->DisplayOff = TRUE;
            SendDisplayOnOffAction(Panel, FALSE);
        }
        else if (Panel->DisplayOff == TRUE)
        {
            Panel->DisplayOff = FALSE;
            SendDisplayOnOffAction(Panel, TRUE);
        }
    }
    return status;
}

// DCS get_display_brightness
NTSTATUS
Get_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _Out_ UCHAR* Value)
{
    SDP_DSI_NODE head;
    ULONG raw = 0;
    NTSTATUS status;

    if (Panel == NULL || Value == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(&head, sizeof(head));
    status = FillMetaDisplayGetBrightnessSequence(&head);
    if (NT_SUCCESS(status))
    {
        if (Panel->Ops.GetBrightness == NULL)
        {
            status = STATUS_INVALID_PARAMETER;
        }
        else
        {
            status = ProcessSequence(Panel, &head, (PUCHAR)&raw, sizeof(raw));
            UCHAR* response = (UCHAR*)&raw;
            ULONG Dbv10 = (((ULONG)response[0]) << 8) | ((ULONG)response[1]);
            *Value = (UCHAR)((Dbv10 * 255) / 1023);
        }
    }
    return status;
}

// Writes the 3 bit panel register
NTSTATUS
Set_Meta2_SetPixelRegistration(_In_ PPANEL_CONTEXT Panel, _In_ UCHAR* Value)
{
    SDP_DSI_NODE head;

    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(&head, sizeof(head));
    NTSTATUS status = FillMeta2PixelRegistrationValue(&head, *Value);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
    }
    return SendSequence(Panel, &head);
}

// DCS get_power_mode; 0x9C is the normal operating mode
NTSTATUS
Get_Meta2_PowerMode(_In_ PPANEL_CONTEXT Panel, _Out_ CHAR* Mode, _Out_opt_ UCHAR* IsNormal)
{
    SDP_DSI_NODE head;
    ULONG raw = 0;
    NTSTATUS status;

    if (Panel == NULL || Mode == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(&head, sizeof(head));
    NTSTATUS buildStatus = FillMetaPowerModeSequence(&head);
    if (!NT_SUCCESS(buildStatus))
    {
        Free_DisplaySequence(head.Next);
    }

    status = ProcessSequence(Panel, &head, (PUCHAR)&raw, sizeof(raw));
    if (NT_SUCCESS(status))
    {
        *Mode = (CHAR)raw;
    }

    if (IsNormal != NULL)
    {
        *IsNormal = ((UCHAR)raw == 0x9C) ? 1 : 0;
    }
    return status;
}

// DCS get_diagnostic_result
NTSTATUS
Get_Meta2_SelfDiagnostic_Info(_In_ PPANEL_CONTEXT Panel, _Out_ UCHAR* Value)
{
    SDP_DSI_NODE head;
    ULONG raw = 0;
    NTSTATUS status;

    if (Panel == NULL || Value == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(&head, sizeof(head));
    NTSTATUS buildStatus = FillMetaSelfDiagnosticResultSequence(&head);
    if (!NT_SUCCESS(buildStatus))
    {
        Free_DisplaySequence(head.Next);
    }

    status = ProcessSequence(Panel, &head, (PUCHAR)&raw, sizeof(raw));
    if (NT_SUCCESS(status))
    {
        *Value = (UCHAR)raw;
    }
    return status;
}

// Switches the colour compensation of the panel when the temperature enters a
// different range of the table. Hysteresis keeps it from flapping
NTSTATUS
Set_Meta2_Throttle(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Temperature, _Inout_ SDP_TEMP_RESULT* Result)
{
    SDP_DSI_NODE head;
    LONG temperature = (LONG)Temperature;

    RtlZeroMemory(&head, sizeof(head));

    ULONG panelIndex = ConvertQDIDisplayIDtoDisplayIndex(Panel->DisplayId);
    SDP_THERMAL_STATE* state = &g_ThermalState[panelIndex];
    ULONG lastIndex = state->LastIndex;

    if (state->LastTemperature < temperature && g_ThermalTable[lastIndex].MinTemp + g_ThermalHysteresis < temperature)
    {
        state->LastTemperature = temperature;
    }
    else
    {
        if (state->LastTemperature < temperature)
        {
            // Rising, but still within the hysteresis band: keep the current range.
            Result->MaxNits = g_ThermalTable[lastIndex].MaxNits;
            Result->Percent = g_ThermalTable[lastIndex].Percent;
            return STATUS_SUCCESS;
        }
        state->LastTemperature = temperature;
    }

    ULONG match = 0;
    while (match <= SDP_THERMAL_ENTRY_COUNT - 1)
    {
        if (g_ThermalTable[match].MinTemp <= temperature && temperature <= g_ThermalTable[match].MaxTemp)
        {
            break;
        }
        match++;
    }
    if (match > SDP_THERMAL_ENTRY_COUNT - 1)
    {
        return STATUS_SUCCESS;
    }

    Result->MaxNits = g_ThermalTable[match].MaxNits;
    Result->Percent = g_ThermalTable[match].Percent;

    if (state->LastIndex == match)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_PANEL, "Temperature %d stays in range %u", temperature, state->LastIndex);
        return STATUS_SUCCESS;
    }

    state->LastIndex = match;
    Panel->ThermalIndex = match;

    NTSTATUS status = SdpSeqBuildTemperatureCompensation(&head, g_ThermalTable[match].ColorComp);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Building the temperature compensation failed");
        return status;
    }

    status = SendSequence(Panel, &head);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Sending the temperature compensation failed");
    }
    return status;
}

// Runs the vsync / TE comparison of the two panels Result
// flags: 0x80000001 transfer failure; 3/5 and 9/0x11 the primary/secondary
// panel runs slow (vsync period difference above 1 ms).
static NTSTATUS
Perform_Meta2_Te_and_ExtTrig_Test(_In_ PPANEL_CONTEXT Panel, _In_ PPANEL_CONTEXT Other, _Out_ PULONG Result)
{
    SDP_DSI_NODE head;
    ULONG flags = 0;
    NTSTATUS status;
    BOOLEAN primary = (Panel->DisplayId == QDI_DISPLAY_PRIMARY);
    ULONG period = Panel->VsyncPeriodUs;

    RtlZeroMemory(&head, sizeof(head));

    status = FillMetaChangeTETimingSequence(&head, FALSE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
        goto Done;
    }

    status = SendSequence(Panel, &head);
    if (!NT_SUCCESS(status))
    {
        flags = 0x80000001;
        goto Done;
    }

    DelayMilliseconds(1000);
    if ((LONG)(Panel->VsyncPeriodUs - period) < 1000)
    {
        flags = primary ? 3 : 5;
    }

    RtlZeroMemory(&head, sizeof(head));
    status = FillMetaEnableQSyncExtTrigSequence(&head, FALSE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
        flags |= 0x80000001;
        goto Done;
    }

    status = SendSequence(Panel, &head);
    if (!NT_SUCCESS(status))
    {
        flags = 0x80000001;
        goto Done;
    }

    status = FillMetaEnableQSyncExtTrigSequence(&head, FALSE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
        flags |= 0x80000001;
        goto Done;
    }

    status = SendSequence(Other, &head);
    if (!NT_SUCCESS(status))
    {
        flags = 0x80000001;
        goto Done;
    }

    DelayMilliseconds(1000);
    if (Other->VsyncPeriodUs < Panel->VsyncPeriodUs)
    {
        if ((ULONG)(Panel->VsyncPeriodUs - Other->VsyncPeriodUs) > 1000)
        {
            flags |= primary ? 9 : 0x11;
        }
    }
    else if ((ULONG)(Other->VsyncPeriodUs - Panel->VsyncPeriodUs) > 1000)
    {
        flags |= primary ? 3 : 5;
    }

    RtlZeroMemory(&head, sizeof(head));
    status = FillMetaEnableQSyncExtTrigSequence(&head, TRUE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
        flags |= 0x80000001;
        goto Done;
    }

    status = SendSequence(Other, &head);
    if (!NT_SUCCESS(status))
    {
        flags = 0x80000001;
        goto Done;
    }

    RtlZeroMemory(&head, sizeof(head));
    status = FillMetaChangeTETimingSequence(&head, TRUE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
        flags |= 0x80000001;
        goto Done;
    }

    status = SendSequence(Panel, &head);
    if (!NT_SUCCESS(status))
    {
        flags = 0x80000001;
    }

Done:
    *Result = flags;
    return status;
}
NTSTATUS
Get_Meta2_MiscDiagnostic_Info(_In_ PPANEL_CONTEXT Panel, _In_ PPANEL_CONTEXT Other, _Out_ ULONG* Result)
{
    ULONG flags = 0;
    NTSTATUS status;

    if (Panel == NULL || Result == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
    }
    else
    {
        status = Perform_Meta2_Te_and_ExtTrig_Test(Panel, Other, &flags);
    }

    *Result = flags;
    return status;
}

// Normal mode: enable the capability bits 1..4
VOID
SetMeta2ManufacturingMode(_In_ PPANEL_CONTEXT Panel)
{
    static const ULONG bits[4] = {1, 2, 3, 4};

    for (ULONG i = 0; i < ARRAYSIZE(bits); i++)
    {
        if (bits[i] < 5)
        {
            Panel->Caps |= 1ull << (bits[i] & 0x1F);
        }
        else
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Invalid capability bit %u", bits[i]);
        }
    }
}

// Maps a temperature to a brightness limit: <= 10 C 60 %, <= 20 C 80 %,
// above 100 % Changed is set when the step changed.
VOID
Set_PassiveCooling_Meta2(_In_ PDEVICE_CONTEXT Device, _In_ ULONG Temperature, _Out_ UCHAR* Changed)
{
    ULONG percent;
    ULONG step;

    if (Temperature < 0x15)
    {
        if (g_ThrottleThresholdHigh < Temperature || Temperature <= g_ThrottleThresholdLow)
        {
            percent = g_ThrottlePercentLow;
            step = g_ThrottleThresholdLow;
        }
        else
        {
            percent = g_ThrottlePercentMid;
            step = g_ThrottleThresholdHigh;
        }
    }
    else
    {
        percent = 100;
        step = 100;
    }

    if (step == g_ThrottleStep)
    {
        *Changed = 0;
    }
    else
    {
        *Changed = 1;
        g_ThrottleStep = step;
        Device->ThermalPercent = percent;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_PANEL, "Thermal throttle %u percent (changed %u)", percent, *Changed);
}
