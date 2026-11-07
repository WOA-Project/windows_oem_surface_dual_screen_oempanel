// ACPI _DSM interface of the MSHW0160 device: reports the panel temperatures.

#include "SdpDriver.h"
#include "Acpi.tmh"

#define SDP_DSM_METHOD_NAME     0x4D53445F  // "_DSM"
#define SDP_DSM_ARGUMENTS       4           // GUID, revision, function, package
#define SDP_DSM_FN_TEMPERATURE  1           // returns the temperatures of both panels
#define SDP_DECIKELVIN_ZERO_C   2732

// Evaluates an ACPI method through the device's I/O target with the
// IOCTL_ACPI_EVAL_METHOD input buffer in Input The output
// buffer is allocated here and must be freed by the caller.
NTSTATUS
EvaluateAcpiMethod(
    _In_ WDFDEVICE Device,
    _In_ ULONG Reserved,
    _In_reads_bytes_(sizeof(ULONG)) PULONG Input,
    _Outptr_result_maybenull_ PVOID* Output,
    _Out_opt_ PULONG OutputLength)
{
    UNREFERENCED_PARAMETER(Reserved);

    NTSTATUS status = STATUS_SUCCESS;
    PVOID outputBuffer = NULL;
    ULONG_PTR bytesReturned = 0;
    ULONG inputSize;

    switch (Input[0])
    {
        case ACPI_EVAL_INPUT_BUFFER_SIGNATURE:
            inputSize = sizeof(ACPI_EVAL_INPUT_BUFFER);
            break;
        case ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE:
            inputSize = Input[2];                       // Size member
            break;
        case ACPI_EVAL_INPUT_BUFFER_SIMPLE_INTEGER_SIGNATURE:
            inputSize = sizeof(ACPI_EVAL_INPUT_BUFFER_SIMPLE_INTEGER);
            break;
        case ACPI_EVAL_INPUT_BUFFER_SIMPLE_STRING_SIGNATURE:
            inputSize = Input[2] + 0xF;                 // StringLength + header
            break;
        default:
            status = STATUS_INVALID_PARAMETER_2;
            goto Done;
    }

    outputBuffer = ExAllocatePoolWithTag(NonPagedPoolNx, 0x200, SDP_POOL_TAG_ACPI);
    if (outputBuffer == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "Allocating the ACPI output buffer failed");
        // success and the caller finds a NULL buffer.
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Done;
    }

    WDF_MEMORY_DESCRIPTOR inputDescriptor;
    WDF_MEMORY_DESCRIPTOR outputDescriptor;
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputDescriptor, Input, inputSize);
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor, outputBuffer, 0x200);

    status = WdfIoTargetSendIoctlSynchronously(
        WdfDeviceGetIoTarget(Device),
        NULL,
        IOCTL_ACPI_EVAL_METHOD,
        &inputDescriptor,
        &outputDescriptor,
        NULL,
        &bytesReturned);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "IOCTL_ACPI_EVAL_METHOD failed %!STATUS!", status);
    }
    else if (bytesReturned == 0)
    {
        if (outputBuffer != NULL)
        {
            ExFreePoolWithTag(outputBuffer, SDP_POOL_TAG_ACPI);
        }
        outputBuffer = NULL;
        *Output = NULL;
    }
    else
    {
        *Output = outputBuffer;
    }

Done:
    if (OutputLength != NULL)
    {
        *OutputLength = (ULONG)bytesReturned;
    }
    if (!NT_SUCCESS(status) && outputBuffer != NULL)
    {
                ExFreePoolWithTag(outputBuffer, SDP_POOL_TAG_ACPI);
    }
    return status;
}

// Builds the complex ACPI input for _DSM(GUID, Revision, Function, Package)
// Free with SDP_POOL_TAG_ACPI.
NTSTATUS
PrepareInputParametersForDsmMethod(
    _In_ ULONG Function,
    _In_ ULONG Revision,
    _In_ ULONG ArgumentCount,
    _Outptr_ PVOID* Input,
    _Out_opt_ PULONG Length)
{
    ULONG size = (ArgumentCount - 1) * 8 + 0x24;

    PACPI_EVAL_INPUT_BUFFER_COMPLEX buffer =
        (PACPI_EVAL_INPUT_BUFFER_COMPLEX)ExAllocatePoolWithTag(NonPagedPoolNx, size, SDP_POOL_TAG_ACPI);
    if (buffer == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "Allocating the _DSM input failed");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, size);
    buffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    buffer->Size = size;
    buffer->ArgumentCount = ArgumentCount;
    buffer->MethodNameAsUlong = SDP_DSM_METHOD_NAME;

    // Arg0: the _DSM UUID.
    PACPI_METHOD_ARGUMENT argument = &buffer->Argument[0];
    argument->Type = ACPI_METHOD_ARGUMENT_BUFFER;
    argument->DataLength = sizeof(GUID);
    RtlCopyMemory(argument->Data, &GUID_SDP_ACPI_DSM, sizeof(GUID));

    // Arg1: revision, Arg2: function index, Arg3: empty package.
    argument = (PACPI_METHOD_ARGUMENT)((PUCHAR)argument + 4 + sizeof(GUID));
    argument->Type = ACPI_METHOD_ARGUMENT_INTEGER;
    argument->DataLength = sizeof(ULONG);
    argument->Argument = Revision;

    argument = (PACPI_METHOD_ARGUMENT)((PUCHAR)argument + 8);
    argument->Type = ACPI_METHOD_ARGUMENT_INTEGER;
    argument->DataLength = sizeof(ULONG);
    argument->Argument = Function;

    argument = (PACPI_METHOD_ARGUMENT)((PUCHAR)argument + 8);
    argument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
    argument->DataLength = sizeof(ULONG);
    argument->Argument = 0;

    *Input = buffer;
    if (Length != NULL)
    {
        *Length = size;
    }
    return STATUS_SUCCESS;
}

// _DSM function 0 returns a bit mask of the supported functions
NTSTATUS
IsDsmFunctionSupported(
    _In_ WDFDEVICE Device,
    _In_ ULONG Function,
    _In_ ULONG Revision,
    _Out_ PBOOLEAN Supported)
{
    PVOID input = NULL;
    PACPI_EVAL_OUTPUT_BUFFER output = NULL;
    ULONG outputLength = 0;

    *Supported = FALSE;

    NTSTATUS status = PrepareInputParametersForDsmMethod(0, Revision, SDP_DSM_ARGUMENTS, &input, NULL);
    if (NT_SUCCESS(status))
    {
        status = EvaluateAcpiMethod(Device, ((PULONG)input)[1], (PULONG)input, (PVOID*)&output, &outputLength);
    }

    if (NT_SUCCESS(status))
    {
        if (output == NULL || output->Argument[0].DataLength == 0 || outputLength < 0x14)
        {
                        status = STATUS_ACPI_FATAL;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "_DSM function 0 returned no data");
        }
        else if (output->Argument[0].DataLength < 5)
        {
            ULONG mask = 0;
            RtlCopyMemory(&mask, output->Argument[0].Data, output->Argument[0].DataLength);
            if ((mask & (1u << (Function & 0x1F))) != 0)
            {
                *Supported = TRUE;
            }
        }
        else
        {
            status = STATUS_ACPI_FATAL;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "_DSM function 0 returned too much data");
        }
    }

    if (output != NULL)
    {
        ExFreePoolWithTag(output, SDP_POOL_TAG_ACPI);
    }

    if (input != NULL)
    {
        ExFreePoolWithTag(input, SDP_POOL_TAG_ACPI);
    }
    
    return status;
}

// _DSM function 1: two temperatures in deci Kelvin, converted to Celsius
NTSTATUS
GetAcpiDsmFuelGaugeTemperatures(_In_ WDFDEVICE Device, _In_ USHORT Reserved, _Out_writes_(2) LONG* Temperatures)
{
    UNREFERENCED_PARAMETER(Reserved);

    PVOID input = NULL;
    PACPI_EVAL_OUTPUT_BUFFER output = NULL;
    ULONG outputLength = 0;
    BOOLEAN supported = FALSE;

    NTSTATUS status = IsDsmFunctionSupported(Device, SDP_DSM_FN_TEMPERATURE, 0, &supported);
    if (!NT_SUCCESS(status) || !supported)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "_DSM temperature function not available %!STATUS!", status);
    }
    else
    {
        status = PrepareInputParametersForDsmMethod(SDP_DSM_FN_TEMPERATURE, 0, SDP_DSM_ARGUMENTS, &input, NULL);
        if (NT_SUCCESS(status))
        {
            status = EvaluateAcpiMethod(Device, ((PULONG)input)[1], (PULONG)input, (PVOID*)&output, &outputLength);
        }

        if (NT_SUCCESS(status))
        {
            PACPI_METHOD_ARGUMENT argument = &output->Argument[0];

            if (output != NULL && argument->DataLength == 2 * sizeof(LONG) &&
                argument->Type == ACPI_METHOD_ARGUMENT_BUFFER)
            {
                RtlCopyMemory(Temperatures, argument->Data, 2 * sizeof(LONG));
                Temperatures[0] = (Temperatures[0] - SDP_DECIKELVIN_ZERO_C) / 10;
                Temperatures[1] = (Temperatures[1] - SDP_DECIKELVIN_ZERO_C) / 10;
            }
            else
            {
                status = STATUS_ACPI_FATAL;
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_ACPI, "Unexpected _DSM temperature data");
            }
        }
    }

    if (output != NULL)
    {
        ExFreePoolWithTag(output, SDP_POOL_TAG_ACPI);
    }

    if (input != NULL)
    {
        ExFreePoolWithTag(input, SDP_POOL_TAG_ACPI);
    }
    return status;
}
