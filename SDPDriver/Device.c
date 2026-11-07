// Device add, PnP / power callbacks, panel contexts and the thermal
// throttle interface.

#include "SdpDriver.h"
#include "Device.tmh"

// Maps QDI_DISPLAY_PRIMARY/SECONDARY to the panel slot
ULONG
ConvertQDIDisplayIDtoDisplayIndex(_In_ ULONG DisplayId)
{
    if (DisplayId == QDI_DISPLAY_PRIMARY)
    {
        return 0;
    }
    if (DisplayId == QDI_DISPLAY_SECONDARY)
    {
        return 1;
    }
    return SDP_INVALID_PANEL_INDEX;
}

// Allocates the per-panel synchronization objects and calibration memory
NTSTATUS
InitializeDeviceContext(_In_ PDEVICE_CONTEXT Device, _In_ WDFDEVICE WdfDevice)
{
    NTSTATUS status = STATUS_SUCCESS;

    Device->State = SDP_DEVICE_STATE_NONE;
    Device->PanelCount = SDP_PANEL_COUNT;
    Device->BrightnessOverride = FALSE;
    Device->ThermalPercent = 100;

    for (ULONG i = 0; i < Device->PanelCount; i++)
    {
        PPANEL_CONTEXT panel = &Device->Panels[i];

        panel->Device = WdfDevice;
        KeInitializeEvent(&panel->PanelOffEvent, SynchronizationEvent, FALSE);
        KeInitializeEvent(&panel->StatusEvent, SynchronizationEvent, FALSE);
        KeInitializeGuardedMutex(&panel->Lock);

        WDF_OBJECT_ATTRIBUTES attributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = WdfDevice;

        WDFMEMORY memory;
        status = WdfMemoryCreate(
            &attributes, NonPagedPool, SDP_POOL_TAG, SDP_CAL_BUFFER_SIZE, &memory, (PVOID*)&panel->Calibration);
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    return status;
}

// 10 ms after creation the timer is re-armed with SDP_ESD_POLL_INTERVAL_MS
// by EsdRefreshTimerFn
NTSTATUS
CreateRefreshTimer(_In_ WDFDEVICE WdfDevice)
{
    PDEVICE_CONTEXT device = DeviceGetContext(WdfDevice);

    WDF_TIMER_CONFIG timerConfig;
    WDF_TIMER_CONFIG_INIT(&timerConfig, EsdRefreshTimerFn);
    timerConfig.AutomaticSerialization = TRUE;

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ExecutionLevel = WdfExecutionLevelPassive;
    attributes.ParentObject = WdfDevice;

    NTSTATUS status = WdfTimerCreate(&timerConfig, &attributes, &device->EsdTimer);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfTimerCreate (ESD) failed %!STATUS!", status);
    }
    return status;
}

// Called by the thermal coordinator through the query interface
// Recomputes the throttle level and, when it changed, re-applies the
// brightness of both panels.
VOID
PanelEngagePassiveCooling(_In_ WDFDEVICE Device, _In_ ULONG Temperature)
{
    PDEVICE_CONTEXT device = DeviceGetContext(Device);
    UCHAR changed = FALSE;
    UCHAR useAllPanels = TRUE;
    ULONG request[2] = {0, 0};

    if ((device->Panels[0].Caps & SDP_CAPS_NO_THERMAL_THROTTLE) != 0)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Thermal throttling disabled by the capability mask");
        return;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Temperature %d reported", Temperature);

    if (device->Panels[0].Ops.ThermalThrottle != NULL)
    {
        device->Panels[0].Ops.ThermalThrottle(device, Temperature, &changed);
    }

    if (changed)
    {
        // Lower of the last brightness requested on both panels.
        request[0] = min(device->Panels[0].LastNitsScaled, device->Panels[1].LastNitsScaled);

        NTSTATUS status = AddDisplayActionRequestSynchronous(
            &device->Panels[0], DisplayActionSetBrightnessNits | SDP_ACTION_FLAG_USER, request, &useAllPanels);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Applying the thermal brightness failed %!STATUS!", status);
        }
    }
}

// PnP: the resource hub connections (two GPIOs) are the only hardware
// resources
NTSTATUS
OemPanelEvtDevicePrepareHardware(
    _In_ WDFDEVICE Device,
    _In_ WDFCMRESLIST ResourcesRaw,
    _In_ WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesRaw);

    PDEVICE_CONTEXT device = DeviceGetContext(Device);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG count = WdfCmResourceListGetCount(ResourcesTranslated);

    for (ULONG i = 0; i < count; i++)
    {
        PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor = WdfCmResourceListGetDescriptor(ResourcesTranslated, i);
        if (descriptor == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfCmResourceListGetDescriptor(%u) returned NULL", i);
            goto Done;
        }

        if (descriptor->Type != CmResourceTypeConnection ||
            descriptor->u.Connection.Class != CM_RESOURCE_CONNECTION_CLASS_GPIO ||
            descriptor->u.Connection.Type != CM_RESOURCE_CONNECTION_TYPE_GPIO_IO)
        {
            continue;
        }

        WCHAR pathBuffer[0x98 / sizeof(WCHAR)];
        UNICODE_STRING path;
        RtlInitEmptyUnicodeString(&path, pathBuffer, sizeof(pathBuffer));

        status = RESOURCE_HUB_CREATE_PATH_FROM_ID(
            &path, descriptor->u.Connection.IdLowPart, descriptor->u.Connection.IdHighPart);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Building the resource hub path failed %!STATUS!", status);
            goto Done;
        }

        WDF_OBJECT_ATTRIBUTES attributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = Device;

        WDFIOTARGET target;
        status = WdfIoTargetCreate(Device, &attributes, &target);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfIoTargetCreate failed %!STATUS!", status);
            goto Done;
        }

        WDF_IO_TARGET_OPEN_PARAMS openParams;
        WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(&openParams, &path, GENERIC_WRITE);
        status = WdfIoTargetOpen(target, &openParams);
        if (!NT_SUCCESS(status))
        {
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfIoTargetOpen failed (resource %u) %!STATUS!", i, status);
            goto Done;
        }

        if (i == 0)
        {
            device->Panels[0].ResetGpio.Target = target;
        }
        else if (i == 1)
        {
            device->Panels[1].ResetGpio.Target = target;
        }
        else
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Unexpected GPIO resource index %u", i);
            status = STATUS_INVALID_PARAMETER;
            goto Done;
        }
    }

    if (device->Panels[0].ResetGpio.Target == NULL || device->Panels[1].ResetGpio.Target == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "The reset GPIO resources are missing");
        status = STATUS_UNSUCCESSFUL;
    }

Done:
    device->ResourceCount = count;
    if (!NT_SUCCESS(status))
    {
        Utility_EventLogEntryWrite(Device, SDP_MSG_PREPARE_HARDWARE_FAILED, status, 0, 0, NULL, 0, NULL, 0);
    }
    return status;
}
NTSTATUS
OemPanelEvtDeviceReleaseHardware(_In_ WDFDEVICE Device, _In_ WDFCMRESLIST ResourcesTranslated)
{
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PDEVICE_CONTEXT device = DeviceGetContext(Device);
    WDFIOTARGET target = NULL;

    for (ULONG i = 0; i < device->ResourceCount; i++)
    {
        if (i == 0)
        {
            target = device->Panels[0].ResetGpio.Target;
            device->Panels[0].ResetGpio.Target = NULL;
        }
        else if (i == 1)
        {
            target = device->Panels[1].ResetGpio.Target;
            device->Panels[1].ResetGpio.Target = NULL;
        }

        if (target != NULL)
        {
            WdfIoTargetClose(target);
            WdfObjectDelete(target);
            target = NULL;
        }
    }

    return STATUS_SUCCESS;
}

// D0 entry (re)loads the panel bin files and stops the ESD timer
NTSTATUS
OemPanelDriverEvtDeviceD0Entry(_In_ WDFDEVICE Device, _In_ WDF_POWER_DEVICE_STATE PreviousState)
{
    UNREFERENCED_PARAMETER(PreviousState);

    PDEVICE_CONTEXT device = DeviceGetContext(Device);

    NTSTATUS status = InitializePanelContext(device);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Loading the panel bin files failed %!STATUS!", status);
    }
    else
    {
        WdfTimerStop(device->EsdTimer, FALSE);
    }
    return status;
}

// D0 exit waits until both panels were powered off by the display worker
NTSTATUS
OemPanelDriverEvtDeviceD0Exit(_In_ WDFDEVICE Device, _In_ WDF_POWER_DEVICE_STATE TargetState)
{
    UNREFERENCED_PARAMETER(TargetState);

    PDEVICE_CONTEXT device = DeviceGetContext(Device);
    NTSTATUS status = STATUS_SUCCESS;

    PVOID events[SDP_PANEL_COUNT];
    events[0] = &device->Panels[0].PanelOffEvent;
    events[1] = &device->Panels[1].PanelOffEvent;

    NTSTATUS waitStatus = KeWaitForMultipleObjects(2, events, WaitAll, Executive, KernelMode, FALSE, NULL, NULL);
    if (waitStatus != STATUS_WAIT_0)
    {
        status = STATUS_UNSUCCESSFUL;
    }
    return status;
}

// Creates the device, its queues, display threads, timer and interfaces
NTSTATUS
OemPanelDriverEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit)
{
    UNREFERENCED_PARAMETER(Driver);

    WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
    pnpCallbacks.EvtDeviceD0Entry = OemPanelDriverEvtDeviceD0Entry;
    pnpCallbacks.EvtDeviceD0Exit = OemPanelDriverEvtDeviceD0Exit;
    pnpCallbacks.EvtDevicePrepareHardware = OemPanelEvtDevicePrepareHardware;
    pnpCallbacks.EvtDeviceReleaseHardware = OemPanelEvtDeviceReleaseHardware;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpCallbacks);
    WdfDeviceInitSetIoInCallerContextCallback(DeviceInit, OnIoInCallerContext);

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);

    WDFDEVICE device;
    NTSTATUS status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceCreate failed %!STATUS!", status);
        return status;
    }

    PDEVICE_CONTEXT context = DeviceGetContext(device);

    status = InitializeDeviceContext(context, device);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "InitializeDeviceContext failed %!STATUS!", status);
        return status;
    }

    status = WdfDeviceCreateDeviceInterface(device, &GUID_DEVINTERFACE_OEM_PANEL_DRIVER, NULL);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceCreateDeviceInterface (OEM panel) failed %!STATUS!", status);
        return status;
    }

    status = OemPanelDriverQueueInitialize(device);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "OemPanelDriverQueueInitialize failed %!STATUS!", status);
        return status;
    }

    status = OemPanelThreadCreate(context);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "OemPanelThreadCreate failed %!STATUS!", status);
        return status;
    }

    // A failing timer is only traced, the device keeps working without ESD polling.
    NTSTATUS timerStatus = CreateRefreshTimer(device);
    if (!NT_SUCCESS(timerStatus))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "CreateRefreshTimer failed %!STATUS!", timerStatus);
    }

    SDP_THERMAL_INTERFACE thermal;
    RtlZeroMemory(&thermal, sizeof(thermal));
    thermal.Interface.Size = sizeof(thermal);
    thermal.Interface.Version = 1;
    thermal.Interface.Context = device;
    thermal.Interface.InterfaceReference = WdfDeviceInterfaceDereferenceNoOp;
    thermal.Interface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
    thermal.InterfaceVersion = 2;
    thermal.SetTemperature = PanelEngagePassiveCooling;

    WDF_QUERY_INTERFACE_CONFIG queryInterface;
    WDF_QUERY_INTERFACE_CONFIG_INIT(&queryInterface, &thermal.Interface, &GUID_SDP_THERMAL_INTERFACE, NULL);
    status = WdfDeviceAddQueryInterface(device, &queryInterface);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceAddQueryInterface failed %!STATUS!", status);
        return status;
    }

    status = WdfDeviceCreateDeviceInterface(device, &GUID_DEVINTERFACE_SDP_PANEL, NULL);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDeviceCreateDeviceInterface (SDP panel) failed %!STATUS!", status);
        return status;
    }

    // Manufacturing mode flag from the firmware (1 byte). The binary returns
    // this status, so a missing variable fails the device add.
    UNICODE_STRING variableName;
    RtlInitUnicodeString(&variableName, L"MfgModeCurrent");
    ULONG valueLength = 1;
    GUID vendorGuid = GUID_SDP_MFG_MODE_VARIABLE;
    status = ExGetFirmwareEnvironmentVariable(&variableName, &vendorGuid, &context->MfgModeCurrent, &valueLength, NULL);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "ExGetFirmwareEnvironmentVariable(MfgModeCurrent) failed %!STATUS!", status);

        // Workaround for lack of variable BEGIN
        context->MfgModeCurrent = 0;
        status = STATUS_SUCCESS;
        // Workaround for lack of variable END
    }
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "MfgModeCurrent = %u", context->MfgModeCurrent);
    }

    return status;
}
