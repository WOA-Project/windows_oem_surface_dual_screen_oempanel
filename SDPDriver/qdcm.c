#include "SdpDriver.h"
#include "sfpd.h"
#include "qdcm.h"
#include "qdcm.tmh"

int _fltused;

static
ULONG
QdcmFindValidEntries(
    const UINT32* Channel
)
{
    ULONG BadRun = 0;

    for (ULONG i = 0;
         i < FEATURE7_XML_ENTRIES;
         i++)
    {
        if (i != 0)
        {
            if (Channel[i] < Channel[i - 1])
            {
                TraceEvents(
                    TRACE_LEVEL_INFORMATION,
                    TRACE_DRIVER,
                    "Monotonic break at %lu: %lu -> %lu",
                    i,
                    Channel[i - 1],
                    Channel[i]);
                return i;
            }
        }

        if (Channel[i] <= FEATURE7_MAX_ENTRY_VALUE)
        {
            BadRun = 0;
            continue;
        }

        BadRun++;

        if (BadRun >= 2)
        {
            return i - 1;
        }
    }

    return FEATURE7_XML_ENTRIES;
}

static
__forceinline
UINT32
QdcmClampLutValue(
    LONG Value
)
{
    if (Value < 0)
    {
        return 0;
    }

    if (Value > FEATURE7_MAX_ENTRY_VALUE)
    {
        return FEATURE7_MAX_ENTRY_VALUE;
    }

    return (UINT32)Value;
}

static
ULONG
QdcmMonotonicityScore(
    const UINT32* Channel,
    ULONG ValidEntries
)
{
    ULONG Breaks = 0;

    for (ULONG i = 1;
         i < ValidEntries;
         i++)
    {
        if (Channel[i] < Channel[i - 1])
        {
            Breaks++;
        }
    }

    if (Breaks == 0)
    {
        return 100;
    }

    ULONG PenaltyScore =
        ((Breaks * 100) /
        max(1, ValidEntries));

    if (PenaltyScore > 100)
    {
        PenaltyScore = 100;
    }

    return 100 - PenaltyScore;
}

static
ULONG
QdcmSmoothnessScore(
    const UINT32* Channel,
    ULONG ValidEntries
)
{
    ULONG Penalty = 0;

    for (ULONG i = 2;
         i < ValidEntries;
         i++)
    {
        LONG SlopeA =
            (LONG)Channel[i - 1] -
            (LONG)Channel[i - 2];

        LONG SlopeB =
            (LONG)Channel[i] -
            (LONG)Channel[i - 1];

        LONG Delta =
            abs(SlopeB - SlopeA);

        if (Delta > 64)
        {
            Penalty++;
        }
    }

    ULONG PenaltyScore =
        ((Penalty * 100) /
        max(1, ValidEntries));

    if (PenaltyScore > 100)
    {
        PenaltyScore = 100;
    }

    return 100 - PenaltyScore;
}

static
QDCM_CHANNEL_HEALTH
QdcmAnalyzeChannel(
    const UINT32* Channel
)
{
    QDCM_CHANNEL_HEALTH Result;

    RtlZeroMemory(
        &Result,
        sizeof(Result));

    Result.ValidEntries =
        QdcmFindValidEntries(
            Channel);

    Result.CompletenessScore =
        (Result.ValidEntries * 100)
        / FEATURE7_XML_ENTRIES;

    Result.MonotonicityScore =
        QdcmMonotonicityScore(
            Channel,
            Result.ValidEntries);

    Result.SmoothnessScore =
        QdcmSmoothnessScore(
            Channel,
            Result.ValidEntries);

    Result.TotalScore =
        (
            Result.CompletenessScore * FEATURE7_WEIGHT_COMPLETENESS +
            Result.MonotonicityScore * FEATURE7_WEIGHT_MONOTONICITY +
            Result.SmoothnessScore   * FEATURE7_WEIGHT_SMOOTHNESS
        ) / 100;

    return Result;
}

static
UINT32
QdcmWeightedBlend(
    UINT32 A,
    ULONG WeightA,

    UINT32 B,
    ULONG WeightB
)
{
    ULONGLONG Numerator;
    ULONGLONG Denominator;

    Denominator =
        (ULONGLONG)WeightA +
        (ULONGLONG)WeightB;

    if (Denominator == 0)
    {
        return A;
    }

    Numerator =
        ((ULONGLONG)A * WeightA) +
        ((ULONGLONG)B * WeightB);

    return
        (UINT32)(
            Numerator /
            Denominator);
}

static
LONG
QdcmSafeAddSlope(
    LONG Current,
    LONG Slope
)
{
    LONGLONG Value =
        (LONGLONG)Current +
        (LONGLONG)Slope;

    if (Value < 0)
    {
        Value = 0;
    }

    if (Value > FEATURE7_MAX_ENTRY_VALUE)
    {
        Value = FEATURE7_MAX_ENTRY_VALUE;
    }

    return (LONG)Value;
}

static
VOID
QdcmRecoverChannel(
    _In_reads_(FEATURE7_XML_ENTRIES)
    const UINT32* RefA,

    _In_reads_(FEATURE7_XML_ENTRIES)
    const UINT32* RefB,

    _In_ const QDCM_CHANNEL_HEALTH* RefAHealth,
    _In_ const QDCM_CHANNEL_HEALTH* RefBHealth,

    _Inout_updates_(FEATURE7_XML_ENTRIES)
    UINT32* Target,

    _In_ const QDCM_CHANNEL_HEALTH* TargetHealth
)
{
    LONG SumSlope = 0;
    LONG AvgSlope;
    LONG Current;

    ULONG ValidEntries =
        TargetHealth->ValidEntries;

    if (ValidEntries >= FEATURE7_XML_ENTRIES)
    {
        return;
    }

    if (ValidEntries < FEATURE7_SLOPE_WINDOW + 1)
    {
        return;
    }

    //
    // Calculate average slope from the last valid window.
    //
    for (ULONG i =
            ValidEntries - FEATURE7_SLOPE_WINDOW;
         i < ValidEntries - 1;
         i++)
    {
        SumSlope +=
            (LONG)Target[i + 1] -
            (LONG)Target[i];
    }

    AvgSlope =
        SumSlope /
        (FEATURE7_SLOPE_WINDOW - 1);

    if (AvgSlope <= 0)
    {
        AvgSlope = 1;
    }

    Current =
        (LONG)Target[
            ValidEntries - 1];

    for (ULONG i = ValidEntries;
         i < FEATURE7_XML_ENTRIES;
         i++)
    {
        ULONG RefWeightA;
        ULONG RefWeightB;
        ULONG TotalRefWeight;

        ULONG ReferenceWeight;

        UINT32 RefValue;
        UINT32 Recovered;

        //
        // Self-extrapolated prediction.
        //
        Current =
            QdcmSafeAddSlope(
                Current,
                AvgSlope);

        //
        // Only use references that are at least recoverable.
        //
        RefWeightA =
            (RefAHealth->TotalScore >=
             FEATURE7_SCORE_RECOVERABLE)
                ? RefAHealth->TotalScore
                : 0;

        RefWeightB =
            (RefBHealth->TotalScore >=
             FEATURE7_SCORE_RECOVERABLE)
                ? RefBHealth->TotalScore
                : 0;

        TotalRefWeight =
            RefWeightA +
            RefWeightB;

        //
        // Trust references more when the target
        // channel has low confidence.
        //
        if (TargetHealth->TotalScore >= 100)
        {
            ReferenceWeight = 0;
        }
        else
        {
            ReferenceWeight =
                min(
                    50,
                    100 - TargetHealth->TotalScore);
        }

        //
        // No trustworthy references available.
        // Fall back to pure extrapolation.
        //
        if (TotalRefWeight == 0)
        {
            RefValue =
                QdcmClampLutValue(
                    Current);

            ReferenceWeight = 0;
        }
        else
        {
            //
            // Health-weighted reference value.
            //
            RefValue =
                (UINT32)
                (
                    (
                        ((ULONGLONG)RefA[i] *
                         RefWeightA)

                        +

                        ((ULONGLONG)RefB[i] *
                         RefWeightB)
                    )

                    /

                    TotalRefWeight
                );
        }

        //
        // Blend extrapolated value with
        // reference prediction.
        //
        Recovered =
            QdcmWeightedBlend(
                QdcmClampLutValue(Current),

                TargetHealth->TotalScore,

                RefValue,

                ReferenceWeight);

        //
        // Hard monotonicity guarantee.
        //
        if (Recovered < Target[i - 1])
        {
            Recovered =
                Target[i - 1];
        }

        //
        // Hard upper bound.
        //
        if (Recovered >
            FEATURE7_MAX_ENTRY_VALUE)
        {
            Recovered =
                FEATURE7_MAX_ENTRY_VALUE;
        }

        Target[i] =
            Recovered;

        Current =
            (LONG)Recovered;
    }
}

static
BOOLEAN
SdpConvertFeature8(
    const QDCM_FEATURE8* Src,
    SDP_CAL_LINEAR_GAMMA_PAYLOAD* Dst
)
{
    ULONG i;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "ConvertFeature8 entered");

    if (Src->Header0 != 1 ||
        Src->EntryCount != 0x400 ||
        Src->Header2 != 7)
    {
        return FALSE;
    }

    RtlZeroMemory(Dst, sizeof(*Dst));

    Dst->Reserved0       = Src->Header0;
    Dst->Enable          = TRUE;
    Dst->NumCoefficients = 1024;
    Dst->Reserved3       = 0;

    for (i = 0; i < 1024; i++)
    {
        Dst->Red[i]   = (USHORT)Src->Red[i];
        Dst->Green[i] = (USHORT)Src->Green[i];
        Dst->Blue[i]  = (USHORT)Src->Blue[i];
    }

    return TRUE;
}

static
VOID
BuildLinearGammaDescriptor(
    SDP_CALIBRATION_BUFFER* Cal
)
{
    QDI_Panel_LinearGammaCorrectionTableType* Gamma =
        &Cal->LinearGamma;

    RtlZeroMemory(Gamma, sizeof(*Gamma));

    Gamma->eDisplayId = QDI_DISPLAY_PRIMARY;
    Gamma->bEnable = TRUE;

    Gamma->uNumCoefficients = 1024;
    Gamma->uCoefficientSizeInBytes = sizeof(USHORT);

    Gamma->pRedCr =
        &Cal->LinearGammaPayload.Red[0];

    Gamma->pGreenY =
        &Cal->LinearGammaPayload.Green[0];

    Gamma->pBlueCb =
        &Cal->LinearGammaPayload.Blue[0];
}

static
BOOLEAN
QdcmValidateChannel(
    const UINT32* Channel
)
{
    for (ULONG i = 1;
         i < FEATURE7_XML_ENTRIES;
         i++)
    {
        if (Channel[i] < Channel[i - 1])
        {
            return FALSE;
        }

        if (Channel[i] > FEATURE7_MAX_ENTRY_VALUE)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static
BOOLEAN
SdpConvertFeature7(
    QDCM_FEATURE7* Src,
    QDI_Panel_InverseGammaCorrectionTableType* Dst
)
{
    BOOLEAN Success = FALSE;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "ConvertFeature7 entered");

    if (Src->Header0 != 0x10 ||
        Src->EntryCount != 0x100 ||
        Src->Header2 != 7)
    {
        return FALSE;
    }

    UINT32* Red =
        ExAllocatePoolWithTag(
            NonPagedPoolNx,
            sizeof(UINT32) * FEATURE7_XML_ENTRIES,
            'rCDF');

    UINT32* Green =
        ExAllocatePoolWithTag(
            NonPagedPoolNx,
            sizeof(UINT32) * FEATURE7_XML_ENTRIES,
            'gCDF');

    UINT32* Blue =
        ExAllocatePoolWithTag(
            NonPagedPoolNx,
            sizeof(UINT32) * FEATURE7_XML_ENTRIES,
            'bCDF');

    if (Red == NULL ||
        Green == NULL ||
        Blue == NULL)
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DRIVER,
            "Feature7 allocation failed");

        goto Cleanup;
    }

    RtlCopyMemory(
        Red,
        Src->Red,
        sizeof(Src->Red));

    RtlCopyMemory(
        Green,
        Src->Green,
        sizeof(Src->Green));

    RtlCopyMemory(
        Blue,
        Src->Blue,
        sizeof(Src->Blue));

    QDCM_CHANNEL_HEALTH RedHealth =
        QdcmAnalyzeChannel(Red);

    QDCM_CHANNEL_HEALTH GreenHealth =
        QdcmAnalyzeChannel(Green);

    QDCM_CHANNEL_HEALTH BlueHealth =
        QdcmAnalyzeChannel(Blue);

    ULONG OriginalRedValid =
        RedHealth.ValidEntries;

    ULONG OriginalGreenValid =
        GreenHealth.ValidEntries;

    ULONG OriginalBlueValid =
        BlueHealth.ValidEntries;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "R valid=%lu comp=%lu mono=%lu smooth=%lu total=%lu",
        RedHealth.ValidEntries,
        RedHealth.CompletenessScore,
        RedHealth.MonotonicityScore,
        RedHealth.SmoothnessScore,
        RedHealth.TotalScore);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "G valid=%lu comp=%lu mono=%lu smooth=%lu total=%lu",
        GreenHealth.ValidEntries,
        GreenHealth.CompletenessScore,
        GreenHealth.MonotonicityScore,
        GreenHealth.SmoothnessScore,
        GreenHealth.TotalScore);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "B valid=%lu comp=%lu mono=%lu smooth=%lu total=%lu",
        BlueHealth.ValidEntries,
        BlueHealth.CompletenessScore,
        BlueHealth.MonotonicityScore,
        BlueHealth.SmoothnessScore,
        BlueHealth.TotalScore);

    BOOLEAN RedRecoverable =
        RedHealth.TotalScore >=
        FEATURE7_SCORE_RECOVERABLE;

    BOOLEAN GreenRecoverable =
        GreenHealth.TotalScore >=
        FEATURE7_SCORE_RECOVERABLE;

    BOOLEAN BlueRecoverable =
        BlueHealth.TotalScore >=
        FEATURE7_SCORE_RECOVERABLE;

    BOOLEAN CanRecoverRed =
        GreenRecoverable ||
        BlueRecoverable;

    BOOLEAN CanRecoverGreen =
        RedRecoverable ||
        BlueRecoverable;

    BOOLEAN CanRecoverBlue =
        RedRecoverable ||
        GreenRecoverable;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Feature7 recovery required: R=%u G=%u B=%u",
        RedHealth.ValidEntries != FEATURE7_XML_ENTRIES,
        GreenHealth.ValidEntries != FEATURE7_XML_ENTRIES,
        BlueHealth.ValidEntries != FEATURE7_XML_ENTRIES);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Feature7 missing entries: R=%lu G=%lu B=%lu",
        FEATURE7_XML_ENTRIES - OriginalRedValid,
        FEATURE7_XML_ENTRIES - OriginalGreenValid,
        FEATURE7_XML_ENTRIES - OriginalBlueValid);

    if (RedHealth.ValidEntries < FEATURE7_XML_ENTRIES && CanRecoverRed)
    {
        QdcmRecoverChannel(
            Green,
            Blue,
            &GreenHealth,
            &BlueHealth,
            Red,
            &RedHealth);

        RedHealth = QdcmAnalyzeChannel(Red);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "R recovered valid=%lu total=%lu",
            RedHealth.ValidEntries,
            RedHealth.TotalScore);

        if (RedHealth.ValidEntries !=
            FEATURE7_XML_ENTRIES)
        {
            TraceEvents(
                TRACE_LEVEL_WARNING,
                TRACE_DRIVER,
                "Red recovery incomplete (%lu entries)",
                RedHealth.ValidEntries);
        }
    }

    if (GreenHealth.ValidEntries < FEATURE7_XML_ENTRIES && CanRecoverGreen)
    {
        QdcmRecoverChannel(
            Red,
            Blue,
            &RedHealth,
            &BlueHealth,
            Green,
            &GreenHealth);

        GreenHealth = QdcmAnalyzeChannel(Green);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "G recovered valid=%lu total=%lu",
            GreenHealth.ValidEntries,
            GreenHealth.TotalScore);

        if (GreenHealth.ValidEntries !=
            FEATURE7_XML_ENTRIES)
        {
            TraceEvents(
                TRACE_LEVEL_WARNING,
                TRACE_DRIVER,
                "Green recovery incomplete (%lu entries)",
                GreenHealth.ValidEntries);
        }
    }

    if (BlueHealth.ValidEntries < FEATURE7_XML_ENTRIES && CanRecoverBlue)
    {
        QdcmRecoverChannel(
            Red,
            Green,
            &RedHealth,
            &GreenHealth,
            Blue,
            &BlueHealth);
        
        BlueHealth = QdcmAnalyzeChannel(Blue);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "B recovered valid=%lu total=%lu",
            BlueHealth.ValidEntries,
            BlueHealth.TotalScore);

        if (BlueHealth.ValidEntries !=
            FEATURE7_XML_ENTRIES)
        {
            TraceEvents(
                TRACE_LEVEL_WARNING,
                TRACE_DRIVER,
                "Blue recovery incomplete (%lu entries)",
                BlueHealth.ValidEntries);
        }
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Recovered score R=%lu G=%lu B=%lu",
        RedHealth.TotalScore,
        GreenHealth.TotalScore,
        BlueHealth.TotalScore);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Recovered valid R=%lu G=%lu B=%lu",
        RedHealth.ValidEntries,
        GreenHealth.ValidEntries,
        BlueHealth.ValidEntries);

    NT_ASSERT(QdcmValidateChannel(Red));
    NT_ASSERT(QdcmValidateChannel(Green));
    NT_ASSERT(QdcmValidateChannel(Blue));

    NT_ASSERT(
        Red[FEATURE7_XML_ENTRIES - 1] <=
        FEATURE7_MAX_ENTRY_VALUE);

    NT_ASSERT(
        Green[FEATURE7_XML_ENTRIES - 1] <=
        FEATURE7_MAX_ENTRY_VALUE);

    NT_ASSERT(
        Blue[FEATURE7_XML_ENTRIES - 1] <=
        FEATURE7_MAX_ENTRY_VALUE);

    RtlZeroMemory(Dst, sizeof(*Dst));

    Dst->eDisplayId = QDI_DISPLAY_PRIMARY;
    Dst->bEnabled = TRUE;

    RtlCopyMemory(
        Src->Red,
        Red,
        sizeof(Src->Red));

    RtlCopyMemory(
        Src->Green,
        Green,
        sizeof(Src->Green));

    RtlCopyMemory(
        Src->Blue,
        Blue,
        sizeof(Src->Blue));

    RtlCopyMemory(
        Dst->uRedCr,
        Red,
        sizeof(Dst->uRedCr));

    RtlCopyMemory(
        Dst->uGreenY,
        Green,
        sizeof(Dst->uGreenY));

    RtlCopyMemory(
        Dst->uBlueCb,
        Blue,
        sizeof(Dst->uBlueCb));

    Success = TRUE;

Cleanup:

    if (Red)
    {
        ExFreePoolWithTag(
            Red,
            'rCDF');
    }

    if (Green)
    {
        ExFreePoolWithTag(
            Green,
            'gCDF');
    }

    if (Blue)
    {
        ExFreePoolWithTag(
            Blue,
            'bCDF');
    }

    return Success;
}

static
BOOLEAN
SdpConvertFeature2(
    _In_reads_bytes_(Feature2Size) CONST VOID* Feature2Data,
    _In_ SIZE_T Feature2Size,
    _Out_ QDI_Panel_ColorCorrectionMatrixType* Dst
)
{
    if (Feature2Data == NULL ||
        Dst == NULL)
    {
        return FALSE;
    }

    if (Feature2Size != sizeof(QDI_Panel_ColorCorrectionMatrixType))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DRIVER,
            "Feature2 size mismatch. XML=%Iu Expected=%Iu",
            Feature2Size,
            sizeof(QDI_Panel_ColorCorrectionMatrixType));

        return FALSE;
    }

    RtlZeroMemory(
        Dst,
        sizeof(*Dst));

    RtlCopyMemory(
        Dst,
        Feature2Data,
        sizeof(*Dst));

    return TRUE;
}

BOOLEAN
HexCharToNibble(
    CHAR Ch,
    UCHAR* Value
)
{
    if (Ch >= '0' && Ch <= '9')
    {
        *Value = Ch - '0';
        return TRUE;
    }

    if (Ch >= 'A' && Ch <= 'F')
    {
        *Value = Ch - 'A' + 10;
        return TRUE;
    }

    if (Ch >= 'a' && Ch <= 'f')
    {
        *Value = Ch - 'a' + 10;
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
HexStringToBytes(
    PCSTR Hex,
    SIZE_T HexLength,
    PUCHAR Buffer,
    SIZE_T BufferSize
)
{
    SIZE_T i;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "HexStringToBytes Hex=%Iu Out=%Iu",
        HexLength / 2,
        BufferSize);

    if ((HexLength & 1) != 0)
        return FALSE;

    if ((HexLength / 2) > BufferSize)
        return FALSE;

    for (i = 0; i < HexLength; i += 2)
    {
        UCHAR Hi;
        UCHAR Lo;

        if (!HexCharToNibble(Hex[i], &Hi))
            return FALSE;

        if (!HexCharToNibble(Hex[i + 1], &Lo))
            return FALSE;

        Buffer[i / 2] = (Hi << 4) | Lo;
    }

    return TRUE;
}

BOOLEAN
LoadFeature2(
    PCSTR Hex,
    SIZE_T HexLength,
    QDCM_PROFILE* Profile
)
{
    if ((HexLength / 2) != 0x110)
    {
        return FALSE;
    }

    if (!HexStringToBytes(
            Hex,
            HexLength,
            Profile->Feature2,
            sizeof(Profile->Feature2)))
    {
        return FALSE;
    }

    ULONG* Header = (ULONG*)Profile->Feature2;

    if (Header[0] != 0xFF)
    {
        TraceEvents(
            TRACE_LEVEL_WARNING,
            TRACE_DRIVER,
            "Unexpected Feature2 header0=%lu",
            Header[0]);
    }

    Profile->HasFeature2 = TRUE;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Feature2 header0=%lu header1=%lu",
        ((ULONG *)Profile->Feature2)[0],
        ((ULONG *)Profile->Feature2)[1]);

    return TRUE;
}

static
VOID
BuildGamutDescriptor(
    SDP_CALIBRATION_BUFFER* Cal
)
{
    QDI_Panel_GamutMappingTableType* Gamut =
        &Cal->Gamut;

    RtlZeroMemory(
        Gamut,
        sizeof(*Gamut));

    Gamut->eDisplayId =
        QDI_DISPLAY_PRIMARY;

    Gamut->bEnabled = TRUE;

    Gamut->uNumSamplesPerColor = 9;

    Gamut->pRedCr =
        Cal->GamutPayload.Red;

    Gamut->pGreenY =
        Cal->GamutPayload.Green;

    Gamut->pBlueCb =
        Cal->GamutPayload.Blue;
}

static
FLOAT
SampleGamma1024(
    const USHORT* Lut,
    FLOAT X
)
{
    ULONG Index;
    ULONG Next;
    FLOAT Fraction;

    if (X <= 0.0f)
    {
        return
            (FLOAT)Lut[0] /
            4095.0f;
    }

    if (X >= 1.0f)
    {
        return
            (FLOAT)Lut[1023] /
            4095.0f;
    }

    X *= 1023.0f;

    Index =
        (ULONG)X;

    Next =
        min(
            Index + 1,
            1023);

    Fraction =
        X -
        (FLOAT)Index;

    return
        (
            ((FLOAT)Lut[Index] *
             (1.0f - Fraction))
            +
            ((FLOAT)Lut[Next] *
             Fraction)
        )
        /
        4095.0f;
}

static
FLOAT
SampleLut256(
    const UINT32* Lut,
    FLOAT X
)
{
    ULONG Index;
    ULONG Next;
    FLOAT Fraction;

    if (X <= 0.0f)
    {
        return
            (FLOAT)Lut[0] /
            4095.0f;
    }

    if (X >= 1.0f)
    {
        return
            (FLOAT)Lut[255] /
            4095.0f;
    }

    X *= 255.0f;

    Index = (ULONG)X;

    Next =
        min(
            Index + 1,
            255);

    Fraction =
        X -
        (FLOAT)Index;

    return
        (
            ((FLOAT)Lut[Index] *
             (1.0f - Fraction))
            +
            ((FLOAT)Lut[Next] *
             Fraction)
        )
        /
        4095.0f;
}

static
VOID
ApplyPcc(
    const QDI_Panel_ColorCorrectionMatrixType* Pcc,
    FLOAT* R,
    FLOAT* G,
    FLOAT* B
)
{
    FLOAT Ri = *R;
    FLOAT Gi = *G;
    FLOAT Bi = *B;

    FLOAT Ro;
    FLOAT Go;
    FLOAT Bo;

    DOUBLE Rc[11];
    DOUBLE Gc[11];
    DOUBLE Bc[11];

    RtlCopyMemory(
        Rc,
        Pcc->sCCMatrix[0],
        sizeof(Rc));

    RtlCopyMemory(
        Gc,
        Pcc->sCCMatrix[1],
        sizeof(Gc));

    RtlCopyMemory(
        Bc,
        Pcc->sCCMatrix[2],
        sizeof(Bc));

    Ro =
        (FLOAT)Rc[0]  +
        (FLOAT)Rc[1]  * Ri +
        (FLOAT)Rc[2]  * Gi +
        (FLOAT)Rc[3]  * Bi +
        (FLOAT)Rc[4]  * Ri * Gi +
        (FLOAT)Rc[5]  * Gi * Bi +
        (FLOAT)Rc[6]  * Bi * Ri +
        (FLOAT)Rc[7]  * Ri * Ri +
        (FLOAT)Rc[8]  * Gi * Gi +
        (FLOAT)Rc[9]  * Bi * Bi +
        (FLOAT)Rc[10] * Ri * Gi * Bi;

    Go =
        (FLOAT)Gc[0]  +
        (FLOAT)Gc[1]  * Ri +
        (FLOAT)Gc[2]  * Gi +
        (FLOAT)Gc[3]  * Bi +
        (FLOAT)Gc[4]  * Ri * Gi +
        (FLOAT)Gc[5]  * Gi * Bi +
        (FLOAT)Gc[6]  * Bi * Ri +
        (FLOAT)Gc[7]  * Ri * Ri +
        (FLOAT)Gc[8]  * Gi * Gi +
        (FLOAT)Gc[9]  * Bi * Bi +
        (FLOAT)Gc[10] * Ri * Gi * Bi;

    Bo =
        (FLOAT)Bc[0]  +
        (FLOAT)Bc[1]  * Ri +
        (FLOAT)Bc[2]  * Gi +
        (FLOAT)Bc[3]  * Bi +
        (FLOAT)Bc[4]  * Ri * Gi +
        (FLOAT)Bc[5]  * Gi * Bi +
        (FLOAT)Bc[6]  * Bi * Ri +
        (FLOAT)Bc[7]  * Ri * Ri +
        (FLOAT)Bc[8]  * Gi * Gi +
        (FLOAT)Bc[9]  * Bi * Bi +
        (FLOAT)Bc[10] * Ri * Gi * Bi;

    *R = max(0.0f, min(1.0f, Ro));
    *G = max(0.0f, min(1.0f, Go));
    *B = max(0.0f, min(1.0f, Bo));
}

static
VOID
BuildSyntheticGamutPayload(
    SDP_CALIBRATION_BUFFER* Cal,

    _In_reads_(FEATURE7_XML_ENTRIES)
    const UINT32* Red,

    _In_reads_(FEATURE7_XML_ENTRIES)
    const UINT32* Green,

    _In_reads_(FEATURE7_XML_ENTRIES)
    const UINT32* Blue
)
{
    ULONG Index = 0;

    SDP_CAL_GAMUT_PAYLOAD* Payload =
        &Cal->GamutPayload;

    Payload->Id = 0;
    Payload->Enabled = TRUE;
    Payload->GamutBeforePcc = TRUE;

    for (ULONG R = 0;
         R < 9;
         R++)
    {
        for (ULONG G = 0;
             G < 9;
             G++)
        {
            for (ULONG B = 0;
                 B < 9;
                 B++)
            {
                FLOAT Rn =
                    (FLOAT)R / 8.0f;

                FLOAT Gn =
                    (FLOAT)G / 8.0f;

                FLOAT Bn =
                    (FLOAT)B / 8.0f;

                //
                // Feature7 (IGC)
                //
                Rn =
                    SampleLut256(
                        Red,
                        Rn);

                Gn =
                    SampleLut256(
                        Green,
                        Gn);

                Bn =
                    SampleLut256(
                        Blue,
                        Bn);

                //
                // Feature2 (CCM)
                //
                ApplyPcc(
                    &Cal->Ccm,
                    &Rn,
                    &Gn,
                    &Bn);

                //
                // Feature8 (Gamma)
                //
                Rn =
                    SampleGamma1024(
                        Cal->LinearGammaPayload.Red,
                        Rn);

                Gn =
                    SampleGamma1024(
                        Cal->LinearGammaPayload.Green,
                        Gn);

                Bn =
                    SampleGamma1024(
                        Cal->LinearGammaPayload.Blue,
                        Bn);

                Payload->Red[Index] =
                    (USHORT)(
                        Rn * 65535.0f);

                Payload->Green[Index] =
                    (USHORT)(
                        Gn * 65535.0f);

                Payload->Blue[Index] =
                    (USHORT)(
                        Bn * 65535.0f);

                Index++;
            }
        }
    }

    BuildGamutDescriptor(
        Cal);
}

BOOLEAN
BuildFromProfile(
    SDP_CALIBRATION_BUFFER* Cal,
    QDCM_PROFILE* Profile
)
{
    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "BuildFromProfile entered");

    if (!Profile->HasFeature2 ||
        !Profile->HasFeature7 ||
        !Profile->HasFeature8)
    {
        return FALSE;
    }

    RtlZeroMemory(Cal, sizeof(*Cal));

    if (!SdpConvertFeature2(
        Profile->Feature2,
        sizeof(Profile->Feature2),
        &Cal->Ccm))
    {
        return FALSE;
    }

    for (ULONG Row = 0;
        Row < 3;
        Row++)
    {
        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "PCC[%lu] %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
            Row,
            Cal->Ccm.sCCMatrix[Row][0],
            Cal->Ccm.sCCMatrix[Row][1],
            Cal->Ccm.sCCMatrix[Row][2],
            Cal->Ccm.sCCMatrix[Row][3],
            Cal->Ccm.sCCMatrix[Row][4],
            Cal->Ccm.sCCMatrix[Row][5],
            Cal->Ccm.sCCMatrix[Row][6],
            Cal->Ccm.sCCMatrix[Row][7],
            Cal->Ccm.sCCMatrix[Row][8],
            Cal->Ccm.sCCMatrix[Row][9],
            Cal->Ccm.sCCMatrix[Row][10]);
    }

    DOUBLE Rc[11];
    DOUBLE Gc[11];
    DOUBLE Bc[11];

    RtlCopyMemory(
        Rc,
        Cal->Ccm.sCCMatrix[0],
        sizeof(Rc));

    RtlCopyMemory(
        Gc,
        Cal->Ccm.sCCMatrix[1],
        sizeof(Gc));

    RtlCopyMemory(
        Bc,
        Cal->Ccm.sCCMatrix[2],
        sizeof(Bc));

    static BOOLEAN DumpedPcc = FALSE;

    if (!DumpedPcc)
    {
        DumpedPcc = TRUE;

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Rc=%f %f %f %f",
            Rc[0],
            Rc[1],
            Rc[2],
            Rc[3]);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Gc=%f %f %f %f",
            Gc[0],
            Gc[1],
            Gc[2],
            Gc[3]);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Bc=%f %f %f %f",
            Bc[0],
            Bc[1],
            Bc[2],
            Bc[3]);
    }

    if (!SdpConvertFeature7(
            &Profile->Feature7,
            &Cal->InverseGamma))
    {
        return FALSE;
    }

    if (!SdpConvertFeature8(
            &Profile->Feature8,
            &Cal->LinearGammaPayload))
    {
        return FALSE;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "IGC H0=%lu EC=%lu H2=%lu",
        Profile->Feature7.Header0,
        Profile->Feature7.EntryCount,
        Profile->Feature7.Header2);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "GC H0=%lu EC=%lu H2=%lu",
        Profile->Feature8.Header0,
        Profile->Feature8.EntryCount,
        Profile->Feature8.Header2);

    BuildLinearGammaDescriptor(Cal);
    BuildSyntheticGamutPayload(
        Cal,
        Profile->Feature7.Red,
        Profile->Feature7.Green,
        Profile->Feature7.Blue);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "BuildFromProfile success");

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "CCM=%u IGC=%u GC=%u Gamut=%u",
        !RtlEqualMemory(&Cal->Ccm, &(QDI_Panel_ColorCorrectionMatrixType){0},
                        sizeof(Cal->Ccm)),
        Cal->InverseGamma.bEnabled,
        Cal->LinearGamma.bEnable,
        Cal->Gamut.bEnabled);

    return TRUE;
}

static
PCHAR
FindString(
    _In_ PCHAR Buffer,
    _In_ SIZE_T Length,
    _In_z_ PCSTR Pattern
)
{
    SIZE_T PatternLength;

    PatternLength = strlen(Pattern);

    if (PatternLength > Length)
        return NULL;

    for (SIZE_T i = 0; i <= Length - PatternLength; i++)
    {
        if (RtlCompareMemory(
                Buffer + i,
                Pattern,
                PatternLength) == PatternLength)
        {
            return Buffer + i;
        }
    }

    return NULL;
}

static
ULONG
ParseDecimal(
    PCSTR String
)
{
    ULONG Value = 0;

    while (*String >= '0' &&
           *String <= '9')
    {
        Value *= 10;
        Value += (*String - '0');
        String++;
    }

    return Value;
}

static
BOOLEAN
LoadFeature(
    PCHAR Hex,
    SIZE_T HexLength,
    ULONG FeatureType,
    QDCM_PROFILE* Profile
)
{
    switch (FeatureType)
    {
    case 2:
        return LoadFeature2(
            Hex,
            HexLength,
            Profile);

    case 7:
        if (!HexStringToBytes(
                Hex,
                HexLength,
                Profile->Feature7Raw,
                sizeof(Profile->Feature7Raw)))
        {
            return FALSE;
        }

        RtlCopyMemory(
            &Profile->Feature7,
            Profile->Feature7Raw,
            min(sizeof(Profile->Feature7),
                sizeof(Profile->Feature7Raw)));

        Profile->HasFeature7 = TRUE;

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Feature7 accepted");
        return TRUE;

    case 8:
        if (!HexStringToBytes(
                Hex,
                HexLength,
                Profile->Feature8Raw,
                sizeof(Profile->Feature8Raw)))
        {
            return FALSE;
        }

        RtlCopyMemory(
            &Profile->Feature8,
            Profile->Feature8Raw,
            min(sizeof(Profile->Feature8),
                sizeof(Profile->Feature8Raw)));

        Profile->HasFeature8 = TRUE;

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Feature8 accepted");
        return TRUE;
    default:
        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Ignoring FeatureType=%lu",
            FeatureType);

        return TRUE;
    }
}

BOOLEAN
CompactHexString(
    PCSTR Input,
    SIZE_T InputLength,
    PCHAR Output,
    SIZE_T OutputSize,
    SIZE_T* OutputLength
)
{
    SIZE_T i;
    SIZE_T j = 0;

    if (Input == NULL ||
        Output == NULL ||
        OutputLength == NULL)
    {
        return FALSE;
    }

    *OutputLength = 0;

    for (i = 0; i < InputLength; i++)
    {
        CHAR c = Input[i];

        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'))
        {
            if (j >= OutputSize)
            {
                return FALSE;
            }

            Output[j++] = c;
        }
    }

    *OutputLength = j;
    return TRUE;
}

BOOLEAN
ParseQdcmXml(
    PCHAR Xml,
    SIZE_T XmlLength,
    QDCM_PROFILE* Profile
)
{
    PCHAR Current;

    if (Xml == NULL ||
        Profile == NULL)
    {
        return FALSE;
    }

    RtlZeroMemory(
        Profile,
        sizeof(*Profile));

    Current = Xml;

    while (TRUE)
    {
        PCHAR FeatureTag;
        PCHAR TypeTag;
        PCHAR PayloadStart;
        PCHAR PayloadEnd;
        ULONG FeatureType;

        CHAR* CompactBuffer;
        SIZE_T CompactLength;
        SIZE_T RawLength;

        FeatureTag =
            FindString(
                Current,
                XmlLength - (Current - Xml),
                "<Feature");

        if (FeatureTag == NULL)
        {
            break;
        }

        TypeTag =
            FindString(
                FeatureTag,
                XmlLength - (FeatureTag - Xml),
                "FeatureType=\"");

        if (TypeTag == NULL)
        {
            return FALSE;
        }

        TypeTag += strlen("FeatureType=\"");

        FeatureType = ParseDecimal(TypeTag);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Found FeatureType=%lu",
            FeatureType);

        PayloadStart = strchr(TypeTag, '>');

        if (PayloadStart == NULL)
        {
            return FALSE;
        }

        PayloadStart++;

        PayloadEnd =
            FindString(
                PayloadStart,
                XmlLength - (PayloadStart - Xml),
                "</Feature>");

        if (PayloadEnd == NULL)
        {
            return FALSE;
        }

        RawLength =
            (SIZE_T)(PayloadEnd - PayloadStart);

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Feature payload length=%Iu",
            RawLength);

        CompactBuffer = ExAllocatePoolWithTag(
            NonPagedPoolNx,
            RawLength + 1,
            'MCDQ');

        if (CompactBuffer == NULL)
        {
            return FALSE;
        }

        if (!CompactHexString(
                PayloadStart,
                RawLength,
                CompactBuffer,
                RawLength,
                &CompactLength))
        {
            ExFreePool(CompactBuffer);
            CompactBuffer = NULL;
            return FALSE;
        }

        TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_DRIVER,
            "Compacted XML bytes=%Iu",
            CompactLength);

        CompactBuffer[CompactLength] = '\0';

        if (!LoadFeature(
                CompactBuffer,
                CompactLength,
                FeatureType,
                Profile))
        {
            ExFreePool(CompactBuffer);
            CompactBuffer = NULL;
            return FALSE;
        }

        ExFreePool(CompactBuffer);
        CompactBuffer = NULL;

        Current = PayloadEnd;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "QDCM parsed F2=%u F7=%u F8=%u",
        Profile->HasFeature2,
        Profile->HasFeature7,
        Profile->HasFeature8);

    return
        Profile->HasFeature2 &&
        Profile->HasFeature7 &&
        Profile->HasFeature8;
}

NTSTATUS
LoadQdcmCalibrationFromSfpd(
    WDFDEVICE Device,
    PCWSTR XmlPath,
    SDP_CALIBRATION_BUFFER* Calibration
)
{
    NTSTATUS Status;
    DWORD Size;
    PCHAR XmlBuffer;
    QDCM_PROFILE* Profile;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Loading QDCM XML: %ws",
        XmlPath);

    Status = GetSFPDItemSize(
        Device,
        (PWSTR)XmlPath,
        &Size);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "XML size=%lu",
        Size);

    XmlBuffer = ExAllocatePoolWithTag(
        NonPagedPoolNx,
        Size + 1,
        'MCDQ');

    if (!XmlBuffer)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    Status = GetSFPDItem(
        Device,
        (PWSTR)XmlPath,
        XmlBuffer,
        Size);

    if (!NT_SUCCESS(Status))
    {
        ExFreePool(XmlBuffer);
        return Status;
    }

    XmlBuffer[Size] = '\0';

    Profile = ExAllocatePoolWithTag(
        NonPagedPoolNx,
        sizeof(QDCM_PROFILE),
        'MCDQ');

    if (Profile == NULL)
    {
        ExFreePool(XmlBuffer);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (!ParseQdcmXml(
            XmlBuffer,
            Size,
            Profile))
    {
        ExFreePool(Profile);
        Profile = NULL;
        ExFreePool(XmlBuffer);
        XmlBuffer = NULL;
        return STATUS_DATA_ERROR;
    }

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Feature7 Header0=%lu EntryCount=%lu Header2=%lu",
        Profile->Feature7.Header0,
        Profile->Feature7.EntryCount,
        Profile->Feature7.Header2);

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "Feature8 Header0=%lu EntryCount=%lu Header2=%lu",
        Profile->Feature8.Header0,
        Profile->Feature8.EntryCount,
        Profile->Feature8.Header2);

    if (!BuildFromProfile(
            Calibration,
            Profile))
    {
        ExFreePool(Profile);
        ExFreePool(XmlBuffer);
        return STATUS_DATA_ERROR;
    }

    ExFreePool(Profile);
    ExFreePool(XmlBuffer);

    return STATUS_SUCCESS;
}