/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SDPDriverQueueInitialize)
#endif

NTSTATUS
SDPDriverQueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:

     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    PQUEUE_CONTEXT queueContext;
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;

    PAGED_CODE();

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchSequential
        );

    queueConfig.EvtIoDeviceControl = SDPDriverEvtIoDeviceControl;

    queueConfig.DefaultQueue = TRUE;
    queueConfig.PowerManaged = WdfFalse;

    //
    // Fill in a callback for destroy, and our QUEUE_CONTEXT size
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    queueAttributes.SynchronizationScope = WdfSynchronizationScopeNone;
    queueAttributes.ExecutionLevel = WdfExecutionLevelPassive;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 &queueAttributes,
                 &queue
                 );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    //
    // Get our Driver Context memory from the returned Queue handle
    //
    queueContext = QueueGetContext(queue);

    if (queueContext != NULL)
    {
        //
        // Store the current device handle in the queue context, used
        // to reference extension structure.
        //
        queueContext->Device = Device;
    }

    return status;
}

NTSTATUS HandleIoCtlRequest(
    IN  PVOID       Device,
    IN  UINT32      IoControlCode,
    IN  PVOID       InputBuffer,
    IN  size_t      InputBufferSize,
    IN  PVOID       OutputBuffer,
    IN  size_t      OutputBufferSize,
    OUT size_t* BytesReturned)
{
    NTSTATUS status = STATUS_SUCCESS;
	
    //
    // Get pointer to Fdo extension
    //
    PDEVICE_CONTEXT pDevExt = DeviceGetContext(Device);

    //
    // OEM should first handle any IoControl defined by its driver and
    // to be processed by it
    //

    //
    // Then call the displayproxy IoControl to let displayproxy to handle
    // any IOCTL targetted for it
    //

    status = DisplayProxy_DeviceIoControl(pDevExt,
        IoControlCode,
        InputBuffer,
        InputBufferSize,
        OutputBuffer,
        OutputBufferSize,
        BytesReturned);

    return status;
}

VOID
SDPDriverEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code.

Return Value:

    VOID

--*/
{
    NTSTATUS 	status = STATUS_SUCCESS;
    PVOID  		inputBuffer = NULL;
    size_t		inputBufferActualSize = 0;
    PVOID  		outputBuffer = NULL;
    size_t		outputBufferActualSize = 0;
    size_t		bytesReturned = 0;
	
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
                Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);

    PQUEUE_CONTEXT queueContext = QueueGetContext(Queue);

    // 
    // Cannot continue of queue context is not valid.
    //
    if (NULL == queueContext)
    {
        WdfRequestComplete(Request, STATUS_INVALID_HANDLE);
    }
    else
    {
        // Get the input buffer
        if (InputBufferLength)
        {
            status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &inputBuffer, &inputBufferActualSize);
            if (!NT_SUCCESS(status))
            {
                WdfRequestComplete(Request, status);
                return;
            }
        }

        // Get the output buffer
        if (OutputBufferLength)
        {
            status = WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &outputBuffer, &outputBufferActualSize);
            if (!NT_SUCCESS(status))
            {
                WdfRequestComplete(Request, status);
                return;
            }
        }

        //
        // Handle IOCTL request
        //
        status = HandleIoCtlRequest(queueContext->Device, IoControlCode, inputBuffer, inputBufferActualSize, outputBuffer, outputBufferActualSize, &bytesReturned);
    }

    //
    // Return IOCTL completion status
    //
    WdfRequestCompleteWithInformation(Request, status, bytesReturned);

    return;
}
