// Reset GPIO of the panels, driven through the ACPI resource hub.

#include "SdpDriver.h"
#include "Gpio.tmh"

// "\Device\RESOURCE_HUB\<16 hex digits of the connection id>"
NTSTATUS
RESOURCE_HUB_CREATE_PATH_FROM_ID(_Out_ PUNICODE_STRING Path, _In_ ULONG ConnectionIdLow, _In_ ULONG ConnectionIdHigh)
{
    WCHAR idText[17];
    ULONGLONG connectionId = ((ULONGLONG)ConnectionIdHigh << 32) | ConnectionIdLow;

    NTSTATUS status = RtlStringCbPrintfW(idText, sizeof(idText), L"%0*I64x", 0x10, connectionId);
    if (NT_SUCCESS(status))
    {
        status = RtlUnicodeStringPrintf(Path, L"%s%s", SDP_RESOURCE_HUB_PREFIX, idText);
    }
    return status;
}

// Writes one pin value with IOCTL_GPIO_WRITE_PINS
NTSTATUS
Gpio_PinSet(_In_ WDFIOTARGET Target, _In_ BOOLEAN High)
{
    NTSTATUS status;
    WDFREQUEST request = NULL;
    WDFMEMORY memory = NULL;
    UCHAR value = High ? 1 : 0;

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = Target;

    status = WdfRequestCreate(&attributes, Target, &request);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_GPIO, "WdfRequestCreate failed %!STATUS!", status);
        return status;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = request;

    status = WdfMemoryCreatePreallocated(&attributes, &value, sizeof(value), &memory);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_GPIO, "WdfMemoryCreatePreallocated failed %!STATUS!", status);
    }
    else
    {
        status = WdfIoTargetFormatRequestForIoctl(Target, request, IOCTL_GPIO_WRITE_PINS, memory, NULL, memory, NULL);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_GPIO, "WdfIoTargetFormatRequestForIoctl failed %!STATUS!", status);
        }
        else
        {
            WDF_REQUEST_SEND_OPTIONS sendOptions;
            WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

            BOOLEAN sent = WdfRequestSend(request, Target, &sendOptions);
            status = WdfRequestGetStatus(request);
            if (!sent || !NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_GPIO, "GPIO write failed %!STATUS!", status);
            }
        }
    }

    WdfObjectDelete(request);
    return status;
}

// Drives the reset line. Without explicit
// levels the line is pulsed 0 -> 1; otherwise Mode selects the level
// change. The post delay is always applied.
NTSTATUS
ToggleResetLine(_Inout_ SDP_RESET_GPIO* Gpio, _In_ ULONG Mode)
{
    if (Gpio == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (!Gpio->ExplicitLevels)
    {
        if (Gpio->PreDelayUs != 0)
        {
            DelayMicroseconds(Gpio->PreDelayUs);
        }

        Gpio_PinSet(Gpio->Target, FALSE);

        if (Gpio->HoldUs == 0)
        {
            Gpio->HoldUs = 1;
        }
        DelayMicroseconds(Gpio->HoldUs);

        Gpio_PinSet(Gpio->Target, TRUE);
    }
    else
    {
        BOOLEAN inactive = (Gpio->ActiveHigh == FALSE);

        if (Mode == SDP_RESET_GPIO_DEASSERT)
        {
            Gpio_PinSet(Gpio->Target, inactive);
        }
        else if (Mode == SDP_RESET_GPIO_ASSERT)
        {
            Gpio_PinSet(Gpio->Target, !inactive);
        }
        else if (Mode == SDP_RESET_GPIO_PULSE)
        {
            Gpio_PinSet(Gpio->Target, inactive);
            DelayMicroseconds(Gpio->PreDelayUs);
            Gpio_PinSet(Gpio->Target, !inactive);
            DelayMicroseconds(Gpio->HoldUs);
            Gpio_PinSet(Gpio->Target, inactive);
        }
    }

    if (Gpio->PostDelayUs != 0)
    {
        DelayMicroseconds(Gpio->PostDelayUs);
    }
    return STATUS_SUCCESS;
}
