#pragma once

#define FEATURE7_XML_ENTRIES      256
#define FEATURE7_HW_ENTRIES       256

#define FEATURE7_WEIGHT_COMPLETENESS 60
#define FEATURE7_WEIGHT_MONOTONICITY 30
#define FEATURE7_WEIGHT_SMOOTHNESS   10

#define FEATURE7_MAX_ENTRY_VALUE   4095

#define FEATURE7_SLOPE_WINDOW      16

#define FEATURE7_SCORE_HEALTHY     80
#define FEATURE7_SCORE_RECOVERABLE 60

typedef struct
{
    UINT32 Header0;
    UINT32 EntryCount;
    UINT32 Header2;
    UINT32 Reserved;

    UINT32 Red[FEATURE7_XML_ENTRIES];
    UINT32 Green[FEATURE7_XML_ENTRIES];
    UINT32 Blue[FEATURE7_XML_ENTRIES];
} QDCM_FEATURE7;

typedef struct
{
    UINT32 Header0;
    UINT32 EntryCount;
    UINT32 Header2;
    UINT32 Reserved;

    UINT32 Red[1024];
    UINT32 Green[1024];
    UINT32 Blue[1024];
} QDCM_FEATURE8;

typedef struct _QDCM_PROFILE
{
    BOOLEAN HasFeature2;
    BOOLEAN HasFeature7;
    BOOLEAN HasFeature8;

    UCHAR Feature2[0x110];

    QDCM_FEATURE7 Feature7;
    QDCM_FEATURE8 Feature8;

    UCHAR Feature7Raw[12300];
    UCHAR Feature8Raw[12300];
} QDCM_PROFILE;

typedef struct
{
    ULONG ValidEntries;

    ULONG CompletenessScore;
    ULONG MonotonicityScore;
    ULONG SmoothnessScore;

    ULONG TotalScore;

} QDCM_CHANNEL_HEALTH;


C_ASSERT(
    sizeof(QDI_Panel_InverseGammaCorrectionTableType)
    == 0xC08
);

C_ASSERT(
    sizeof(SDP_CAL_LINEAR_GAMMA_PAYLOAD)
    == 0x1810
);

C_ASSERT(
    FIELD_OFFSET(
        SDP_CALIBRATION_BUFFER,
        InverseGamma
    ) == 0x110
);

C_ASSERT(
    FIELD_OFFSET(
        SDP_CALIBRATION_BUFFER,
        LinearGammaPayload
    ) == 0x0D18
);

C_ASSERT(
    FIELD_OFFSET(
        SDP_CALIBRATION_BUFFER,
        LinearGamma
    ) == 0x2528
);

C_ASSERT(
    sizeof(QDI_Panel_ColorCorrectionMatrixType) == 0x110
);

C_ASSERT(sizeof(QDCM_FEATURE8) == 0x3010);
C_ASSERT(sizeof(QDCM_FEATURE7) == 0x0C10);

C_ASSERT(
    sizeof(SDP_CAL_GAMUT_PAYLOAD) ==
    SDP_CAL_SIZE_GAMUT
);

C_ASSERT(
    FIELD_OFFSET(
        QDCM_FEATURE7,
        Green) ==
    0x410);

C_ASSERT(
    FIELD_OFFSET(
        QDCM_FEATURE7,
        Blue) ==
    0x810);

C_ASSERT(
    sizeof(QDCM_FEATURE8) ==
    0x3010);