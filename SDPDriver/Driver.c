// Driver entry and driver level callbacks.

#include "SdpDriver.h"
#include "Driver.tmh"

// {6F37FEB5-BD4B-4158-B819-BCF421B2FDB1}
TRACELOGGING_DEFINE_PROVIDER(
    g_SdpTraceLoggingProvider,
    "Microsoft.SDPDriver",
    (0x6f37feb5, 0xbd4b, 0x4158, 0xb8, 0x19, 0xbc, 0xf4, 0x21, 0xb2, 0xfd, 0xb1));

// Real driver entry The binary's "entry" (140018e60) is the
// GS cookie initialisation + FxDriverEntry (KMDF boilerplate) that calls it.
NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TraceLoggingRegister(g_SdpTraceLoggingProvider);

    WDF_DRIVER_CONFIG config;
    WDF_DRIVER_CONFIG_INIT(&config, OemPanelDriverEvtDeviceAdd);
    config.EvtDriverUnload = OemPanelDriverOnDriverUnload;
    config.DriverPoolTag = SDP_POOL_TAG;

    NTSTATUS status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
    }
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Driver created, DriverObject %p", DriverObject);
    }

    return status;
}

// Stops both display threads of every panel before the driver goes away
// Note: the binary registers this as OemPanelDriverOnDriverUnload but
// fetches the DEVICE_CONTEXT from the WDFDRIVER handle it receives.
VOID
OemPanelDriverOnDriverUnload(_In_ WDFDRIVER Driver)
{
    PDEVICE_CONTEXT device = DeviceGetContext((WDFOBJECT)Driver);

    for (ULONG i = 0; i < device->PanelCount; i++)
    {
        PPANEL_CONTEXT panel = &device->Panels[i];

        // Ask the worker to leave its loop and wait for it.
        AddDisplayActionRequestSynchronous(panel, DisplayActionExitThread, NULL, NULL);
        DestroyThread(&panel->ActionThread);

        WdfIoQueuePurge(panel->StatusQueue, NULL, NULL);
        KeSetEvent(&panel->StatusThread.StopEvent, 0, FALSE);
        DestroyThread(&panel->StatusThread);
    }

    PDRIVER_OBJECT driverObject = WdfDriverWdmGetDriverObject(Driver);
    WPP_CLEANUP(driverObject);
    TraceLoggingUnregister(g_SdpTraceLoggingProvider);
}
