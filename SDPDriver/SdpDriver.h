#pragma once

// SDPDriver - OEM display panel driver for the Surface Duo style dual screen
// "SDP" panels (ACPI\MSHW0160). The driver sits next to the Qualcomm display
// miniport: the Graphics KMD sends IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE with
// its QDI_DisplayUtil_FunctionTableType, and the driver registers one
// QDI_Panel_FunctionTableType per panel (QDI_DISPLAY_PRIMARY/SECONDARY).
//
// All panel work (DSI sequences, GPIO reset, brightness, ESD) runs on one
// worker thread per panel that consumes an action queue.

// ntifs.h has to come first (displayfw.h/oempanel.h pull in ntddk.h).
#define NTSTRSAFE_LIB
#include <ntifs.h>
#include <ntddk.h>
#include <ntdddisk.h>
#include <wdf.h>
#include <acpiioct.h>
#include <gpio.h>
#include <reshub.h>
#include <stdarg.h>
#include <ntstrsafe.h>

#include "oempanel.h"
#include "qdi_oem.h"

#include "SdpPublic.h"
#include "SdpMessages.h"
#include "Trace.h"

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

#define SDP_POOL_TAG            '1PmO'  // "OmP1" in pool dumps: general allocations
#define SDP_POOL_TAG_ACPI       '2PmO'  // "OmP2": ACPI _DSM buffers
#define SDP_POOL_TAG_PANELBIN   '0DPQ'  // "QPD0": panel bin file path/memory
#define SDP_POOL_TAG_PARTITION  '0FHP'  // "PHF0": partition path buffers

#define SDP_PANEL_COUNT         2
#define SDP_INVALID_PANEL_INDEX 0xF
#define SDP_PANEL_ID_3406       0x3406  // the only panel id with an action table
#define SDP_PANEL_ID_3416       0x3416
#define SDP_PANEL_ID_INVALID    0xFFFF

// The binary registers its panel table with this API version (qdi_oem.h of the
// QCDK in use ships 0x2010057).
#define SDP_QDI_PANEL_API_VERSION   0x02010056

// QDI_DISPLAY_PRIMARY / QDI_DISPLAY_SECONDARY map to panel slot 0 / 1.
#define SDP_MAX_PANEL_NITS_MILLI    545369   // 0x85259, upper bound of the nits range (milli-nits)
#define SDP_MIN_PANEL_NITS_MILLI    10000

#define SDP_ACTION_QUEUE_DEPTH      100
#define SDP_ESD_POLL_INTERVAL_MS    10000    // WdfTimer period between ESD / temperature polls
#define SDP_MAX_DSI_PAYLOAD         0x1000
#define SDP_PANEL_BIN_ID_BYTES      9
#define SDP_PANEL_BIN_SIZE          0x20

// Device state values kept in DEVICE_CONTEXT::State.
#define SDP_DEVICE_STATE_NONE           0
#define SDP_DEVICE_STATE_INTERFACE_SET  1   // QDI interface table received
#define SDP_DEVICE_STATE_DRIVER_LOADED  2   // QDI_PANEL_EVENT_DRIVER_LOADED seen

// Panel state values kept in PANEL_CONTEXT::State.
#define SDP_PANEL_STATE_RESET_DONE      1
#define SDP_PANEL_STATE_OFF             2
#define SDP_PANEL_STATE_POWER_ON        3
#define SDP_PANEL_STATE_POWER_OFF       4
#define SDP_PANEL_STATE_MODE_SET        5
#define SDP_PANEL_STATE_BRIGHTNESS      6
#define SDP_PANEL_STATE_INITIALIZED     7
#define SDP_PANEL_STATE_TERMINATING     8

// Capability / feature mask kept in PANEL_CONTEXT::Caps. Bit 0 is set when
// the "MfgModeCurrent" firmware variable is 0 (normal mode), in which case
// DriverInit also sets bits 1..4.
#define SDP_CAPS_NORMAL_MODE            0x01
#define SDP_CAPS_POWERSAVE_LEVEL0       0x02  // panel info: power saving override = LEVEL0
#define SDP_CAPS_SKIP_EXTRA_INIT_CMD    0x04  // skip one optional DCS command in the init sequence
#define SDP_CAPS_NO_TEMP_COMPENSATION   0x08  // ignore TemperatureCompensation actions
#define SDP_CAPS_NO_THERMAL_THROTTLE    0x10  // ignore thermal throttle interface calls

// Dirty bits kept in CALIBRATION_BUFFER::DirtyFlags.
#define SDP_CAL_DIRTY_COLOR_MATRIX      0x01
#define SDP_CAL_DIRTY_INVERSE_GAMMA     0x02
#define SDP_CAL_DIRTY_LINEAR_GAMMA      0x04
#define SDP_CAL_DIRTY_GAMUT_MAPPING     0x08
#define SDP_CAL_DIRTY_BRIGHTNESS        0x10
#define SDP_CAL_DIRTY_AL_ENHANCEMENT    0x20
#define SDP_CAL_DIRTY_AMBIENT_LEVEL     0x40
#define SDP_CAL_DIRTY_DITHERING         0x80

// ---------------------------------------------------------------------------
// Display actions (worker thread requests)
// ---------------------------------------------------------------------------

// Two flag bits are or'ed into the action id by the senders. Both names are
// inferred (the binary only clears them in ProcessDisplayAction): bit 31 is
// set by the IOCTL handlers, the ESD poll and the thermal report and is the only
// one that is tested - DsiSendCmd / DsiReadCmd are ignored without it; bit 30 is
// set by the QDI callbacks and by the IOCTL handlers of the property / AL
// enhancement requests and is never tested.
#define SDP_ACTION_FLAG_USER    0x80000000
#define SDP_ACTION_FLAG_QDI     0x40000000
#define SDP_ACTION_ID_MASK      0x3FFFFFFF

typedef enum _SDP_DISPLAY_ACTION
{
    DisplayActionNone = 0,
    DisplayActionOn = 1,
    DisplayActionOff = 2,
    DisplayActionInitSequence = 3,
    DisplayActionTermSequence = 4,
    DisplayActionDisplayId = 5,
    DisplayActionSetBrightness = 6,
    DisplayActionDriverInit = 7,
    DisplayActionDriverTerminate = 8,
    DisplayActionSetMode = 9,
    DisplayActionPower = 10,
    DisplayActionPrePower = 11,
    DisplayActionPostPower = 12,
    DisplayActionDsiSendCmd = 13,
    DisplayActionDsiReadCmd = 14,
    DisplayActionToggleReset = 15,
    DisplayActionDriverLoaded = 16,
    DisplayActionDisplayReset = 17,
    DisplayActionFrameBufferInfo = 18,
    DisplayActionUpdateFrameBuffer = 19,
    DisplayActionFrameBufferClear = 20,
    DisplayActionGetDisplayPropertyInfo = 21,
    DisplayActionSetDisplayPropertyInfo = 22,
    DisplayActionSetPixelReg = 23,
    DisplayActionGetBrightness = 24,
    DisplayActionGetMfgInfo = 25,
    DisplayActionGetPowerMode = 32,
    DisplayActionSelfTest = 33,
    // Ids 0..25, 32, 33 and 0xFF are named by the binary's own DisplayActionToString (140006938),
    // the names below are those strings. The binary prints "Invalid Action" for every other id,
    // so the following names are inferred from what the handler does and who sends the action.
    DisplayActionFlushProperties = 35,      // push dirty calibration data through QDI SetProperty
    DisplayActionCommitProperties = 37,     // QDI SetProperty with a (non async) commit config
    DisplayActionDualPanelSyncTest = 38,
    DisplayActionSetBrightnessNits = 39,
    DisplayActionEsdReset = 40,
    DisplayActionReserved41 = 41,           // empty case of the dispatcher, never sent
    DisplayActionInitPanelAttr = 48,
    DisplayActionTemperatureCompensation = 49,
    DisplayActionApplyAlEnhancement = 50,
    DisplayActionExitThread = 0xFF          // string "ExitDisplayThread"; sent by OemPanelDriverOnDriverUnload only
} SDP_DISPLAY_ACTION;

// Entry of the per-panel action ring (0x30 bytes).
typedef struct _SDP_ACTION_NODE
{
    ULONG Action;               // SDP_DISPLAY_ACTION | SDP_ACTION_FLAG_*
    ULONG Reserved;
    PVOID Arg1;
    PVOID Arg2;
    PKEVENT CompletionEvent;    // signalled by UnBlockDisplayActionThread
    PLONG Status;               // receives the handler status
    BOOLEAN Skip;               // 0x28: the worker ignores a node with a non-zero byte here (140013fe8); always 0 when queued
    UCHAR Padding29[7];         // left uninitialised by the original AddDisplayActionRequestHelper
} SDP_ACTION_NODE;

// Per-panel action ring buffer, 0x1320 bytes.
typedef struct _SDP_ACTION_RING
{
    ULONG ReadIndex;
    ULONG WriteIndex;
    SDP_ACTION_NODE Nodes[SDP_ACTION_QUEUE_DEPTH];
    KSEMAPHORE ItemsAvailable;
    KGUARDED_MUTEX Lock;
} SDP_ACTION_RING;

// ---------------------------------------------------------------------------
// DSI command sequences
// ---------------------------------------------------------------------------

#define SDP_DSI_NODE_READ   0
#define SDP_DSI_NODE_WRITE  1

// DSI data types used by the init sequences.
#define SDP_DSI_DT_DCS_SHORT_WRITE_0    0x05
#define SDP_DSI_DT_DCS_SHORT_WRITE_1    0x15
#define SDP_DSI_DT_DCS_LONG_WRITE       0x39
#define SDP_DSI_DT_SET_MAX_RETURN_SIZE  0x37
#define SDP_DSI_DT_DCS_READ_0           0x06
// Pseudo packet type that only carries a 1 byte payload: the delay in ms.
#define SDP_DSI_DT_DELAY_MS             0xFF

#define SDP_SEQ_NODE_FLAGS_DEFAULT      2   // value stored when the caller passes 0

// A node of a DSI command list. The head of every list is embedded in the
// panel context and is itself the first node (Next at +0x28 links the rest).
typedef struct _SDP_DSI_NODE
{
    ULONG Type;                 // SDP_DSI_NODE_READ / SDP_DSI_NODE_WRITE
    ULONG Flags;                // transfer flags (or'ed into TxFlags when sent)
    UCHAR PacketType;           // DSI packet data type
    UCHAR Reserved[7];
    PUCHAR Data;
    ULONG DataSize;
    ULONG Reserved1C;
    ULONG TxFlags;
    BOOLEAN Persistent;         // 1: list survives sending (heap nodes are freed otherwise)
    UCHAR Reserved25[3];
    struct _SDP_DSI_NODE* Next;
} SDP_DSI_NODE;

//C_ASSERT(sizeof(SDP_DSI_NODE) == 0x30);

// Parameters of IOCTL_SDP_DSI_READ as handed to the worker (DisplayActionDsiReadCmd).
typedef struct _SDP_DSI_READ_REQUEST
{
    PUCHAR Data;                // [0] = read packet type, followed by the request payload
    ULONG DataSize;
    ULONG ReadSize;
} SDP_DSI_READ_REQUEST;

// ---------------------------------------------------------------------------
// Panel description handed to QDI
// ---------------------------------------------------------------------------

// Defaults for one panel (written by InitPanelInfo and used by FillPanelAttr to
// populate QDI_Panel_AttrType). Lives at PANEL_CONTEXT + 0x1548. The fields
// named ReservedXX are written by FillMeta2PanelProperties (14000fc40) but read by
// nothing in the image (not by FillPanelAttr, not by an IOCTL), so no name can
// be derived for them; the values written are kept as a record.
typedef struct _SDP_PANEL_INFO
{
    ULONG PanelId;                  // 0x00: SDP_PANEL_ID_3406
    ULONG Reserved04;
    UCHAR Reserved08;
    UCHAR Reserved09[3];
    ULONG Reserved0C;
    ULONG Reserved10;
    ULONG Reserved14;
    ULONG PhysConnect;              // 0x18: QDI_DISPLAY_CONNECT_PRIMARY_DSI_CMD / SECONDARY_DSI_VIDEO
    ULONG IsVideoMode;              // 0x1C: 0 => QDI_PANEL_INTERFACE_COMMAND, else ACTIVE
    ULONG DataLanes;                // 0x20
    UCHAR LaneRemap;                // 0x24
    UCHAR LP11AtInit;               // 0x25
    UCHAR PhyDcdcMode;              // 0x26
    UCHAR DisableEotp;              // 0x27
    ULONG ControllerIndex;          // 0x28 (uControllerMapping[0])
    ULONG ControllerMask;           // 0x2C
    UCHAR DscEnable;                // 0x30
    UCHAR Reserved31[3];
    ULONG DscProfileId;             // 0x34
    ULONG DscSliceWidth;            // 0x38
    ULONG DscSliceHeight;           // 0x3C
    ULONG DscMajorVersion;          // 0x40
    ULONG DscMinorVersion;          // 0x44
    ULONG DscScrVersion;            // 0x48
    UCHAR BitClockOverride;         // 0x4C
    UCHAR Reserved4D[3];
    ULONG BitClockFrequency;        // 0x50
    UCHAR BitClkScaleOverride;      // 0x54
    UCHAR Reserved55[3];
    ULONG BitClkScalePercent;       // 0x58
    UCHAR Reserved5C;
    UCHAR Reserved5D[3];
    ULONG Width;                    // 0x60
    ULONG Height;                   // 0x64
    UCHAR Reserved68;
    UCHAR Reserved69;
    USHORT RefreshRate;             // 0x6A (Hz, written as 16.16 fixed point)
    ULONG Reserved6C;
    UCHAR DedicatedTePin;           // 0x70
    UCHAR Reserved71[3];
    ULONG VsyncStartPos;            // 0x74
    ULONG VsyncContinueLines;       // 0x78
    ULONG VsyncStartLineDivisor;    // 0x7C
    ULONG VsyncPercentVariance;     // 0x80
    ULONG VsyncSelect;              // 0x84
    ULONG Reserved88[10];           // 0x88..0xAF
    ULONG ClockHsForceRequest;      // 0xB0
    USHORT Reserved1[3];            // 0xB4..0xB9
    USHORT PanelParam0;             // 0xBA (reported by IOCTL_SDP_GET_PANEL_PARAMS)
    USHORT PanelParam1;             // 0xBC
    USHORT Reserved2;               // 0xBE
    ULONG PowerSavingOverride;      // 0xC0
    UCHAR EnterUlps;                // 0xC4
    UCHAR Reserved3[3];
    ULONG DsiFlags;                 // 0xC8
    ULONG DisplayFlags;             // 0xCC
    ULONG EdidSerial;               // 0xD0
    UCHAR EdidWeek;                 // 0xD4
    UCHAR EdidYear;                 // 0xD5
    UCHAR Reserved4[2];
    ULONG EscClockFreq;             // 0xD8
    ULONG EscClockDivisor;          // 0xDC
} SDP_PANEL_INFO;

/*C_ASSERT(sizeof(SDP_PANEL_INFO) == 0xE0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, Width) == 0x60);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, RefreshRate) == 0x6A);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, ClockHsForceRequest) == 0xB0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, PanelParam0) == 0xBA);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, PowerSavingOverride) == 0xC0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_INFO, EscClockDivisor) == 0xDC);*/

// ---------------------------------------------------------------------------
// Calibration buffer (one per panel, 0x3748 bytes, the "refresh rate" data)
// ---------------------------------------------------------------------------

#define SDP_CAL_BUFFER_SIZE         0x3748
#define SDP_CAL_SECTION_GAMUT       0   // QDI_Panel_GamutMappingTableType payload, 0x1122 bytes
#define SDP_CAL_SECTION_INV_GAMMA   1   // QDI_Panel_InverseGammaCorrectionTableType payload, 0xC08 bytes
#define SDP_CAL_SECTION_LIN_GAMMA   2   // QDI_Panel_LinearGammaCorrectionTableType payload, 0x1810 bytes
#define SDP_CAL_SECTION_CCM         3   // QDI_Panel_ColorCorrectionMatrixType, 0x110 bytes
#define SDP_CAL_SECTION_BRIGHTNESS  4   // 8 bytes: initial nits and transition

#define SDP_CAL_SIZE_GAMUT          0x1122
#define SDP_CAL_SIZE_INV_GAMMA      0xC08
#define SDP_CAL_SIZE_LIN_GAMMA      0x1810
#define SDP_CAL_SIZE_CCM            0x110
#define SDP_CAL_SIZE_BRIGHTNESS     8

#define SDP_CAL_GAMUT_SAMPLES       729         // 0x2D9

typedef struct _SDP_CAL_HEADER_ENTRY
{
    ULONG Type;
    ULONG Offset;
    ULONG Size;
} SDP_CAL_HEADER_ENTRY;

// Header of the calibration blob handed in by IOCTL_SDP_SET_CALIBRATION /
// the a01 IOCTL.
typedef struct _SDP_CAL_HEADER
{
    ULONG Reserved0;
    ULONG DataEnd;                      // data size; section offsets must stay below
    ULONG DisplayId;                    // QDI_DISPLAY_PRIMARY / SECONDARY
    ULONG EntryCount;
    SDP_CAL_HEADER_ENTRY Entries[1];
} SDP_CAL_HEADER;

#pragma pack(push, 1)
// Brightness of the calibration blob (section 4).
typedef struct _SDP_CAL_BRIGHTNESS
{
    ULONG Nits;
    ULONG TransitionMs;
} SDP_CAL_BRIGHTNESS;
#pragma pack(pop)

// Section 2 payload; the QDI linear gamma table is built on top of it.
typedef struct _SDP_CAL_LINEAR_GAMMA_PAYLOAD
{
    ULONG Reserved0;
    ULONG Enable;
    ULONG NumCoefficients;
    ULONG Reserved3;
    USHORT Red[1024];
    USHORT Green[1024];
    USHORT Blue[1024];
} SDP_CAL_LINEAR_GAMMA_PAYLOAD;

// Section 0 payload; the QDI gamut mapping table points into it.
#pragma pack(push, 1)
typedef struct _SDP_CAL_GAMUT_PAYLOAD
{
    LONG Id;                            // -1: section not valid
    ULONG Enabled;
    ULONG GamutBeforePcc;
    USHORT Red[SDP_CAL_GAMUT_SAMPLES];
    USHORT Green[SDP_CAL_GAMUT_SAMPLES];
    USHORT Blue[SDP_CAL_GAMUT_SAMPLES];
} SDP_CAL_GAMUT_PAYLOAD;
#pragma pack(pop)

// User supplied ambient light enhancement settings (IOCTL_SDP_SET_AL_ENHANCEMENT).
typedef struct _SDP_AL_ENHANCEMENT_REQUEST
{
    ULONG Enable;
    ULONG MaxIterations;
    ULONG CalibrationA;
    ULONG CalibrationB;
    ULONG CalibrationC;
    ULONG CalibrationD;
    ULONG StrengthLimit;
    ULONG TFilterControl;
    ULONG AmbientLevel;                 // low 16 bits only are kept
} SDP_AL_ENHANCEMENT_REQUEST;

// Calibration data of one panel (0x3748 bytes, WDFMEMORY allocated).
typedef struct _SDP_CALIBRATION_BUFFER
{
    QDI_Panel_ColorCorrectionMatrixType Ccm;                    // 0x0000 section 3 payload
    QDI_Panel_InverseGammaCorrectionTableType InverseGamma;     // 0x0110 section 1 payload
    SDP_CAL_LINEAR_GAMMA_PAYLOAD LinearGammaPayload;            // 0x0D18 section 2 payload
    QDI_Panel_LinearGammaCorrectionTableType LinearGamma;       // 0x2528 built from the payload
    SDP_CAL_GAMUT_PAYLOAD GamutPayload;                         // 0x2550 section 0 payload
    UCHAR Reserved3672[6];
    QDI_Panel_GamutMappingTableType Gamut;                      // 0x3678 built from the payload
    SDP_CAL_BRIGHTNESS Brightness;                              // 0x36C8 section 4 payload
    SDP_AL_ENHANCEMENT_REQUEST AlRequest;                       // 0x36D0 raw user input
    QDI_Panel_ALEnhancementEnable AlEnable;                     // 0x36F4
    QDI_Panel_ALEnhancementCalibrationType AlCalibration;       // 0x36FC
    QDI_Panel_DitheringMatrixType Dithering;                    // 0x3718
    ULONG AmbientLightLevel;                                    // 0x3740
    ULONG DirtyFlags;                                           // 0x3744 SDP_CAL_DIRTY_*
} SDP_CALIBRATION_BUFFER;

#define SDP_NITS_LUT_COUNT      255

// One point of the DBV / luminance curve.
typedef struct _SDP_NITS_LUT_ENTRY
{
    ULONG Dbv;
    ULONG MilliNits;
} SDP_NITS_LUT_ENTRY;

typedef struct _PANEL_RUNTIME_CALIBRATION
{
    BOOLEAN Valid;
    SDP_NITS_LUT_ENTRY RuntimeLut[SDP_NITS_LUT_COUNT];
} PANEL_RUNTIME_CALIBRATION;

// ---------------------------------------------------------------------------
// Threads
// ---------------------------------------------------------------------------

typedef VOID (*PFN_SDP_THREAD_ROUTINE)(_In_ PVOID Context);

// Helper object around PsCreateSystemThread, 0x88 bytes.
typedef struct _SDP_THREAD
{
    HANDLE Handle;                  // 0x00
    PKTHREAD Thread;                // 0x08
    PVOID Reserved10;               // 0x10
    KEVENT Event0;                  // 0x18 synchronization (or notification) event
    KEVENT StopEvent;               // 0x30
    PFN_SDP_THREAD_ROUTINE StartRoutine;    // 0x48
    KGUARDED_MUTEX Lock;            // 0x50
} SDP_THREAD;

/*C_ASSERT(sizeof(SDP_THREAD) == 0x88);
C_ASSERT(FIELD_OFFSET(SDP_THREAD, StartRoutine) == 0x48);*/

// ---------------------------------------------------------------------------
// Reset GPIO (resource hub connection) of a panel, 0x18 bytes
// ---------------------------------------------------------------------------

typedef struct _SDP_RESET_GPIO
{
    ULONG PreDelayUs;               // 0x00
    ULONG PostDelayUs;              // 0x04
    ULONG HoldUs;                   // 0x08
    BOOLEAN ActiveHigh;             // 0x0C
    BOOLEAN ExplicitLevels;         // 0x0D: use the level based modes instead of the toggle sequence
    UCHAR Reserved0E[2];
    WDFIOTARGET Target;             // 0x10
} SDP_RESET_GPIO;

// Modes of ToggleResetLine.
#define SDP_RESET_GPIO_DEASSERT         1   // drive the inactive level
#define SDP_RESET_GPIO_ASSERT           2   // drive the active level (hold the panel in reset)
#define SDP_RESET_GPIO_PULSE            3   // inactive -> active -> inactive

struct _DEVICE_CONTEXT;

// ---------------------------------------------------------------------------
// Panel operations (function pointers, panel specific)
// ---------------------------------------------------------------------------

struct _SDP_PANEL_CONTEXT;

typedef struct _SDP_TEMP_RESULT
{
    ULONG Reserved[4];
    ULONG Percent;                  // 0x10: brightness limit in percent
    ULONG MaxNits;                  // 0x14: panel max nits (milli-nits) at that temperature
} SDP_TEMP_RESULT;

typedef NTSTATUS (*PFN_SDP_PANEL_OP)(_In_ struct _SDP_PANEL_CONTEXT* Panel);
typedef VOID (*PFN_SDP_PANEL_RESET_SEQUENCES)(_In_ struct _SDP_PANEL_CONTEXT* Panel);
typedef NTSTATUS (*PFN_SDP_PANEL_GET_MFG_INFO)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _Out_ ULONG* MfgInfo);
typedef BOOLEAN (*PFN_SDP_PANEL_IS_NITS_VALID)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ ULONG MilliNits);
typedef NTSTATUS (*PFN_SDP_PANEL_NITS_TO_DBV)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ ULONG MilliNits, _Out_ ULONG* Dbv);
typedef NTSTATUS (*PFN_SDP_PANEL_SET_DBV)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ ULONG Dbv);
typedef NTSTATUS (*PFN_SDP_PANEL_READ_BYTE)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _Out_ UCHAR* Value);
typedef NTSTATUS (*PFN_SDP_PANEL_WRITE_BYTE)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ UCHAR* Value);
typedef NTSTATUS (*PFN_SDP_PANEL_GET_POWER_MODE)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _Out_ CHAR* Mode, _Out_opt_ UCHAR* IsNormal);
typedef NTSTATUS (*PFN_SDP_PANEL_TEMP_COMP)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ ULONG Temperature, _Inout_ SDP_TEMP_RESULT* Result);
typedef NTSTATUS (*PFN_SDP_PANEL_DUAL_TEST)(_In_ struct _SDP_PANEL_CONTEXT* Panel, _In_ struct _SDP_PANEL_CONTEXT* Other, _Out_ ULONG* Result);
typedef VOID (*PFN_SDP_PANEL_SET_CAPS)(_In_ struct _SDP_PANEL_CONTEXT* Panel);
typedef VOID (*PFN_SDP_PANEL_THERMAL)(_In_ struct _DEVICE_CONTEXT* Device, _In_ ULONG Temperature, _Out_ UCHAR* Changed);

// Function table installed by InitPanelActionTable for panel id 0x3406.
typedef struct _SDP_PANEL_OPS
{
    PFN_SDP_PANEL_OP BuildSequences;                // 0x18E0
    PFN_SDP_PANEL_RESET_SEQUENCES ResetSequences;   // 0x18E8
    PFN_SDP_PANEL_OP InitPanelInfo;                 // 0x18F0
    PFN_SDP_PANEL_GET_MFG_INFO GetMfgInfo;          // 0x18F8
    PFN_SDP_PANEL_OP SendInitSequence;              // 0x1900
    PFN_SDP_PANEL_OP SendTermSequence;              // 0x1908
    PFN_SDP_PANEL_IS_NITS_VALID IsNitsValid;        // 0x1910
    PFN_SDP_PANEL_NITS_TO_DBV NitsToDbv;            // 0x1918
    PFN_SDP_PANEL_SET_DBV SetDbv;                   // 0x1920
    PFN_SDP_PANEL_READ_BYTE GetBrightness;          // 0x1928
    PFN_SDP_PANEL_WRITE_BYTE SetPixelReg;           // 0x1930
    PFN_SDP_PANEL_GET_POWER_MODE GetPowerMode;      // 0x1938
    PFN_SDP_PANEL_TEMP_COMP TemperatureCompensate;  // 0x1940
    PFN_SDP_PANEL_READ_BYTE GetDiagnostic;          // 0x1948
    PFN_SDP_PANEL_DUAL_TEST DualPanelSyncTest;      // 0x1950
    PFN_SDP_PANEL_SET_CAPS SetCaps;                 // 0x1958
    PFN_SDP_PANEL_THERMAL ThermalThrottle;          // 0x1960
} SDP_PANEL_OPS;

// ---------------------------------------------------------------------------
// Panel context: one per physical panel (slot 0 = primary, slot 1 = secondary)
// ---------------------------------------------------------------------------

typedef struct _SDP_PANEL_CONTEXT
{
    ULONG PanelId;                                  // 0x0000
    ULONG PanelVersion;                             // 0x0004
    WDFDEVICE Device;                               // 0x0008
    WDFQUEUE StatusQueue;                           // 0x0010 manual queue of pending status requests
    SDP_THREAD StatusThread;                        // 0x0018
    PVOID StatusWaitObjects[3];                     // 0x00A0
    UCHAR Reserved00B8[8];                          // 0x00B8
    SDP_THREAD ActionThread;                        // 0x00C0
    UCHAR Reserved0148[0x20];                       // 0x0148
    HANDLE WorkerProcessId;                         // 0x0168
    SDP_ACTION_RING ActionRing;                     // 0x0170
    ULONG DisplayId;                                // 0x1490 QDI_Display_IDType
    ULONG Reserved1494;
    QDI_DisplayUtil_FunctionTableType DisplayUtil;  // 0x1498 copy of the Graphics KMD table
    UCHAR Reserved14E8[0x60];                       // 0x14E8
    SDP_PANEL_INFO Info;                            // 0x1548
    PUCHAR PanelBin;                                // 0x1628 DisplayRefreshRate-PanelN.bin contents
    ULONG PanelBinSize;                             // 0x1630
    ULONG HasCalibration;                           // 0x1634
    BOOLEAN PowerOn;                                // 0x1638
    BOOLEAN PanelAsleep;                            // 0x1639 term sequence was sent
    BOOLEAN DisplayOff;                             // 0x163A DCS display off / brightness 0 sent
    UCHAR Reserved163B;
    ULONG UpdateRequestCount;                       // 0x163C
    ULONG LastEvent;                                // 0x1640
    ULONG State;                                    // 0x1644 SDP_PANEL_STATE_*
    ULONG Reserved1648;
    ULONG VsyncSampleCount;                         // 0x164C
    ULONG VsyncSampleTarget;                        // 0x1650 (5)
    BOOLEAN VsyncEnabled;                           // 0x1654
    BOOLEAN VsyncMeasure;                           // 0x1655
    UCHAR Reserved1656[2];
    ULONG VsyncPeriodUs;                            // 0x1658 averaged vsync period (microseconds)
    ULONG VsyncLastDelta;                           // 0x165C
    ULONGLONG VsyncFirstTime;                       // 0x1660
    ULONGLONG VsyncLastTime;                        // 0x1668
    ULONG VsyncDeltas[5];                           // 0x1670
    ULONG EsdCount;                                 // 0x1684
    ULONG DsiHostErrors;                            // 0x1688
    ULONG DsiPanelErrors;                           // 0x168C
    ULONG DsiLastStatus;                            // 0x1690
    ULONG Reserved1694;
    LONG Temperature;                               // 0x1698 last temperature (Celsius)
    ULONG ThermalIndex;                             // 0x169C
    UCHAR Reserved16A0[0x10];                       // 0x16A0
    BOOLEAN InfoInitialized;                        // 0x16B0 set by InitPanelInfo
    UCHAR Reserved16B1[7];
    SDP_RESET_GPIO ResetGpio;                       // 0x16B8
    KGUARDED_MUTEX Lock;                            // 0x16D0
    USHORT FbWidth;                                 // 0x1708
    USHORT FbHeight;                                // 0x170A
    USHORT FbStride;                                // 0x170C
    USHORT Reserved170E;
    PVOID FbBuffer;                                 // 0x1710
    ULONG MfgInfo[7];                               // 0x1718 cached GetMfgInfo result (0x1C bytes)
    ULONG Reserved1734;
    KEVENT PanelOffEvent;                           // 0x1738 set after PostPower(off), D0Exit waits for it
    PUCHAR Calibration;                             // 0x1750 SDP_CALIBRATION_BUFFER (0x3748 bytes)
    QDI_Display_PropertyConfigType Properties;      // 0x1758
    UCHAR Reserved1810[8];                          // 0x1810
    SDP_DSI_NODE Sequences[4];                      // 0x1818 init, version, term, brightness
    BOOLEAN SequencesValid;                         // 0x18D8
    UCHAR Reserved18D9[7];
    SDP_PANEL_OPS Ops;                              // 0x18E0
    BOOLEAN EsdRecoveryPending;                     // 0x1968
    UCHAR Reserved1969[3];
    ULONG LastNitsScaled;                           // 0x196C
    ULONG RequestedNits;                            // 0x1970
    ULONG Brightness;                               // 0x1974 current DBV level
    ULONG Brightness2;                              // 0x1978
    ULONG ThermalPercent;                           // 0x197C
    ULONG ThermalMaxNits;                           // 0x1980
    QDI_Panel_BrightnessConfigType BrightnessConfig;// 0x1984
    ULONGLONG StatusData;                           // 0x1A30
    KEVENT StatusEvent;                             // 0x1A38
    ULONGLONG Caps;                                 // 0x1A50
    PANEL_RUNTIME_CALIBRATION RuntimeCalibration;
} SDP_PANEL_CONTEXT;

typedef SDP_PANEL_CONTEXT PANEL_CONTEXT, *PPANEL_CONTEXT;

/*C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Device) == 0x08);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, StatusQueue) == 0x10);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, StatusThread) == 0x18);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, StatusWaitObjects) == 0xA0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, ActionThread) == 0xC0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, WorkerProcessId) == 0x168);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, ActionRing) == 0x170);
C_ASSERT(sizeof(SDP_ACTION_RING) == 0x1320);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, DisplayId) == 0x1490);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, DisplayUtil) == 0x1498);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Info) == 0x1548);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, PanelBin) == 0x1628);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, PowerOn) == 0x1638);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, State) == 0x1644);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, VsyncSampleCount) == 0x164C);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, VsyncPeriodUs) == 0x1658);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, VsyncDeltas) == 0x1670);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, EsdCount) == 0x1684);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Temperature) == 0x1698);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, ResetGpio) == 0x16B8);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Lock) == 0x16D0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, FbWidth) == 0x1708);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, FbBuffer) == 0x1710);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, MfgInfo) == 0x1718);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, PanelOffEvent) == 0x1738);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Calibration) == 0x1750);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Properties) == 0x1758);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Sequences) == 0x1818);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, SequencesValid) == 0x18D8);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Ops) == 0x18E0);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, EsdRecoveryPending) == 0x1968);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, LastNitsScaled) == 0x196C);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, ThermalMaxNits) == 0x1980);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, BrightnessConfig) == 0x1984);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, StatusData) == 0x1A30);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, StatusEvent) == 0x1A38);
C_ASSERT(FIELD_OFFSET(SDP_PANEL_CONTEXT, Caps) == 0x1A50);
C_ASSERT(sizeof(SDP_PANEL_CONTEXT) == 0x1A58);

C_ASSERT(sizeof(SDP_CALIBRATION_BUFFER) == SDP_CAL_BUFFER_SIZE);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, InverseGamma) == 0x110);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, LinearGammaPayload) == 0xD18);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, LinearGamma) == 0x2528);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, GamutPayload) == 0x2550);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, Gamut) == 0x3678);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, Brightness) == 0x36C8);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, AlEnable) == 0x36F4);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, AlCalibration) == 0x36FC);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, Dithering) == 0x3718);
C_ASSERT(FIELD_OFFSET(SDP_CALIBRATION_BUFFER, DirtyFlags) == 0x3744);
C_ASSERT(sizeof(SDP_CAL_GAMUT_PAYLOAD) == SDP_CAL_SIZE_GAMUT);
C_ASSERT(sizeof(SDP_CAL_LINEAR_GAMMA_PAYLOAD) == SDP_CAL_SIZE_LIN_GAMMA);
C_ASSERT(sizeof(QDI_Panel_ColorCorrectionMatrixType) == SDP_CAL_SIZE_CCM);
C_ASSERT(sizeof(QDI_Panel_InverseGammaCorrectionTableType) == SDP_CAL_SIZE_INV_GAMMA);
C_ASSERT(sizeof(QDI_Display_PropertyConfigType) == 0xB8);*/

// ---------------------------------------------------------------------------
// Device context
// ---------------------------------------------------------------------------

typedef struct _DEVICE_CONTEXT
{
    WDFQUEUE DefaultQueue;                          // 0x0000
    SDP_PANEL_CONTEXT Panels[SDP_PANEL_COUNT];      // 0x0008
    ULONG State;                                    // 0x34B8 SDP_DEVICE_STATE_*
    ULONG PanelCount;                               // 0x34BC
    ULONG ResourceCount;                            // 0x34C0
    ULONG Reserved34C4;
    WDFTIMER EsdTimer;                              // 0x34C8
    BOOLEAN BrightnessOverride;                     // 0x34D0 IOCTL_SDP_SET_BRIGHTNESS_OVERRIDE
    UCHAR Reserved34D1[3];
    ULONG MfgModeCurrent;                           // 0x34D4 UEFI variable "MfgModeCurrent"
    ULONG ThermalPercent;                           // 0x34D8 brightness limit from the thermal interface
    ULONG Reserved34DC;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

/*C_ASSERT(sizeof(DEVICE_CONTEXT) == 0x34E0);
C_ASSERT(FIELD_OFFSET(DEVICE_CONTEXT, State) == 0x34B8);
C_ASSERT(FIELD_OFFSET(DEVICE_CONTEXT, EsdTimer) == 0x34C8);
C_ASSERT(FIELD_OFFSET(DEVICE_CONTEXT, MfgModeCurrent) == 0x34D4);*/

// Context of the I/O queues (the driver never stores anything in it).
typedef struct _QUEUE_CONTEXT
{
    PVOID Reserved;
} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

// Context of the work item that resets a panel after an ESD detection.
typedef struct _WORKITEM_CONTEXT
{
    ULONG DisplayId;
    ULONG Reserved;
} WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, SdpGetQueueContext)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT, SdpGetWorkItemContext)

// ---------------------------------------------------------------------------
// Thermal throttle interface (WdfDeviceAddQueryInterface)
// ---------------------------------------------------------------------------

// {ECBE47A8-C498-4BB9-BD70-E867E0940D22}
DEFINE_GUID(
    GUID_SDP_THERMAL_INTERFACE,
    0xecbe47a8, 0xc498, 0x4bb9, 0xbd, 0x70, 0xe8, 0x67, 0xe0, 0x94, 0x0d, 0x22);

// {DBE4373D-3C81-40CB-ACE4-E0E5D05F0C9F}
// Second device interface exposed by the device (used by the user mode service).
DEFINE_GUID(
    GUID_DEVINTERFACE_SDP_PANEL,
    0xdbe4373d, 0x3c81, 0x40cb, 0xac, 0xe4, 0xe0, 0xe5, 0xd0, 0x5f, 0x0c, 0x9f);

typedef VOID (*PFN_SDP_SET_TEMPERATURE)(_In_ WDFDEVICE Device, _In_ ULONG Temperature);

typedef struct _SDP_THERMAL_INTERFACE
{
    INTERFACE Interface;                // 0x00: Size 0x38, Version 1, Context = WDFDEVICE
    ULONG InterfaceVersion;             // 0x20: 2
    ULONG Reserved24;
    PVOID Reserved28;
    PFN_SDP_SET_TEMPERATURE SetTemperature;     // 0x30
} SDP_THERMAL_INTERFACE;

//C_ASSERT(sizeof(SDP_THERMAL_INTERFACE) == 0x38);

// ---------------------------------------------------------------------------
// ACPI _DSM (the MSHW0160 device reports the panel temperatures)
// ---------------------------------------------------------------------------

// {79554422-22B5-47C4-A3F0-9EA36FCE44E8}
DEFINE_GUID(
    GUID_SDP_ACPI_DSM,
    0x79554422, 0x22b5, 0x47c4, 0xa3, 0xf0, 0x9e, 0xa3, 0x6f, 0xce, 0x44, 0xe8);

// Resource hub path of a connection id.
#define SDP_RESOURCE_HUB_PREFIX  L"\\Device\\RESOURCE_HUB\\"

// "DPP" partition on the Surface storage (Basic Data type, name "DPP").
// {EBD0A0A2-B9E5-4433-87C0-68B6B72699C7}
typedef struct _SDP_PARTITION_QUERY
{
    const GUID* Type;
    PCWSTR Name;
} SDP_PARTITION_QUERY;

// UEFI variable "MfgModeCurrent" lives in this vendor namespace.
// {23896E64-07E6-4B23-A638-C966750EE3FF}
DEFINE_GUID(
    GUID_SDP_MFG_MODE_VARIABLE,
    0x23896e64, 0x07e6, 0x4b23, 0xa6, 0x38, 0xc9, 0x66, 0x75, 0x0e, 0xe3, 0xff);

// ---------------------------------------------------------------------------
// Constant tables (PanelTables.c)
// ---------------------------------------------------------------------------

//#define SDP_NITS_LUT_COUNT      255
#define SDP_THERMAL_ENTRY_COUNT 11

// Temperature range with the colour compensation of the panel.
typedef struct _SDP_THERMAL_ENTRY
{
    UCHAR ColorComp[10];
    UCHAR Reserved0A[2];
    LONG MinTemp;
    LONG MaxTemp;
    ULONG MaxNits;      // milli-nits
    ULONG Percent;
    ULONG Reserved1C;
} SDP_THERMAL_ENTRY;

C_ASSERT(sizeof(SDP_THERMAL_ENTRY) == 0x20);

typedef struct _SDP_THERMAL_STATE
{
    LONG LastTemperature;
    ULONG LastIndex;
} SDP_THERMAL_STATE;

extern const SDP_NITS_LUT_ENTRY g_NitsToDbvLut[SDP_NITS_LUT_COUNT];
extern const SDP_NITS_LUT_ENTRY g_SW3416_NitsToDbvLut[SDP_NITS_LUT_COUNT];
extern const SDP_THERMAL_ENTRY g_ThermalTable[SDP_THERMAL_ENTRY_COUNT];
extern const SHORT g_ThermalHysteresis;
extern SDP_THERMAL_STATE g_ThermalState[SDP_PANEL_COUNT];
extern ULONG g_ThrottleStep;
extern const ULONG g_ThrottleThresholdHigh;
extern const ULONG g_ThrottlePercentMid;
extern const ULONG g_ThrottleThresholdLow;
extern const ULONG g_ThrottlePercentLow;

// ---------------------------------------------------------------------------
// Function prototypes (split by module)
// ---------------------------------------------------------------------------

// Driver.c
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_UNLOAD OemPanelDriverOnDriverUnload;

// Device.c
EVT_WDF_DRIVER_DEVICE_ADD OemPanelDriverEvtDeviceAdd;
EVT_WDF_DEVICE_PREPARE_HARDWARE OemPanelEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE OemPanelEvtDeviceReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY OemPanelDriverEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT OemPanelDriverEvtDeviceD0Exit;
NTSTATUS InitializeDeviceContext(_In_ PDEVICE_CONTEXT Device, _In_ WDFDEVICE WdfDevice);
NTSTATUS CreateRefreshTimer(_In_ WDFDEVICE WdfDevice);
VOID PanelEngagePassiveCooling(_In_ WDFDEVICE Device, _In_ ULONG Temperature);
ULONG ConvertQDIDisplayIDtoDisplayIndex(_In_ ULONG DisplayId);

// ErrorLog.c
VOID Utility_EventLogEntryWrite(
    _In_ WDFDEVICE Device,
    _In_ NTSTATUS MessageCode,
    _In_ NTSTATUS Status,
    _In_ ULONG UniqueId,
    _In_ ULONG DumpDataSize,
    _In_opt_ PUCHAR DumpData,
    _In_ ULONG FormatCount,
    _In_opt_ PCWSTR* Formats,
    _In_ ULONG HasArguments,
    ...);

// Esd.c
EVT_WDF_TIMER EsdRefreshTimerFn;
EVT_WDF_WORKITEM ResetWorkItemCallBack;
VOID EsdRefreshTimerCallBack(_In_ WDFTIMER Timer);
VOID ForcePanelReInit(_In_ PPANEL_CONTEXT Panel);
NTSTATUS SendDisplayOnOffAction(_In_ PPANEL_CONTEXT Panel, _In_ BOOLEAN On);
NTSTATUS SendReInitalizeCommand(_In_ PPANEL_CONTEXT Panel);

// Queue.c
NTSTATUS OemPanelDriverQueueInitialize(_In_ WDFDEVICE Device);
EVT_WDF_IO_IN_CALLER_CONTEXT OnIoInCallerContext;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL OemPanelDriverEvtIoDeviceControl;
NTSTATUS OemDisplay_InitFunctionTable(_In_ PDEVICE_CONTEXT Device, _In_reads_bytes_(Length) PUCHAR Table, _In_ ULONG Length);
NTSTATUS Qdi_OEMInitFunctionTable(_In_ PDEVICE_CONTEXT Device, _In_reads_bytes_(Length) PUCHAR Table, _In_ ULONG Length);
NTSTATUS ExtractPanelIDFromColorRequest(_In_ PVOID Blob, _In_ SIZE_T Length, _Out_ PULONG PanelIndex);

// PanelIoctl.c
NTSTATUS HandleTools_Ioctl(
    _In_ PDEVICE_CONTEXT Device,
    _In_ ULONG IoControlCode,
    _In_ PSDP_IOCTL_HEADER Input,
    _In_ SIZE_T InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PUCHAR Output,
    _In_ SIZE_T OutputBufferLength,
    _Out_ PULONG BytesReturned);

// ActionQueue.c
NTSTATUS DisplayActionQueueInitalize(_Out_ SDP_ACTION_RING* Ring);
NTSTATUS DisplayActionQueueAddAction(_Inout_ SDP_ACTION_RING* Ring, _In_ const SDP_ACTION_NODE* Node);
NTSTATUS DisplayActionQueueAddActions(_Inout_ SDP_ACTION_RING* Ring, _In_reads_(Count) const SDP_ACTION_NODE* Nodes, _In_ ULONG Count);
NTSTATUS DisplayActionQueueWaitNewandGet(_Inout_ SDP_ACTION_RING* Ring, _Out_ SDP_ACTION_NODE* Node);
NTSTATUS AddDisplayActionRequestHelper(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Action, _In_opt_ PVOID Arg1, _In_opt_ PVOID Arg2, _In_ PKEVENT Completion);
NTSTATUS AddDisplayActionRequestSynchronous(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Action, _In_opt_ PVOID Arg1, _In_opt_ PVOID Arg2);
VOID BlockDisplayActionThread(_In_ SDP_ACTION_NODE* Node);
VOID UnBlockDisplayActionThread(_In_ SDP_ACTION_NODE* Node, _In_ LONG Status);
ULONGLONG UsFromBootPrecise(VOID);
VOID DelayMicroseconds(_In_ ULONG Microseconds);
VOID DelayMilliseconds(_In_ ULONG Milliseconds);

// ActionDispatch.c
NTSTATUS ProcessDisplayAction(_In_ PPANEL_CONTEXT Panel, _Inout_ SDP_ACTION_NODE* Node);
PCWSTR GetDisplayActionName(_In_ LONG Action);
PCWSTR GetQDIEventName(_In_ ULONG Event);

// Thread.c
NTSTATUS WorkerThreadCreate(_Inout_ SDP_THREAD* Thread, _In_ PVOID Context);
VOID WorkerThreadInitContext(_Out_ SDP_THREAD* Thread, _In_ BOOLEAN AutoReset);
VOID DestroyThread(_Inout_ SDP_THREAD* Thread);
NTSTATUS OemPanelThreadCreate(_In_ PDEVICE_CONTEXT Device);
VOID DisplayWorkerThread(_In_ PVOID Context);
VOID ColorWorkerThread(_In_ PVOID Context);

// QdiPanel.c
QDI_ErrorType OEMPanel_Init(_In_ QDI_HandleType Handle, _In_ QDI_Panel_AttrType* PanelConfig);
QDI_ErrorType OEMPanel_SetMode(_In_ QDI_HandleType Handle, _In_ QDI_Panel_AttrType* PanelConfig);
QDI_ErrorType OEMPanel_Term(_In_ QDI_HandleType Handle);
QDI_ErrorType OEMPanel_SetPower(_In_ QDI_HandleType Handle, _In_ bool32 PowerOn);
QDI_ErrorType OEMPanel_SetBacklight(_In_ QDI_HandleType Handle, _In_ uint32 Level);
QDI_ErrorType OEMPanel_HandleEvent(_In_ QDI_HandleType Handle, _In_ uint32 Events, _In_opt_ void* EventData, _In_ uint32 EventDataSize);
QDI_ErrorType OEMPanel_GetPropertyConfig(_In_ QDI_HandleType Handle, _In_ QDI_Display_PropertyConfigType* PropertyData);
QDI_ErrorType OemPanel_SetBrightnessNits(_In_ QDI_HandleType Handle, _In_ uint32 MilliNits, _In_ uint32 TransitionTimeMs);

// PanelConfig.c
NTSTATUS Qdi_OemGetPanelID(_Inout_ PPANEL_CONTEXT Panel, _In_ ULONG* PanelIdWord);
VOID Init_PanelActionTable(_Inout_ PPANEL_CONTEXT Panel);
NTSTATUS FillPanelConfiguration(_Out_ QDI_Panel_AttrType* Attr, _In_ const SDP_PANEL_INFO* Info, _In_ PPANEL_CONTEXT Panel);
NTSTATUS StartDisplayInitAction(_In_ PPANEL_CONTEXT Panel, _Out_ QDI_Panel_AttrType* Attr);
NTSTATUS FillMeta2PanelProperties(_In_ PPANEL_CONTEXT Panel);
VOID FillBrightnessSpecificSettings(_Out_ QDI_Panel_BrightnessConfigType* Config);
VOID FillApicalSettings(_Out_ SDP_AL_ENHANCEMENT_REQUEST* Request);

// PanelSequence.c
NTSTATUS AppendCommand(
    _Inout_ SDP_DSI_NODE* Head,
    _In_ ULONG Type,
    _In_ UCHAR PacketType,
    _In_ ULONG Flags,
    _In_reads_bytes_(DataSize) PUCHAR Data,
    _In_ ULONG DataSize,
    _In_ BOOLEAN Persistent);
NTSTATUS AppendDelay(_Inout_ SDP_DSI_NODE* Head, _In_ UCHAR Milliseconds, _In_ BOOLEAN Persistent);
NTSTATUS AppendDisplayCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent);
NTSTATUS AppendSleepCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent);
NTSTATUS AppendTECommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent);
NTSTATUS AppendTESetScanLineCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG Scanline, _In_ BOOLEAN Persistent);
VOID Free_DisplaySequence(_In_opt_ SDP_DSI_NODE* FirstNode);
NTSTATUS Free_PreAllocated_DisplaySequence(_Inout_ PPANEL_CONTEXT Panel);
NTSTATUS SendSequence(_In_ PPANEL_CONTEXT Panel, _Inout_ SDP_DSI_NODE* Head);
NTSTATUS ProcessSequence(
    _In_ PPANEL_CONTEXT Panel,
    _Inout_ SDP_DSI_NODE* Head,
    _Out_writes_bytes_(ReadBufferSize) PUCHAR ReadBuffer,
    _In_ SIZE_T ReadBufferSize);
NTSTATUS ProcessIoctlReadBuffer(_In_ PPANEL_CONTEXT Panel, _In_ SDP_DSI_READ_REQUEST* Request, _Out_ PUCHAR Output);
NTSTATUS ProcessIoctlWriteBuffer(_In_ PPANEL_CONTEXT Panel, _In_reads_bytes_(*Size) PUCHAR Data, _In_ PULONG Size);
VOID LogDSICommandString(_In_ UCHAR PacketType, _In_reads_bytes_opt_(Size) PUCHAR Data, _In_ ULONG Size);
VOID GetDSIError(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Status);
VOID PREDEFINED_DISPLAY_SEQ_INIT(_Inout_ SDP_DSI_NODE* Head);

// PanelSequences.c (sequence contents)
NTSTATUS FillMeta2Ev3PanelInitSequence(
    _Inout_ SDP_DSI_NODE* Head,
    _In_ ULONG DisplayId,
    _In_ ULONGLONG Caps,
    _In_opt_ PUCHAR PanelBin,
    _In_ ULONG PanelBinSize);
NTSTATUS FillMeta2Ev3RevSpecificPanelInitSequence(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DisplayId, _In_ ULONG PanelVersion);
NTSTATUS FillMetaPanelTermSeqeuence(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMetaDisplaySetBrightnessSequence(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMeta2PixelRegistrationValue(_Inout_ SDP_DSI_NODE* Head, _In_ UCHAR Value);
NTSTATUS FillMetaChangeTETimingSequence(_Inout_ SDP_DSI_NODE* Head, _In_ BOOLEAN Second);
NTSTATUS FillMetaEnableQSyncExtTrigSequence(_Inout_ SDP_DSI_NODE* Head, _In_ BOOLEAN Second);
NTSTATUS FillMetaDisplayGetBrightnessSequence(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMetaPanelGetId(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMetaGetGuid(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMetaPowerModeSequence(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS FillMetaSelfDiagnosticResultSequence(_Inout_ SDP_DSI_NODE* Head);
NTSTATUS SdpSeqBuildTemperatureCompensation(_Inout_ SDP_DSI_NODE* Head, _In_ const UCHAR* ColorCompensation);

// PanelOps.c (panel 0x3406 operations)
NTSTATUS Init_PreAllocated_META2Ev3_Sequence(_In_ PPANEL_CONTEXT Panel);
VOID Free_PreAllocated_META_Sequence(_In_ PPANEL_CONTEXT Panel);
NTSTATUS Get_Meta2_Mfg_Info(_In_ PPANEL_CONTEXT Panel, _Out_ ULONG* MfgInfo);
NTSTATUS Send_Meta2_Init_Sequence(_In_ PPANEL_CONTEXT Panel);
NTSTATUS Send_Meta_Terminate_Sequence(_In_ PPANEL_CONTEXT Panel);
BOOLEAN Validate_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _In_ ULONG MilliNits);
NTSTATUS GetDBV_Meta_BrightnessNits(_In_ PPANEL_CONTEXT Panel, _In_ ULONG MilliNits, _Out_ ULONG* Dbv);
NTSTATUS Set_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Dbv);
NTSTATUS Get_Meta_Brightness(_In_ PPANEL_CONTEXT Panel, _Out_ UCHAR* Value);
NTSTATUS Set_Meta2_SetPixelRegistration(_In_ PPANEL_CONTEXT Panel, _In_ UCHAR* Value);
NTSTATUS Get_Meta2_PowerMode(_In_ PPANEL_CONTEXT Panel, _Out_ CHAR* Mode, _Out_opt_ UCHAR* IsNormal);
NTSTATUS Set_Meta2_Throttle(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Temperature, _Inout_ SDP_TEMP_RESULT* Result);
NTSTATUS Get_Meta2_SelfDiagnostic_Info(_In_ PPANEL_CONTEXT Panel, _Out_ UCHAR* Value);
NTSTATUS Get_Meta2_MiscDiagnostic_Info(_In_ PPANEL_CONTEXT Panel, _In_ PPANEL_CONTEXT Other, _Out_ ULONG* Result);
VOID SetMeta2ManufacturingMode(_In_ PPANEL_CONTEXT Panel);
VOID Set_PassiveCooling_Meta2(_In_ PDEVICE_CONTEXT Device, _In_ ULONG Temperature, _Out_ UCHAR* Changed);

// PanelBin.c
NTSTATUS InitializePanelContext(_In_ PDEVICE_CONTEXT Device);
NTSTATUS GetGPTVolumePathAndSizeEx(
    _In_ WDFDEVICE Device,
    _In_ const SDP_PARTITION_QUERY* Query,
    _In_ ULONG PartitionIndexHint,
    _Out_writes_(0x104) PWCHAR Path,
    _Out_ PULONGLONG PartitionLength);
NTSTATUS ProcessColorPayload(_In_ PPANEL_CONTEXT Panel, _In_reads_bytes_(Length) PUCHAR Blob, _In_ SIZE_T Length);
NTSTATUS ValidateColorRequest(_In_ const SDP_CAL_HEADER* Header, _In_ SIZE_T Length);

// PanelTables.c
ULONG CalcCrc32(_In_reads_bytes_(Length) const UCHAR* Data, _In_ ULONG Length);

// Acpi.c
NTSTATUS EvaluateAcpiMethod(
    _In_ WDFDEVICE Device,
    _In_ ULONG Reserved,
    _In_reads_bytes_(sizeof(ULONG)) PULONG Input,
    _Outptr_result_maybenull_ PVOID* Output,
    _Out_opt_ PULONG OutputLength);
NTSTATUS PrepareInputParametersForDsmMethod(_In_ ULONG Function, _In_ ULONG Revision, _In_ ULONG ArgumentCount, _Outptr_ PVOID* Input, _Out_opt_ PULONG Length);
NTSTATUS IsDsmFunctionSupported(_In_ WDFDEVICE Device, _In_ ULONG Function, _In_ ULONG Revision, _Out_ PBOOLEAN Supported);
NTSTATUS GetAcpiDsmFuelGaugeTemperatures(_In_ WDFDEVICE Device, _In_ USHORT Reserved, _Out_writes_(2) LONG* Temperatures);

// Gpio.c
NTSTATUS ToggleResetLine(_Inout_ SDP_RESET_GPIO* Gpio, _In_ ULONG Mode);
NTSTATUS Gpio_PinSet(_In_ WDFIOTARGET Target, _In_ BOOLEAN High);
NTSTATUS RESOURCE_HUB_CREATE_PATH_FROM_ID(_Out_ PUNICODE_STRING Path, _In_ ULONG ConnectionIdLow, _In_ ULONG ConnectionIdHigh);

NTSTATUS InitializePanelRuntimeLut(_In_ PPANEL_CONTEXT Panel, BOOLEAN LeftPanel);

NTSTATUS
LoadQdcmCalibrationFromSfpd(
    WDFDEVICE Device,
    PCWSTR XmlPath,
    SDP_CALIBRATION_BUFFER* Calibration
);

NTSTATUS
ProcessQdcmCalibration(
    _In_ PPANEL_CONTEXT Panel,
    _In_ PCWSTR XmlPath
);