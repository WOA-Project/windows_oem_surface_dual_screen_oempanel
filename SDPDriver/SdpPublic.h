#pragma once

// Interface between SDPDriver and its user mode client (display service /
// manufacturing tools): the device interface GUID_DEVINTERFACE_OEM_PANEL_DRIVER
// ({0D4A3F63-0D08-49A1-B91C-C60576894174}, see oempanel.h) is opened and these
// IOCTLs are sent. The Graphics KMD uses the same interface with
// IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE.

#ifndef _SDP_PUBLIC_H_
#define _SDP_PUBLIC_H_

#define SDP_DEVICE_TYPE         0x8321
#define SDP_IOCTL(fn)           CTL_CODE(SDP_DEVICE_TYPE, (fn), METHOD_BUFFERED, FILE_ANY_ACCESS)

// Handled directly by OemPanelDriverEvtIoDeviceControl (request carries no SDP_IOCTL_HEADER).
#define IOCTL_SDP_LOAD_CALIBRATION          SDP_IOCTL(0xA01)    // input: SDP_CAL_HEADER blob
#define IOCTL_SDP_APPLY_THERMAL_LIMIT       SDP_IOCTL(0xA02)    // input: 8 bytes {display id, ...}
#define IOCTL_SDP_WAIT_PANEL_STATE          SDP_IOCTL(0xA03)    // input: 0x18 bytes, completes when the panel state changes

// Handled by HandleTools_Ioctl (request starts with an SDP_IOCTL_HEADER).
#define IOCTL_SDP_NOP                       SDP_IOCTL(0x801)
#define IOCTL_SDP_SET_BRIGHTNESS            SDP_IOCTL(0x802)    // payload: UCHAR level
#define IOCTL_SDP_SET_DISPLAY_STATE         SDP_IOCTL(0x803)    // payload: UCHAR 0 = on, 1 = off/reset
#define IOCTL_SDP_DSI_WRITE                 SDP_IOCTL(0x804)    // payload: UCHAR packet type, data...
#define IOCTL_SDP_DSI_READ                  SDP_IOCTL(0x805)    // payload: UCHAR packet type, data...
#define IOCTL_SDP_TOGGLE_RESET              SDP_IOCTL(0x806)
#define IOCTL_SDP_GET_BRIGHTNESS            SDP_IOCTL(0x807)    // output: 1 byte
#define IOCTL_SDP_GET_FRAMEBUFFER_INFO      SDP_IOCTL(0x809)    // output: SDP_FRAMEBUFFER_INFO
#define IOCTL_SDP_UPDATE_FRAMEBUFFER        SDP_IOCTL(0x80A)    // input: SDP_FRAMEBUFFER_UPDATE (locked in the caller context)
#define IOCTL_SDP_GET_CONTEXT_SIZE          SDP_IOCTL(0x80B)    // output: ULONG sizeof(DEVICE_CONTEXT)
#define IOCTL_SDP_GET_CONTEXT               SDP_IOCTL(0x80C)    // output: raw DEVICE_CONTEXT (debug)
#define IOCTL_SDP_MEASURE_VSYNC             SDP_IOCTL(0x80D)
#define IOCTL_SDP_GET_DISPLAY_PROPERTIES    SDP_IOCTL(0x80E)    // output: >= 0x58 bytes
#define IOCTL_SDP_NOP2                      SDP_IOCTL(0x80F)
#define IOCTL_SDP_DISPLAY_OFF               SDP_IOCTL(0x810)    // payload: UCHAR
#define IOCTL_SDP_GET_PANEL_PARAMS          SDP_IOCTL(0x811)    // output: 2 ULONGs
#define IOCTL_SDP_SET_PIXEL_REG             SDP_IOCTL(0x812)    // payload: UCHAR
#define IOCTL_SDP_GET_MFG_INFO              SDP_IOCTL(0x813)    // output: 0x1C bytes
#define IOCTL_SDP_SELF_TEST                 SDP_IOCTL(0x814)    // output: SDP_SELF_TEST_RESULT
#define IOCTL_SDP_SET_CALIBRATION           SDP_IOCTL(0x815)    // payload: SDP_CAL_HEADER blob
#define IOCTL_SDP_ESD_RESET                 SDP_IOCTL(0x816)
#define IOCTL_SDP_GET_VERSION               SDP_IOCTL(0x817)    // output: SDP_VERSION_INFO
#define IOCTL_SDP_GET_DSI_ERRORS            SDP_IOCTL(0x818)    // output: 3 ULONGs
#define IOCTL_SDP_GET_PANEL_STATUS          SDP_IOCTL(0x819)    // output: SDP_PANEL_STATUS
#define IOCTL_SDP_SET_NITS                  SDP_IOCTL(0x81A)    // payload: ULONG milli-nits
#define IOCTL_SDP_SET_AL_ENHANCEMENT        SDP_IOCTL(0x81B)    // payload: SDP_AL_ENHANCEMENT_REQUEST
#define IOCTL_SDP_SET_DITHERING             SDP_IOCTL(0x81C)    // payload: QDI_Panel_DitheringMatrixType (0x28 bytes)
#define IOCTL_SDP_SET_THERMAL_PERCENT       SDP_IOCTL(0x81D)    // payload: ULONG temperature / percent (<= 100)
#define IOCTL_SDP_SET_BRIGHTNESS_OVERRIDE   SDP_IOCTL(0x81E)    // payload: UCHAR enable, ULONG 1
#define IOCTL_SDP_SET_AMBIENT_LEVEL         SDP_IOCTL(0x81F)    // payload: ULONG

#pragma pack(push, 1)

// Header of the IOCTLs handled by HandleTools_Ioctl; the payload follows it.
typedef struct _SDP_IOCTL_HEADER
{
    ULONG DisplayId;            // QDI_DISPLAY_PRIMARY (1) / QDI_DISPLAY_SECONDARY (2)
    ULONG InputSize;            // payload bytes
    ULONG OutputSize;           // expected output bytes
    ULONG Reserved;
    UCHAR Payload[1];
} SDP_IOCTL_HEADER, *PSDP_IOCTL_HEADER;

typedef struct _SDP_FRAMEBUFFER_UPDATE
{
    USHORT Width;
    USHORT Height;
    USHORT Stride;              // bytes per row
    USHORT Reserved;
    PVOID Data;                 // user pointer, locked by OnIoInCallerContext
} SDP_FRAMEBUFFER_UPDATE;

typedef struct _SDP_VERSION_INFO
{
    ULONG Major;                // 2
    ULONG Minor;                // 9
    ULONG QdiPanelApiVersion;   // 0x02010056
} SDP_VERSION_INFO;

typedef struct _SDP_SELF_TEST_RESULT
{
    ULONG FailureMask;              // bit 0: any failure, bits 1..3: failed step
    ULONG Reserved;
    ULONG DualPanelSyncResult;
} SDP_SELF_TEST_RESULT;

// Output of IOCTL_SDP_GET_PANEL_STATUS (0xA4 bytes, packed).
typedef struct _SDP_PANEL_STATUS
{
    UCHAR PanelAsleep;
    ULONG Brightness;
    ULONG LastNitsScaled;
    ULONG ThermalPercent;
    ULONG ThermalMaxNits;
    UCHAR VsyncEnabled;
    ULONG VsyncPeriodUs;
    ULONG EsdCount;
    UCHAR DisplayOn;
    UCHAR HasPanelBin;
    UCHAR PanelBin[0x80];
    ULONG Temperature;
    ULONG ThermalIndex;
} SDP_PANEL_STATUS;

// Payload of IOCTL_SDP_MEASURE_VSYNC (0x20 bytes).
typedef struct _SDP_MEASURE_VSYNC_REQUEST
{
    ULONG Reserved[6];
    ULONG SampleCount;          // vsync periods averaged (minimum 5)
    ULONG DurationMs;           // measurement time (minimum 585 ms)
} SDP_MEASURE_VSYNC_REQUEST;

// Output of the 0xA03 wait IOCTL (0x18 bytes).
typedef struct _SDP_PANEL_STATE_NOTIFICATION
{
    ULONG Reserved0;
    ULONG PanelId;
    ULONG PanelVersion;
    ULONG State;                // 2 or 3
    UCHAR Data[8];
} SDP_PANEL_STATE_NOTIFICATION;

#pragma pack(pop)

#endif // _SDP_PUBLIC_H_
