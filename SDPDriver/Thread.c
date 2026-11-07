// Display worker thread, status thread and the thread helper object.

#include "SdpDriver.h"
#include "Thread.tmh"

// Creates the system thread described by Thread->StartRoutine
NTSTATUS
WorkerThreadCreate(_Inout_ SDP_THREAD* Thread, _In_ PVOID Context)
{
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    NTSTATUS status = PsCreateSystemThread(
        &Thread->Handle, THREAD_ALL_ACCESS, &objectAttributes, NULL, NULL, Thread->StartRoutine, Context);
    if (!NT_SUCCESS(status))
    {
        Thread->Handle = NULL;
    }
    else
    {
        status = ObReferenceObjectByHandle(
            Thread->Handle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, (PVOID*)&Thread->Thread, NULL);
        if (!NT_SUCCESS(status))
        {
            Thread->Thread = NULL;
            ZwClose(Thread->Handle);
            Thread->Handle = NULL;
        }
    }
    return status;
}

// Initialises the synchronization objects of a thread helper
VOID
WorkerThreadInitContext(_Out_ SDP_THREAD* Thread, _In_ BOOLEAN AutoReset)
{
    KeInitializeEvent(&Thread->Event0, AutoReset ? SynchronizationEvent : NotificationEvent, FALSE);
    KeInitializeEvent(&Thread->StopEvent, SynchronizationEvent, FALSE);
    KeInitializeGuardedMutex(&Thread->Lock);
}

// Waits for the thread to exit and releases it
VOID
DestroyThread(_Inout_ SDP_THREAD* Thread)
{
    KeWaitForSingleObject(Thread->Thread, Executive, KernelMode, FALSE, NULL);
    ObDereferenceObject(Thread->Thread);
    ZwClose(Thread->Handle);
    Thread->Handle = NULL;
    Thread->Thread = NULL;
}

// Creates the action ring plus the worker and status thread of each panel
NTSTATUS
OemPanelThreadCreate(_In_ PDEVICE_CONTEXT Device)
{
    NTSTATUS status = STATUS_SUCCESS;

    for (ULONG i = 0; i < Device->PanelCount; i++)
    {
        PPANEL_CONTEXT panel = &Device->Panels[i];

        DisplayActionQueueInitalize(&panel->ActionRing);

        panel->ActionThread.StartRoutine = DisplayWorkerThread;
        status = WorkerThreadCreate(&panel->ActionThread, panel);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "Starting the display worker thread failed %!STATUS!", status);
            goto Done;
        }

        panel->StatusThread.StartRoutine = ColorWorkerThread;
        WorkerThreadInitContext(&panel->StatusThread, TRUE);
        status = WorkerThreadCreate(&panel->StatusThread, panel);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "Starting the status thread failed %!STATUS!", status);
            goto Done;
        }
    }

Done:
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "OemPanelThreadCreate failed %!STATUS!", status);
    }
    return status;
}

// Worker: executes the queued actions of one panel until ExitDisplayThread
VOID
DisplayWorkerThread(_In_ PVOID Context)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Context;
    SDP_ACTION_NODE node;
    RtlZeroMemory(&node, sizeof(node));

    panel->WorkerProcessId = PsGetCurrentProcessId();

    do
    {
        DisplayActionQueueWaitNewandGet(&panel->ActionRing, &node);

                if (!node.Skip)
        {
            NTSTATUS status = ProcessDisplayAction(panel, &node);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(
                    TRACE_LEVEL_WARNING, TRACE_THREAD, "Display action 0x%x failed %!STATUS!", node.Action, status);

                TraceLoggingWrite(
                    g_SdpTraceLoggingProvider,
                    "DisplayWorkerThreadActionFail",
                    TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
                    TraceLoggingHexUInt32(node.Action, "DisplayAction"));
            }

            UnBlockDisplayActionThread(&node, status);
        }
    } while (node.Action != DisplayActionExitThread);

    TraceEvents(TRACE_LEVEL_WARNING, TRACE_THREAD, "Display worker of display %u exits", panel->DisplayId);

    panel->State = SDP_PANEL_STATE_TERMINATING;
    ToggleResetLine(&panel->ResetGpio, SDP_RESET_GPIO_ASSERT);
}

// Completes pending IOCTL_SDP_WAIT_PANEL_STATE requests with the panel state
// when one of the state events fires
VOID
ColorWorkerThread(_In_ PVOID Context)
{
    PPANEL_CONTEXT panel = (PPANEL_CONTEXT)Context;
    ULONG waitIndex = 0;
    WDFREQUEST request = NULL;

    panel->StatusWaitObjects[0] = &panel->StatusThread.Event0;
    panel->StatusWaitObjects[1] = &panel->StatusThread.StopEvent;
    panel->StatusWaitObjects[2] = &panel->StatusEvent;

    while (waitIndex != 1)
    {
        waitIndex = KeWaitForMultipleObjects(
            3, panel->StatusWaitObjects, WaitAny, Executive, KernelMode, FALSE, NULL, NULL);
        if (waitIndex == 1)
        {
            break;
        }

        ULONG state = (waitIndex == 2) ? 3 : 2;

        NTSTATUS status = WdfIoQueueRetrieveNextRequest(panel->StatusQueue, &request);
        if (!NT_SUCCESS(status))
        {
            if (status == STATUS_NO_MORE_ENTRIES)
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "No pending status request %!STATUS!", status);
            }
            else
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "WdfIoQueueRetrieveNextRequest failed %!STATUS!", status);
            }
            continue;
        }

        WDF_REQUEST_PARAMETERS parameters;
        WDF_REQUEST_PARAMETERS_INIT(&parameters);
        WdfRequestGetParameters(request, &parameters);

        if (parameters.Parameters.DeviceIoControl.OutputBufferLength != sizeof(SDP_PANEL_STATE_NOTIFICATION))
        {
            status = STATUS_INVALID_BUFFER_SIZE;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "Unexpected output length %!STATUS!", status);
        }
        else
        {
            SDP_PANEL_STATE_NOTIFICATION* notification = NULL;

            status = WdfRequestRetrieveOutputBuffer(
                request, sizeof(SDP_PANEL_STATE_NOTIFICATION), (PVOID*)&notification, NULL);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_THREAD, "WdfRequestRetrieveOutputBuffer failed %!STATUS!", status);
            }
            else
            {
                KeAcquireGuardedMutex(&panel->Lock);
                notification->State = state;
                notification->PanelId = panel->PanelId;
                notification->PanelVersion = panel->PanelVersion;
                RtlCopyMemory(notification->Data, &panel->StatusData, sizeof(notification->Data));
                KeReleaseGuardedMutex(&panel->Lock);
            }
        }

        WdfRequestCompleteWithInformation(request, status, sizeof(SDP_PANEL_STATE_NOTIFICATION));
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_THREAD, "Status thread exits");
}
