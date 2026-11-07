// Per-panel action queue. Every panel operation is executed on the panel's
// worker thread; callers either wait for the result (AddDisplayActionRequestSynchronous) or only
// queue the work.

#include "SdpDriver.h"
#include "ActionQueue.tmh"

// Initialises the ring
NTSTATUS
DisplayActionQueueInitalize(_Out_ SDP_ACTION_RING* Ring)
{
    KeInitializeGuardedMutex(&Ring->Lock);
    KeInitializeSemaphore(&Ring->ItemsAvailable, 0, SDP_ACTION_QUEUE_DEPTH);
    Ring->ReadIndex = 0;
    Ring->WriteIndex = 0;
    return STATUS_SUCCESS;
}

// Appends one node. The caller holds the ring lock
NTSTATUS
DisplayActionQueueAddAction(_Inout_ SDP_ACTION_RING* Ring, _In_ const SDP_ACTION_NODE* Node)
{
    if (Ring == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    BOOLEAN full = (Ring->WriteIndex == Ring->ReadIndex - 1) ||
                   (Ring->WriteIndex == SDP_ACTION_QUEUE_DEPTH - 1 && Ring->ReadIndex == 0);
    if (full)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Action queue is full, dropping action 0x%x", Node->Action);

        TraceLoggingWrite(
            g_SdpTraceLoggingProvider,
            "DisplayActionQueueAddActionFail",
            TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
            TraceLoggingHexUInt32(Node->Action, "DisplayAction"));

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(&Ring->Nodes[Ring->WriteIndex], Node, sizeof(SDP_ACTION_NODE));
    Ring->WriteIndex++;
    if (Ring->WriteIndex == SDP_ACTION_QUEUE_DEPTH)
    {
        Ring->WriteIndex = 0;
    }
    return STATUS_SUCCESS;
}

// Appends Count nodes under the ring lock and wakes the worker for each of
// them
NTSTATUS
DisplayActionQueueAddActions(_Inout_ SDP_ACTION_RING* Ring, _In_reads_(Count) const SDP_ACTION_NODE* Nodes, _In_ ULONG Count)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Count == 0 || Ring == NULL)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "DisplayActionQueueAddActions: invalid parameter");
        return STATUS_INVALID_PARAMETER;
    }

    KeAcquireGuardedMutex(&Ring->Lock);
    for (ULONG i = 0; i < Count; i++)
    {
        status = DisplayActionQueueAddAction(Ring, &Nodes[i]);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "DisplayActionQueueAddAction failed %!STATUS!", status);
            break;
        }
        KeReleaseSemaphore(&Ring->ItemsAvailable, 0, 1, FALSE);
    }
    KeReleaseGuardedMutex(&Ring->Lock);

    return status;
}

// Waits for the semaphore and pops the oldest node
NTSTATUS
DisplayActionQueueWaitNewandGet(_Inout_ SDP_ACTION_RING* Ring, _Out_ SDP_ACTION_NODE* Node)
{
    BOOLEAN locked = FALSE;

    NTSTATUS status = KeWaitForSingleObject(&Ring->ItemsAvailable, Executive, KernelMode, FALSE, NULL);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Waiting for the action semaphore failed %!STATUS!", status);
    }
    else
    {
        KeAcquireGuardedMutex(&Ring->Lock);
        locked = TRUE;

        if (Ring->WriteIndex == Ring->ReadIndex)
        {
            TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "Action queue is empty");
        }
        else
        {
            ULONG index = Ring->ReadIndex++;
            RtlCopyMemory(Node, &Ring->Nodes[index], sizeof(SDP_ACTION_NODE));
            if (Ring->ReadIndex == SDP_ACTION_QUEUE_DEPTH)
            {
                Ring->ReadIndex = 0;
            }
        }
    }

    if (locked)
    {
        KeReleaseGuardedMutex(&Ring->Lock);
    }
    return status;
}

// Queues an action for the panel's worker. When Completion is given the
// call blocks until the worker finished and returns the handler's status
NTSTATUS
AddDisplayActionRequestHelper(
    _In_ PPANEL_CONTEXT Panel,
    _In_ ULONG Action,
    _In_opt_ PVOID Arg1,
    _In_opt_ PVOID Arg2,
    _In_ PKEVENT Completion)
{
    PDEVICE_CONTEXT device = DeviceGetContext(Panel->Device);
    LONG result = 0x80;
    NTSTATUS status;

    if (device->State == SDP_DEVICE_STATE_NONE)
    {
        return STATUS_DEVICE_NOT_READY;
    }

    SDP_ACTION_NODE node;
    RtlZeroMemory(&node, sizeof(node));
    node.Action = Action;
    node.Arg1 = Arg1;
    node.Arg2 = Arg2;
    node.CompletionEvent = Completion;
    node.Status = (Completion != NULL) ? &result : NULL;

    status = DisplayActionQueueAddActions(&Panel->ActionRing, &node, 1);
    if (NT_SUCCESS(status) && Completion != NULL)
    {
        BlockDisplayActionThread(&node);
        status = result;
    }
    return status;
}

// Synchronous action request used by IOCTLs, QDI callbacks and the poll timer
NTSTATUS
AddDisplayActionRequestSynchronous(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Action, _In_opt_ PVOID Arg1, _In_opt_ PVOID Arg2)
{
    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Panel->State == SDP_PANEL_STATE_TERMINATING)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Panel is terminating, action 0x%x ignored", Action);
        return STATUS_SUCCESS;
    }

    KEVENT completion;
    KeInitializeEvent(&completion, SynchronizationEvent, FALSE);
    return AddDisplayActionRequestHelper(Panel, Action, Arg1, Arg2, &completion);
}

// Blocks the requester until the worker has processed the node
VOID
BlockDisplayActionThread(_In_ SDP_ACTION_NODE* Node)
{
    if (Node != NULL && Node->Status != NULL)
    {
        NTSTATUS status = KeWaitForSingleObject(Node->CompletionEvent, Executive, KernelMode, FALSE, NULL);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Waiting for the action completion failed %!STATUS!", status);
        }
    }
}

// Called by the worker when the handler returned
VOID
UnBlockDisplayActionThread(_In_ SDP_ACTION_NODE* Node, _In_ LONG Status)
{
    if (Node != NULL && Node->Status != NULL)
    {
        *Node->Status = Status;
        KeSetEvent(Node->CompletionEvent, 0, FALSE);
    }
}

// Microseconds of interrupt time
ULONGLONG
UsFromBootPrecise(VOID)
{
    ULONGLONG qpc;
    return KeQueryInterruptTimePrecise(&qpc) / 10;
}
VOID
DelayMicroseconds(_In_ ULONG Microseconds)
{
    LARGE_INTEGER interval;
    interval.QuadPart = -10ll * Microseconds;
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}
VOID
DelayMilliseconds(_In_ ULONG Milliseconds)
{
    LARGE_INTEGER interval;
    interval.QuadPart = WDF_REL_TIMEOUT_IN_MS(Milliseconds);
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}
