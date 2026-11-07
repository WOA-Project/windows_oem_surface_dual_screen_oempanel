#pragma once

// Tracing for SDPDriver: classic WPP (auto-logger / WppRecorder flavour, the
// shipped binary imports WppAutoLogStart/Stop/Trace from WPPRECORDER.SYS) plus
// a TraceLogging provider named "Microsoft.SDPDriver" used for the display
// action / QDI event telemetry.
//
// Every source file includes SdpDriver.h (which includes this header) and then
// its own generated "<file>.tmh".

#include <TraceLoggingProvider.h>

// Control GUID: {BB72F504-2CC1-4F46-9DCB-EE575A6F71B6}
//
// The flag numbers match the bit indices tested by the shipped binary
// (flag N in the binary == bit N-1 here). The binary only uses levels 2..5
// (error, warning, information, verbose).
#define WPP_CONTROL_GUIDS                                                              \
    WPP_DEFINE_CONTROL_GUID(                                                           \
        SdpDriverTraceGuid, (BB72F504, 2CC1, 4F46, 9DCB, EE575A6F71B6),                \
        WPP_DEFINE_BIT(TRACE_DRIVER)      /* 1: driver/device add, PnP, D0 */          \
        WPP_DEFINE_BIT(TRACE_QUEUE)       /* 2: I/O queue and IOCTL dispatch */        \
        WPP_DEFINE_BIT(TRACE_INTERFACE)   /* 3: QDI interface table, panel id */       \
        WPP_DEFINE_BIT(TRACE_QDI)         /* 4: QDI panel callbacks */                 \
        WPP_DEFINE_BIT(TRACE_GPIO)        /* 5: reset GPIO through the resource hub */ \
        WPP_DEFINE_BIT(TRACE_PANEL)       /* 6: DSI sequences and panel operations */  \
        WPP_DEFINE_BIT(TRACE_IOCTL)       /* 7: panel IOCTL handlers */                \
        WPP_DEFINE_BIT(TRACE_RESERVED8)   /* 8: unused */                              \
        WPP_DEFINE_BIT(TRACE_THREAD)      /* 9: display worker and status threads */   \
        WPP_DEFINE_BIT(TRACE_CALIBRATION) /* 10: calibration blob parser */            \
        WPP_DEFINE_BIT(TRACE_ACPI)        /* 11: ACPI _DSM / partition lookup */       \
        )

#define WPP_FLAG_LEVEL_LOGGER(flag, level) \
    WPP_LEVEL_LOGGER(flag)

#define WPP_FLAG_LEVEL_ENABLED(flag, level) \
    (WPP_LEVEL_ENABLED(flag) && WPP_CONTROL(WPP_BIT_ ## flag).Level >= level)

#define WPP_LEVEL_FLAGS_LOGGER(lvl, flags) \
    WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
    (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// begin_wpp config
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp

TRACELOGGING_DECLARE_PROVIDER(g_SdpTraceLoggingProvider);

// TraceLogging levels (WINEVENT_LEVEL_xxx).
#define SDP_TL_LEVEL_ERROR      2
#define SDP_TL_LEVEL_WARNING    3
#define SDP_TL_LEVEL_INFO       4
#define SDP_TL_LEVEL_VERBOSE    5

// TraceLogging keywords used by the driver. Everything else is keyword 0.
#define SDP_KEYWORD_PERF        0x80ull    // QDIOEMxxx Start/End timing events
#define SDP_KEYWORD_THERMAL     0x81ull    // TEMPERATURE_COLOR_COMPENSATION
