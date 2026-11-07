#include "SdpDriver.h"
#include "sfpd.h"
#include "Calibration.tmh"

const ULONG g_Lut1[256] =
{
      4000,   4420,   4985,   5549,   6114,   6678,   7243,   7807,
      8372,   8936,   9501,  10065,  10489,  11053,  11618,  12261,
     12983,  13705,  14427,  15148,  15870,  16592,  17314,  17855,
     18577,  19299,  20021,  20743,  21465,  22392,  23417,  24463,
     25529,  26615,  27722,  28848,  29994,  31158,  32342,  33545,
     34766,  36006,  37264,  38541,  39835,  41147,  42476,  43823,
     45188,  46569,  47968,  49383,  50815,  52264,  53729,  55211,
     56709,  58223,  59753,  61299,  62861,  64438,  66031,  67640,
     69263,  70903,  72557,  74227,  75911,  77611,  79325,  81054,
     82798,  84556,  86329,  88116,  89918,  91734,  93564,  95408,
     97267,  99139, 101025, 102925, 104839, 106767, 108708, 110663,
    112631, 114613, 116608, 118617, 120639, 122674, 124722, 126783,
    128858, 130945, 133046, 135159, 137285, 139424, 141576, 143740,
    145917, 148107, 150309, 152524, 154751, 156990, 159242, 161507,
    163783, 166072, 168373, 170686, 173011, 175348, 177697, 180059,
    182432, 184817, 187214, 189622, 192043, 194475, 196919, 199375,
    201842, 204321, 206811, 209313, 211826, 214351, 216887, 219435,
    221994, 224564, 227146, 229739, 232343, 234958, 237584, 240222,
    242870, 245530, 248200, 250882, 253575, 256278, 258993, 261718,
    264454, 267201, 269959, 272728, 275507, 278297, 281098, 283909,
    286731, 289564, 292407, 295261, 298125, 301000, 303885, 306781,
    309687, 312603, 315530, 318467, 321415, 324373, 327341, 330319,
    333308, 336307, 339316, 342335, 345364, 348404, 351453, 354513,
    357582, 360662, 363752, 366851, 369961, 373080, 376210, 379349,
    382498, 385657, 388826, 392005, 395194, 398392, 401600, 404818,
    408045, 411282, 414529, 417786, 421052, 424328, 427613, 430908,
    434213, 437527, 440851, 444184, 447526, 450878, 454240, 457611,
    460991, 464381, 467780, 471189, 474607, 478034, 481471, 484917,
    488372, 491836, 495310, 498793, 502285, 505786, 509297, 512816,
    516345, 519883, 523430, 526987, 530552, 534126, 537710, 541302,
    544904, 548514, 552134, 555762, 559400, 563046, 566702, 570366,
    574039, 577721, 581412, 585112, 588821, 592538, 596265, 600000
};

static
BOOLEAN
ValidateLut2(
    _In_reads_(256) const SDP_NITS_LUT_ENTRY* Lut2
    )
{
    ULONG i;

    for (i = 0; i < 256; i++)
    {
        //
        // Android brightness range.
        //
        if (Lut2[i].Dbv > 255)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "LUT2[%lu] Dbv=%lu > 255",
                i,
                Lut2[i].Dbv);

            return FALSE;
        }

        //
        // Nits should never be zero.
        //
        if (Lut2[i].MilliNits == 0)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "LUT2[%lu] MilliNits=0",
                i);

            return FALSE;
        }

        if (i == 0)
        {
            continue;
        }

        //
        // LUT2 is sorted by milli-nits.
        //
        if (Lut2[i].MilliNits <=
            Lut2[i - 1].MilliNits)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "LUT2 monotonic fail index=%lu prev=%lu cur=%lu",
                i,
                Lut2[i - 1].MilliNits,
                Lut2[i].MilliNits);

            return FALSE;
        }

        //
        // Brightness should never decrease.
        //
        if (Lut2[i].Dbv <
            Lut2[i - 1].Dbv)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "LUT2 brightness fail index=%lu prev=%lu cur=%lu",
                i,
                Lut2[i - 1].Dbv,
                Lut2[i].Dbv);

            return FALSE;
        }
    }

    return TRUE;
}

static
BOOLEAN
ValidateRuntimeLut(
    _In_reads_(SDP_NITS_LUT_COUNT)
    const SDP_NITS_LUT_ENTRY* Lut
    )
{
    ULONG i;

    for (i = 1; i < SDP_NITS_LUT_COUNT; i++)
    {
        if (Lut[i].MilliNits <=
            Lut[i - 1].MilliNits)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "RuntimeLut nits fail index=%lu prev=%lu cur=%lu",
                i,
                Lut[i - 1].MilliNits,
                Lut[i].MilliNits);

            return FALSE;
        }

        if (Lut[i].Dbv <
            Lut[i - 1].Dbv)
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "RuntimeLut dbv fail index=%lu prev=%lu cur=%lu",
                i,
                Lut[i - 1].Dbv,
                Lut[i].Dbv);

            return FALSE;
        }
    }

    return TRUE;
}

static
BOOLEAN
ParseUlong(
    _In_z_ PCSTR String,
    _Out_ PULONG Value
    )
{
    ULONG Result = 0;

    while (*String)
    {
        CHAR Ch = *String++;

        if (Ch < '0' || Ch > '9')
        {
            return FALSE;
        }

        Result *= 10;
        Result += (Ch - '0');
    }

    *Value = Result;

    return TRUE;
}

static
BOOLEAN
ParseLut2Line(
    _In_z_ PCHAR Line,
    _Out_ SDP_NITS_LUT_ENTRY* Entry
    )
{
    PCHAR Comma;
    ULONG MilliNits;
    ULONG Brightness;

    Comma = strchr(Line, ',');

    if (!Comma)
    {
        return FALSE;
    }

    *Comma = 0;

    if (!ParseUlong(Line, &MilliNits))
    {
        return FALSE;
    }

    if (!ParseUlong(Comma + 1, &Brightness))
    {
        return FALSE;
    }

    Entry->MilliNits = MilliNits;
    Entry->Dbv = Brightness;

    return TRUE;
}


static
ULONG
ResolveBrightness(
    _In_reads_(256) const ULONG* Lut1,
    _In_reads_(256) const SDP_NITS_LUT_ENTRY* Lut2,
    _In_ ULONG OsBrightness
    )
{
    LONG Low;
    LONG High;
    LONG Last;
    ULONG TargetNits;

    TargetNits = Lut1[OsBrightness];

    Low = 0;
    High = 255;
    Last = -1;

    while (High > Low)
    {
        LONG Mid;

        Mid = Low + ((High - Low) / 2);

        if (Mid == Last)
        {
            break;
        }

        if (Lut2[Mid].MilliNits < TargetNits)
        {
            Low = Mid + 1;
        }
        else
        {
            High = Low + ((High - Low) / 2);
        }

        Last = Mid;
    }

    return Lut2[Low].Dbv;
}

NTSTATUS
LoadCustomerLut2(
    _In_ WDFDEVICE Device,
    _In_ BOOLEAN Panel1,
    _Out_writes_(256) SDP_NITS_LUT_ENTRY* Lut2
    )
{
    NTSTATUS Status;
    DWORD Size;
    PCHAR Buffer = NULL;

    PCWSTR Path =
        Panel1
        ? CUSTOMER_OS_LUT2_1_FILE_PATH
        : CUSTOMER_OS_LUT2_0_FILE_PATH;

    Status = GetSFPDItemSize(
        Device,
        (WCHAR*)Path,
        &Size);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Buffer = ExAllocatePoolWithTag(
        NonPagedPool,
        Size + 1,
        '2TUL');

    if (!Buffer)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    Status = GetSFPDItem(
        Device,
        (WCHAR*)Path,
        Buffer,
        Size);

    if (!NT_SUCCESS(Status))
    {
        ExFreePool(Buffer);
        return Status;
    }

    Buffer[Size] = 0;

    ULONG Index = 0;
    PCHAR Context = NULL;
    PCHAR Line = strtok_s(Buffer, "\n", &Context);

    while (Line)
    {
        while (*Line == ' ' || *Line == '\r')
        {
            Line++;
        }

        if (strncmp(Line, "DisplayIndex", 12) &&
            strncmp(Line, "Millinits", 9))
        {
            if (Index >= 256)
            {
                break;
            }

        PCHAR End = Line + strlen(Line);

        while (End > Line)
        {
            CHAR Ch = End[-1];

            if (Ch != '\r' &&
                Ch != '\n' &&
                Ch != ' '  &&
                Ch != '\t')
            {
                break;
            }

            End--;
        }

        *End = '\0';

        if (ParseLut2Line(Line, &Lut2[Index]))
        {
            Index++;
        }
        else
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_CALIBRATION,
                "ParseLut2Line FAILED: '%s'",
                Line);
        }
        }

        Line = strtok_s(NULL, "\n", &Context);
    }

    ExFreePool(Buffer);

    TraceEvents(
        TRACE_LEVEL_ERROR,
        TRACE_CALIBRATION,
        "LoadCustomerLut2 parsed %lu entries (expected 256)",
        Index);

    return (Index == 256)
        ? STATUS_SUCCESS
        : STATUS_DATA_ERROR;
}

VOID
GenerateRuntimeLut(
    _In_reads_(256) const ULONG* Lut1,
    _In_reads_(256) const SDP_NITS_LUT_ENTRY* Lut2,
    _Out_ PANEL_RUNTIME_CALIBRATION* Runtime
    )
{
    ULONG Out = 0;

    for (ULONG Brightness = 1;
         Brightness < 256;
         Brightness++)
    {
        Runtime->RuntimeLut[Out].Dbv =
            ResolveBrightness(
                Lut1,
                Lut2,
                Brightness);

        Runtime->RuntimeLut[Out].MilliNits =
            Lut1[Brightness];

        Out++;
    }

    Runtime->Valid = TRUE;
}

NTSTATUS
InitializePanelRuntimeLut(
    _In_ PPANEL_CONTEXT Panel,
    BOOLEAN LeftPanel
    )
{
    NTSTATUS Status;
    SDP_NITS_LUT_ENTRY Lut2[256];

    Status = LoadCustomerLut2(
        Panel->Device,
        LeftPanel,
        Lut2);

    if (!NT_SUCCESS(Status) ||
        !ValidateLut2(Lut2))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_CALIBRATION,
            "LUT2 invalid");

        return STATUS_DATA_ERROR;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_CALIBRATION,
        "ValidateLut2 passed");

    GenerateRuntimeLut(
        g_Lut1,
        Lut2,
        &Panel->RuntimeCalibration);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_CALIBRATION,
        "GenerateRuntimeLut finished");

    if (!ValidateRuntimeLut(Panel->RuntimeCalibration.RuntimeLut))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_CALIBRATION,
            "Generated LUT invalid");

        return STATUS_DATA_ERROR;
    }

    return STATUS_SUCCESS;
}