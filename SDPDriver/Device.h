/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

#include "qdi_types.h"
#include "qdi_oem.h"
#include "oempanel.h"

/*
* Rob Green, a member of the NTDEV list, provides the
* following set of macros that'll keep you from having
* to scratch your head and count zeros ever again.
* Using these defintions, all you'll have to do is write:
*
* interval.QuadPart = RELATIVE(SECONDS(5));
*/

#ifndef ABSOLUTE
#define ABSOLUTE(wait) (wait)
#endif

#ifndef RELATIVE
#define RELATIVE(wait) (-(wait))
#endif

#ifndef NANOSECONDS
#define NANOSECONDS(nanos) \
	(((signed __int64)(nanos)) / 100L)
#endif

#ifndef MICROSECONDS
#define MICROSECONDS(micros) \
	(((signed __int64)(micros)) * NANOSECONDS(1000L))
#endif

#ifndef MILLISECONDS
#define MILLISECONDS(milli) \
	(((signed __int64)(milli)) * MICROSECONDS(1000L))
#endif

#ifndef SECONDS
#define SECONDS(seconds) \
	(((signed __int64)(seconds)) * MILLISECONDS(1000L))
#endif

typedef struct _PANEL_DEVICE_CONTEXT
{
    PVOID DeviceContext;
    QDI_Display_IDType eDisplayId;
} PANEL_DEVICE_CONTEXT, * PPANEL_DEVICE_CONTEXT;

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    WDFDEVICE Device;
    PANEL_DEVICE_CONTEXT PrimaryPanel;
    PANEL_DEVICE_CONTEXT SecondaryPanel;
    QDI_DisplayUtil_FunctionTableType sDispUtilsFunctionTable;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
SDPDriverCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

NTSTATUS DisplayProxy_DeviceIoControl(IN PDEVICE_CONTEXT hDisplayProxyHandle,
    IN  UINT32              IoControlCode,
    IN  PVOID               InputBuffer,
    IN  size_t              InputBufferSize,
    IN  PVOID               OutputBuffer,
    IN  size_t              OutputBufferSize,
    OUT size_t* BytesReturned);