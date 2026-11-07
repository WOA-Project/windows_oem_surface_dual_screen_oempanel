// Contents of the DSI sequences of panel 0x3406 (init, terminate, register
// reads, ...). The byte values come straight from the binary.

#include "SdpDriver.h"
#include "PanelSequences.tmh"

// Constant payloads kept in the writable data section of the binary.
static UCHAR g_DsiUnlockBankB0[3] = {0xB0, 0x60, 0x43};     // 14001c0d0
static UCHAR g_DsiCmdF5[2] = {0xF5, 0x1B};                  // 14001c0d4
static UCHAR g_DsiCmdDf[3] = {0xDF, 0x0A, 0x00};            // 14001c0dc
static UCHAR g_DsiCmd53[2] = {0x53, 0x00};                  // 14001c0e8
static UCHAR g_DsiCmd55[2] = {0x55, 0x0C};                  // 14001c0ec
static UCHAR g_DsiUnlockB0[3] = {0xB0, 0xA5, 0x00};         // 14001c0f4
static UCHAR g_DsiCmdB1[2] = {0xB1, 0x69};                  // 14001c0f8

static UCHAR g_SW3416_DsiLockMCS[2] = {0xB0, 0xCA};                // lock MCS
static UCHAR g_SW3416_DsiUnlockMCS[2] = {0xB0, 0xAC};              // Unlock MCS
static UCHAR g_SW3416_DsiCmd2A[5] = {0x2A, 0x00, 0x00, 0x05, 0x45}; // Window Size
static UCHAR g_SW3416_DsiCmd2B[5] = {0x2B, 0x00, 0x00, 0x07, 0x07}; // Window Size
static UCHAR g_SW3416_DsiGammaSet[2] = {0x26, 0x02};               // Gama set
static UCHAR g_SW3416_DsiCmd53A[3] = {0x53, 0x0C, 0x30};           // Write Control Display 1
static UCHAR g_SW3416_DsiCmd53B[3] = {0x53, 0x0D, 0x30};
static UCHAR g_SW3416_DsiCmd55[7] = {0x55, 0x04, 0x70, 0xDB, 0x00, 0x78, 0xDB}; // Write Control Display 2
static UCHAR g_SW3416_DsiCmdEE[2] = {0xEE, 0x24};                  // Pixel Reg Enable
static UCHAR g_SW3416_DsiCmdFB[2] = {0xFB, 0xAC};                  // OTP WP
static UCHAR g_SW3416_DsiCmdE9[12] = {0xE9, 0x32, 0x32, 0x55, 0x06, 0x00, 0x1C, 0x00, 0x00, 0x55, 0x50, 0x4B}; // AGPC config
static UCHAR g_SW3416_DsiCmdD0_DV[22] = {0xD0, 0x80, 0x10, 0x81, 0x02, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0C, 0x00, 0xD4, 0x0B, 0x00, 0x11, 0x00, 0x1C}; // Gamma brightness for AGPC
static UCHAR g_SW3416_DsiCmdD0_EV2[22] = {0xD0, 0x80, 0x10, 0x01, 0x02, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0C, 0x00, 0xD4, 0x0A, 0x00, 0x0F, 0x00, 0x1C};
static UCHAR g_SW3416_DsiCmd57[2] = {0x57, 0x10};                  // to be removed when we have calibrated panels

// DSC picture parameter set (DSI data type 0x0A), 128 bytes.
static const UCHAR g_DscPps[0x80] = {
    0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x07, 0x08, 0x05, 0x46, 0x00, 0x12, 0x02, 0xA3, 0x02, 0xA3,
    0x02, 0x00, 0x02, 0x52, 0x00, 0x20, 0x01, 0xE7, 0x00, 0x09, 0x00, 0x0C, 0x05, 0xA6, 0x04, 0x85,
    0x18, 0x00, 0x10, 0xF0, 0x03, 0x0C, 0x20, 0x00, 0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38,
    0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40,
    0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8, 0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6,
    0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const UCHAR g_SW3416_DscPps[0x80] = {
    0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x07, 0x08, 0x05, 0x46, 0x03, 0x84, 0x02, 0xA3, 0x02, 0xA3,
    0x02, 0x00, 0x02, 0x51, 0x00, 0x20, 0x5F, 0x4B, 0x00, 0x09, 0x00, 0x0C, 0x00, 0x1C, 0x00, 0x18,
    0x18, 0x00, 0x10, 0xF0, 0x03, 0x0C, 0x20, 0x00, 0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38,
    0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B, 0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40,
    0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8, 0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6,
    0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Wraps the common "write a short/long packet with the keep flag" pattern.
#define ADD(Head, PacketType, Data, Persistent) \
    AppendCommand((Head), SDP_DSI_NODE_WRITE, (PacketType), 2, (PUCHAR)(Data), sizeof(Data), (Persistent))

// Maximum return packet size writes around a DCS read (set, read, restore to 1).
static NTSTATUS
SdpSeqBuildRead(_Inout_ SDP_DSI_NODE* Head, _In_ UCHAR MaxReturnSize, _In_ UCHAR ReadCommand, _In_ ULONG ReadFlags)
{
    UCHAR setSize[2] = {0, 0};
    UCHAR restoreSize[2] = {1, 0};
    UCHAR command[2] = {0, 0};

    setSize[0] = MaxReturnSize;
    command[0] = ReadCommand;

    NTSTATUS status = AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, setSize, 2, FALSE);
    status |= AppendCommand(
        Head, SDP_DSI_NODE_READ, SDP_DSI_DT_DCS_READ_0, ReadFlags, command, ReadFlags == 1 ? 1 : 2, FALSE);
    status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, restoreSize, 2, FALSE);
    return status;
}

// Panel init: soft reset, sleep out, DSC PPS, panel specific trim data from
// the DPP bin file, tearing effect and brightness defaults
NTSTATUS
FillMeta2Ev3PanelInitSequence(
    _Inout_ SDP_DSI_NODE* Head,
    _In_ ULONG DisplayId,
    _In_ ULONGLONG Caps,
    _In_opt_ PUCHAR PanelBin,
    _In_ ULONG PanelBinSize)
{
    UNREFERENCED_PARAMETER(DisplayId);

    static const UCHAR softReset[2] = {0x01, 0x00};
    static const UCHAR brightnessZero[3] = {0x51, 0x00, 0x00};
    //static const UCHAR cmd5C[3] = {0x5C, 0x1F, 0x00};
    //static const UCHAR cmd5E[2] = {0x5E, 0x10};

    UCHAR trimData[SDP_PANEL_BIN_ID_BYTES] = {0};
    BOOLEAN haveTrimData = FALSE;

    // The bin holds 9 bytes of trim data at +0x13 and an optional CRC32 at +0x1c.
    if (PanelBinSize == SDP_PANEL_BIN_SIZE && PanelBin != NULL)
    {
        if (*(ULONG*)(PanelBin + 0x1C) == 0)
        {
            haveTrimData = TRUE;
            RtlCopyMemory(trimData, PanelBin + 0x13, sizeof(trimData));
        }
        else
        {
            ULONG crc = CalcCrc32(PanelBin + 0x13, sizeof(trimData));
            if (crc == *(ULONG*)(PanelBin + 0x1C))
            {
                haveTrimData = TRUE;
                RtlCopyMemory(trimData, PanelBin + 0x13, sizeof(trimData));
            }
            else
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Panel bin CRC mismatch");
            }
        }
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Panel bin missing or wrong size (%u)", PanelBinSize);
    }

    NTSTATUS status = AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, (PUCHAR)softReset, 2, TRUE);
    status |= AppendDelay(Head, 1, TRUE);
    status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, (PUCHAR)brightnessZero, 3, TRUE);
    status |= AppendSleepCommand(Head, 0x11, TRUE);      // sleep out
    status |= AppendDelay(Head, 0x10, TRUE);
    status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, 0x0A, 2, (PUCHAR)g_SW3416_DscPps, sizeof(g_SW3416_DscPps), TRUE);
    status |= AppendDelay(Head, 0x10, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiUnlockMCS, TRUE);

    if (haveTrimData)
    {
        status |= AppendDelay(Head, 5, TRUE);
        //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, trimData, sizeof(trimData), TRUE);
    }

    status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd2A, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd2B, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiGammaSet, TRUE);

    status |= AppendTESetScanLineCommand(Head, 1000, TRUE);
    status |= AppendTECommand(Head, 0x35, TRUE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, (PUCHAR)cmd5C, sizeof(cmd5C), TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd53A, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd55, TRUE);

    status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiCmdEE, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiCmdFB, TRUE);
    status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiLockMCS, TRUE);

    if ((Caps & SDP_CAPS_SKIP_EXTRA_INIT_CMD) == 0)
    {
        //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_1, 2, (PUCHAR)cmd5E, sizeof(cmd5E), TRUE);
    }
    return status;
}

// Second init list, depends on the panel version
NTSTATUS
FillMeta2Ev3RevSpecificPanelInitSequence(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DisplayId, _In_ ULONG PanelVersion)
{
    UNREFERENCED_PARAMETER(DisplayId);

    ULONG minor = PanelVersion & 0xFF;
    ULONG major = (PanelVersion & 0xFF00) >> 8;

    //NTSTATUS status = ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_DsiCmdB1, TRUE);
    NTSTATUS status = AppendSleepCommand(Head, 0x11, TRUE);      // sleep out

    if (major == 6 || major == 7)
    {
        if (minor < 8 || minor > 10)
        {
            //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockBankB0, TRUE);
            //status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_DsiCmdF5, TRUE);
            //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiCmdDf, TRUE);
        }
        // EV1.5
        status |= AppendDelay(Head, 130, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiCmd57, TRUE);
        status |= AppendDisplayCommand(Head, 0x29, TRUE);
        status |= AppendDelay(Head, 130, TRUE);
    }
    else if (major == 8 || major == 9)
    {
        // EV2
        status |= AppendDelay(Head, 90, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiCmd57, TRUE);
        status |= AppendDisplayCommand(Head, 0x29, TRUE);
        status |= AppendDelay(Head, 50, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiUnlockMCS, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmdE9, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmdD0_EV2, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiLockMCS, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd53B, TRUE);
    }
    else if (major == 10)
    {
        // DV
        status |= AppendDelay(Head, 90, TRUE);
        status |= AppendDisplayCommand(Head, 0x29, TRUE);
        status |= AppendDelay(Head, 50, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiUnlockMCS, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmdE9, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmdD0_DV, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_SW3416_DsiLockMCS, TRUE);
        status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_SW3416_DsiCmd53B, TRUE);
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Unsupported panel version %u.%u", major, minor);
    }
    return status;
}

// Terminate: brightness 0, display off, sleep in
NTSTATUS
FillMetaPanelTermSeqeuence(_Inout_ SDP_DSI_NODE* Head)
{
    static const UCHAR brightnessZero[3] = {0x51, 0x00, 0x00};

    NTSTATUS status = AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, (PUCHAR)brightnessZero, 3, TRUE);
    status |= AppendDisplayCommand(Head, 0x28, TRUE);
    status |= AppendSleepCommand(Head, 0x10, TRUE);      // sleep in
    return status;
}

// Single node list with the brightness command; the level byte (Data[1])
// is patched before every send
NTSTATUS
FillMetaDisplaySetBrightnessSequence(_Inout_ SDP_DSI_NODE* Head)
{
    UCHAR brightness[3] = {0x51, 0x01, 0x47};

    NTSTATUS status = AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, brightness, 3, TRUE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(Head->Next);
    }
    return status;
}

// Writes a 3 bit panel register
NTSTATUS
FillMeta2PixelRegistrationValue(_Inout_ SDP_DSI_NODE* Head, _In_ UCHAR Value)
{
    UNREFERENCED_PARAMETER(Value);

    //UCHAR cmd57[2] = {0x57, 0};
    //UCHAR cmd4F[1] = {0x4F};
    //UCHAR cmd4E[1] = {0x4E};

    //cmd57[1] = (UCHAR)(Value & 7);

    NTSTATUS status = STATUS_SUCCESS;
    //NTSTATUS status = ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockB0, FALSE);
    status |= AppendDelay(Head, 1, TRUE);
    //status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, g_DsiCmdB1, FALSE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_1, 2, cmd57, sizeof(cmd57), FALSE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, cmd4F, sizeof(cmd4F), FALSE);
    status |= AppendDelay(Head, 0xD0, TRUE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, cmd4E, sizeof(cmd4E), FALSE);
    return status;
}

// Dual panel test, step A
NTSTATUS
FillMetaChangeTETimingSequence(_Inout_ SDP_DSI_NODE* Head, _In_ BOOLEAN Second)
{
    //UCHAR firstVariant[7] = {0xE4, 0x30, 0x05, 0x00, 0x48, 0x34, 0x02};
    //UCHAR secondVariant[7] = {0xE4, 0x30, 0x04, 0x00, 0x48, 0x34, 0x02};

    if (Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = STATUS_SUCCESS;
    //NTSTATUS status = ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockB0, FALSE);
    if (!Second)
    {
        //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, firstVariant, FALSE);
    }
    else
    {
        //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, secondVariant, FALSE);
    }
    return status;
}

// Dual panel test, step B
NTSTATUS
FillMetaEnableQSyncExtTrigSequence(_Inout_ SDP_DSI_NODE* Head, _In_ BOOLEAN Second)
{
    //UCHAR cmd5A05[2] = {0x5A, 0x05};
    //UCHAR cmd5A00[2] = {0x5A, 0x00};
    //UCHAR cmd5CC2[3] = {0x5C, 0xC2, 0x01};
    //UCHAR scanline1000[3] = {0x44, 0x03, 0xE8};
    //UCHAR scanline0[3] = {0x44, 0x00, 0x00};
    NTSTATUS status;

    if (Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    status = STATUS_SUCCESS;

    if (!Second)
    {
        //status = ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, scanline0, FALSE);
        //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, cmd5CC2, FALSE);
        //status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, cmd5A05, FALSE);
    }
    else
    {
        //status = ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, scanline1000, FALSE);
        //status |= ADD(Head, SDP_DSI_DT_DCS_SHORT_WRITE_1, cmd5A00, FALSE);
    }
    return status;
}

// DCS get_display_brightness (0x52)
NTSTATUS
FillMetaDisplayGetBrightnessSequence(_Inout_ SDP_DSI_NODE* Head)
{
    NTSTATUS status;

    if (Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    status = SdpSeqBuildRead(Head, 4, 0x52, 2);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(Head->Next);
    }
    return status;
}

// Reads 0x12 bytes of register 0xA9
NTSTATUS
FillMetaGetGuid(_Inout_ SDP_DSI_NODE* Head)
{
    if (Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }
    return SdpSeqBuildRead(Head, 0x12, 0xA9, 2);
}

// Reads the manufacturer id bytes: register 0xA1 and five reads of 0xA8
NTSTATUS
FillMetaPanelGetId(_Inout_ SDP_DSI_NODE* Head)
{
    //UCHAR setSize[2] = {8, 0};
    //UCHAR restoreSize[2] = {1, 0};
    UCHAR readA1[2] = {0xA1, 0x00};
    UCHAR readA8[2] = {0xA8, 0x00};
    //UCHAR selectPage[4] = {0xB2, 0x5D, 0x41, 0x02};

    if (Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = STATUS_SUCCESS;
    //NTSTATUS status = AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, setSize, 2, FALSE);
    //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockB0, FALSE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, selectPage, sizeof(selectPage), FALSE);
    status |= AppendCommand(Head, SDP_DSI_NODE_READ, SDP_DSI_DT_DCS_READ_0, 2, readA1, 2, FALSE);
    for (ULONG i = 0; i < 5; i++)
    {
        status |= AppendCommand(Head, SDP_DSI_NODE_READ, SDP_DSI_DT_DCS_READ_0, 2, readA8, 2, FALSE);
    }
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, restoreSize, 2, FALSE);
    return status;
}

// DCS get_power_mode (0x0A)
NTSTATUS
FillMetaPowerModeSequence(_Inout_ SDP_DSI_NODE* Head)
{
    return SdpSeqBuildRead(Head, 4, 0x0A, 2);
}

// DCS get_diagnostic_result (0x0F)
NTSTATUS
FillMetaSelfDiagnosticResultSequence(_Inout_ SDP_DSI_NODE* Head)
{
    return SdpSeqBuildRead(Head, 4, 0x0F, 2);
}

// Colour compensation for a temperature range: 10 bytes of E9 register
// data (part of the table of PanelBrightnessTables.c)
NTSTATUS
SdpSeqBuildTemperatureCompensation(_Inout_ SDP_DSI_NODE* Head, _In_ const UCHAR* ColorCompensation)
{
    UCHAR data[0x12];

    RtlZeroMemory(data, sizeof(data));
    //data[0] = 0xE9;
    RtlCopyMemory(&data[3], ColorCompensation, 10);
    //RtlFillMemory(&data[13], 5, 0x32);

    NTSTATUS status = STATUS_SUCCESS;
    //NTSTATUS status = ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockBankB0, FALSE);
    //status |= ADD(Head, SDP_DSI_DT_DCS_LONG_WRITE, g_DsiUnlockB0, FALSE);
    status |= AppendDelay(Head, 0x78, TRUE);
    //status |= AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, data, sizeof(data), FALSE);
    return status;
}
