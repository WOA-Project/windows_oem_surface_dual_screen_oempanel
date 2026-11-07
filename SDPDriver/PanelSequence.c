// DSI command lists: building, sending and reading back through the Graphics
// KMD (QDI_DisplayUtil_SendCmdBuf / ReadCmdBuf).

#include "SdpDriver.h"
#include "PanelSequence.tmh"

#define SDP_DSI_RETRY_COUNT         3
#define SDP_DSI_WRITE_RETRY_MS      50
#define SDP_DSI_READ_RETRY_MS       500
#define SDP_DSI_LOG_MAX_BYTES       200

// Appends a node at the end of the list that starts with Head
// The payload is copied. A persistent node survives SendSequence.
NTSTATUS
AppendCommand(
    _Inout_ SDP_DSI_NODE* Head,
    _In_ ULONG Type,
    _In_ UCHAR PacketType,
    _In_ ULONG Flags,
    _In_reads_bytes_(DataSize) PUCHAR Data,
    _In_ ULONG DataSize,
    _In_ BOOLEAN Persistent)
{
    NTSTATUS status = STATUS_SUCCESS;
    SDP_DSI_NODE* node = NULL;

    if (Head == NULL || Data == NULL || Type > SDP_DSI_NODE_WRITE)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "AppendCommand: invalid parameter");
        return STATUS_INVALID_PARAMETER;
    }

    SDP_DSI_NODE* tail = Head;
    while (tail->Next != NULL)
    {
        tail = tail->Next;
    }

    node = (SDP_DSI_NODE*)ExAllocatePoolWithTag(PagedPool, sizeof(SDP_DSI_NODE), SDP_POOL_TAG);
    if (node == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Allocating a DSI node failed %!STATUS!", status);
    }
    else
    {
        RtlZeroMemory(node, sizeof(*node));

        node->Data = (PUCHAR)ExAllocatePoolWithTag(PagedPool, DataSize, SDP_POOL_TAG);
        if (node->Data == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Allocating the DSI payload failed %!STATUS!", status);
        }
        else
        {
            RtlCopyMemory(node->Data, Data, DataSize);
            node->Flags = (Flags == 0) ? SDP_SEQ_NODE_FLAGS_DEFAULT : Flags;
            node->Type = Type;
            node->DataSize = DataSize;
            node->PacketType = PacketType;
            node->TxFlags = 0;
            node->Persistent = Persistent ? TRUE : FALSE;
            node->Next = NULL;
            tail->Next = node;
        }
    }

    if (!NT_SUCCESS(status) && node != NULL)
    {
        if (node->Data != NULL)
        {
            ExFreePoolWithTag(node->Data, SDP_POOL_TAG);
        }
        ExFreePoolWithTag(node, SDP_POOL_TAG);
    }
    return status;
}

// Pseudo packet 0xFF: delay in milliseconds
NTSTATUS
AppendDelay(_Inout_ SDP_DSI_NODE* Head, _In_ UCHAR Milliseconds, _In_ BOOLEAN Persistent)
{
    return AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DELAY_MS, 2, &Milliseconds, 1, Persistent);
}

// DCS display off (0x28) / on (0x29)
NTSTATUS
AppendDisplayCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent)
{
    UCHAR command = (UCHAR)DcsCommand;

    if (command != 0x29 && command != 0x28)
    {
        return STATUS_INVALID_PARAMETER;
    }
    return AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, &command, 1, Persistent);
}

// Any DCS short write without parameter
NTSTATUS
AppendSleepCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent)
{
    UCHAR command = (UCHAR)DcsCommand;
    return AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, &command, 1, Persistent);
}

// DCS tearing effect off (0x34) / on (0x35)
NTSTATUS
AppendTECommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG DcsCommand, _In_ BOOLEAN Persistent)
{
    UCHAR command = (UCHAR)DcsCommand;

    if (command != 0x34 && command != 0x35)
    {
        return STATUS_INVALID_PARAMETER;
    }
    return AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_SHORT_WRITE_0, 2, &command, 1, Persistent);
}

// DCS set_tear_scanline (0x44) with a big endian line number
NTSTATUS
AppendTESetScanLineCommand(_Inout_ SDP_DSI_NODE* Head, _In_ ULONG Scanline, _In_ BOOLEAN Persistent)
{
    UCHAR data[3];

    data[0] = 0x44;
    data[1] = (UCHAR)(Scanline >> 8);
    data[2] = (UCHAR)Scanline;
    return AppendCommand(Head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_DCS_LONG_WRITE, 2, data, sizeof(data), Persistent);
}

// Frees the nodes that follow a list head
VOID
Free_DisplaySequence(_In_opt_ SDP_DSI_NODE* FirstNode)
{
    SDP_DSI_NODE* node = FirstNode;

        while (node != NULL)
    {
        SDP_DSI_NODE* next = node->Next;

        if (node->Data != NULL)
        {
            ExFreePoolWithTag(node->Data, SDP_POOL_TAG);
        }
        ExFreePoolWithTag(node, SDP_POOL_TAG);
        node = next;
    }
}

// Releases the panel's cached sequences The version specific
// list (Sequences[1]) is not released by the binary.
NTSTATUS
Free_PreAllocated_DisplaySequence(_Inout_ PPANEL_CONTEXT Panel)
{
    if (Panel == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    Free_DisplaySequence(Panel->Sequences[0].Next);
    RtlZeroMemory(&Panel->Sequences[0], sizeof(SDP_DSI_NODE));
    Free_DisplaySequence(Panel->Sequences[2].Next);
    RtlZeroMemory(&Panel->Sequences[2], sizeof(SDP_DSI_NODE));
    Free_DisplaySequence(Panel->Sequences[3].Next);
    RtlZeroMemory(&Panel->Sequences[3], sizeof(SDP_DSI_NODE));
    Panel->SequencesValid = FALSE;
    return STATUS_SUCCESS;
}

// Hex dump of a DSI packet to the trace
VOID
LogDSICommandString(_In_ UCHAR PacketType, _In_reads_bytes_opt_(Size) PUCHAR Data, _In_ ULONG Size)
{
    if (Data == NULL)
    {
        return;
    }

    WCHAR lineBuffer[0x100];
    WCHAR digitsBuffer[4];
    UNICODE_STRING line;
    UNICODE_STRING digits;

    RtlZeroMemory(lineBuffer, sizeof(lineBuffer));
    RtlZeroMemory(digitsBuffer, sizeof(digitsBuffer));
    RtlInitEmptyUnicodeString(&line, lineBuffer, 0x200);
    RtlInitEmptyUnicodeString(&digits, digitsBuffer, 8);

    NTSTATUS status = RtlIntegerToUnicodeString(PacketType, 16, &digits);
    NTSTATUS appendStatus = RtlAppendUnicodeStringToString(&line, &digits);
    NTSTATUS spaceStatus = RtlAppendUnicodeToString(&line, L" ");

    if (NT_SUCCESS(status | appendStatus | spaceStatus))
    {
        ULONG count = min(Size, SDP_DSI_LOG_MAX_BYTES);

        for (ULONG i = 0; i < count; i++)
        {
            RtlIntegerToUnicodeString(Data[i], 16, &digits);
            RtlAppendUnicodeStringToString(&line, &digits);
            RtlAppendUnicodeToString(&line, L" ");
        }

        TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_PANEL, "DSI packet: %ws", lineBuffer);
    }
}

// Reads the DSI error counters from QDI and caches them
VOID
GetDSIError(_In_ PPANEL_CONTEXT Panel, _In_ ULONG Status)
{
    ULONG hostErrors = 0;
    ULONG panelErrors = 0;

    QDI_ErrorType qdiStatus =
        Panel->DisplayUtil.QDI_DisplayUtil_GetLastDSIErrors(Panel->DisplayId, &hostErrors, &panelErrors, 0);
    if (qdiStatus == QDI_STATUS_OK)
    {
        KeAcquireGuardedMutex(&Panel->Lock);
        Panel->DsiHostErrors = hostErrors;
        Panel->DsiPanelErrors = panelErrors;
        Panel->DsiLastStatus = Status;
        KeReleaseGuardedMutex(&Panel->Lock);

        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI errors: host 0x%x panel 0x%x", hostErrors, panelErrors);
    }
    else
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "QDI_DisplayUtil_GetLastDSIErrors failed");
    }
}

// Sends a write transfer to the panel, retrying up to three times.
static QDI_ErrorType
SdpSeqSendNode(_In_ PPANEL_CONTEXT Panel, _Inout_ SDP_DSI_NODE* Node)
{
    QDI_ErrorType qdiStatus;
    ULONG attempt = 0;

    Node->TxFlags |= Node->Flags;

    do
    {
        LogDSICommandString(Node->PacketType, Node->Data, Node->DataSize);

        qdiStatus = Panel->DisplayUtil.QDI_DisplayUtil_SendCmdBuf(
            Panel->DisplayId, Node->PacketType, Node->Data, Node->DataSize, Node->TxFlags);
        attempt++;
        if (qdiStatus == QDI_STATUS_OK)
        {
            break;
        }
        DelayMilliseconds(SDP_DSI_WRITE_RETRY_MS);
    } while (attempt < SDP_DSI_RETRY_COUNT);

    return qdiStatus;
}

// Sends every node of a write only list and consumes the temporary nodes
NTSTATUS
SendSequence(_In_ PPANEL_CONTEXT Panel, _Inout_ SDP_DSI_NODE* Head)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Panel == NULL || Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    SDP_DSI_NODE* node = (Head->Next != NULL) ? Head->Next : Head;

    if (node->DataSize > SDP_MAX_DSI_PAYLOAD)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI payload too large (%u)", node->DataSize);
        Free_DisplaySequence(node);
        return STATUS_INVALID_PARAMETER;
    }

    do
    {
        QDI_ErrorType qdiStatus = SdpSeqSendNode(Panel, node);
        if (qdiStatus != QDI_STATUS_OK)
        {
            GetDSIError(Panel, qdiStatus);
            status = STATUS_UNSUCCESSFUL;
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI write failed %u", qdiStatus);

            TraceLoggingWrite(
                g_SdpTraceLoggingProvider, "SendSequenceFail", TraceLoggingLevel(SDP_TL_LEVEL_ERROR));
        }

        SDP_DSI_NODE* next = node->Next;

        if (node->Data != NULL && node->Persistent != TRUE)
        {
            ExFreePoolWithTag(node->Data, SDP_POOL_TAG);
            node->Data = NULL;
        }

                if (node != Head && node->Persistent != TRUE)
        {
            ExFreePoolWithTag(node, SDP_POOL_TAG);
        }
        node = next;
    } while (node != NULL);

    if (Head->Persistent != TRUE)
    {
        Head->Next = NULL;
    }
    return status;
}

// Executes a list that mixes writes and reads Read nodes fill
// ReadBuffer (a read that returns fewer bytes than requested is accumulated
// until the buffer is complete). Returns STATUS_PENDING when a read failed.
NTSTATUS
ProcessSequence(
    _In_ PPANEL_CONTEXT Panel,
    _Inout_ SDP_DSI_NODE* Head,
    _Out_writes_bytes_(ReadBufferSize) PUCHAR ReadBuffer,
    _In_ SIZE_T ReadBufferSize)
{
    NTSTATUS status = STATUS_SUCCESS;
    PUCHAR scratch = NULL;
    BOOLEAN readDone = FALSE;
    ULONG copyOffset = 0;
    ULONG readFailure = 0;
    ULONG remaining = (ULONG)ReadBufferSize;

    if (Panel == NULL || Head == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    scratch = (PUCHAR)ExAllocatePoolWithTag(PagedPool, ReadBufferSize, SDP_POOL_TAG);
    if (scratch == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    SDP_DSI_NODE* node = (Head->Next != NULL) ? Head->Next : Head;

    if (node->DataSize > SDP_MAX_DSI_PAYLOAD)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI payload too large (%u)", node->DataSize);
        Free_DisplaySequence(node);
    }
    else
    {
        do
        {
            ULONG attempt = 0;
            QDI_ErrorType qdiStatus = QDI_STATUS_OK;

            if (node->Type == SDP_DSI_NODE_READ)
            {
                if (!readDone)
                {
                    do
                    {
                        qdiStatus = Panel->DisplayUtil.QDI_DisplayUtil_ReadCmdBuf(
                            Panel->DisplayId,
                            node->PacketType,
                            node->Data,
                            node->DataSize,
                            ReadBuffer,
                            &remaining,
                            QDI_DSI_CMD_FLAG_DMA_FORCE_LP);
                        attempt++;
                        if (qdiStatus == QDI_STATUS_OK)
                        {
                            break;
                        }
                        DelayMilliseconds(SDP_DSI_READ_RETRY_MS);
                    } while (attempt < SDP_DSI_RETRY_COUNT);

                    if (qdiStatus != QDI_STATUS_OK)
                    {
                        GetDSIError(Panel, qdiStatus);
                        status = STATUS_UNSUCCESSFUL;
                        readFailure = qdiStatus;
                        TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI read failed %u", qdiStatus);

                        TraceLoggingWrite(
                            g_SdpTraceLoggingProvider,
                            "ProcessSeqReadCmdFailed",
                            TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
                            TraceLoggingHexUInt32(qdiStatus, "QDI_STATUS"));
                    }
                }
            }
            else if (node->Type == SDP_DSI_NODE_WRITE)
            {
                qdiStatus = SdpSeqSendNode(Panel, node);
                if (qdiStatus != QDI_STATUS_OK)
                {
                    GetDSIError(Panel, qdiStatus);
                    status = STATUS_UNSUCCESSFUL;
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "DSI write failed %u", qdiStatus);

                    TraceLoggingWrite(
                        g_SdpTraceLoggingProvider,
                        "ProcessSeqWriteCmdFailed",
                        TraceLoggingLevel(SDP_TL_LEVEL_ERROR),
                        TraceLoggingHexUInt32(qdiStatus, "QDI_STATUS"));
                }
            }
            else
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_PANEL, "Unknown DSI node type %u", node->Type);
            }

            if (node->Type == SDP_DSI_NODE_READ)
            {
                if (remaining == ReadBufferSize)
                {
                    readDone = TRUE;
                }
                else
                {
                    RtlCopyMemory(scratch + copyOffset, ReadBuffer, remaining);
                    copyOffset += remaining;
                    remaining = (ULONG)ReadBufferSize;

                    if (copyOffset == ReadBufferSize)
                    {
                        readDone = TRUE;
                        RtlCopyMemory(ReadBuffer, scratch, ReadBufferSize);
                        copyOffset = 0;
                    }
                }
            }

            SDP_DSI_NODE* next = node->Next;

            if (node->Data != NULL && node->Persistent != TRUE)
            {
                ExFreePoolWithTag(node->Data, SDP_POOL_TAG);
                node->Data = NULL;
            }
            if (node != Head && node->Persistent != TRUE)
            {
                ExFreePoolWithTag(node, SDP_POOL_TAG);
            }
            node = next;
        } while (node != NULL);

        if (Head->Persistent != TRUE)
        {
            Head->Next = NULL;
        }
    }

    ExFreePoolWithTag(scratch, SDP_POOL_TAG);

    if (readFailure != 0)
    {
        status = STATUS_PENDING;
    }
    return status;
}

// IOCTL_SDP_DSI_READ: sets the maximum return packet size, issues the read
// and restores the size to 1
NTSTATUS
ProcessIoctlReadBuffer(_In_ PPANEL_CONTEXT Panel, _In_ SDP_DSI_READ_REQUEST* Request, _Out_ PUCHAR Output)
{
    SDP_DSI_NODE head;
    NTSTATUS status = STATUS_SUCCESS;
    UCHAR maxReturnSize[2] = {0, 0};
    UCHAR restoreSize[2] = {1, 0};
    USHORT readSize = (USHORT)Request->ReadSize;
    UCHAR packetType = Request->Data[0];

    RtlZeroMemory(&head, sizeof(head));

    PUCHAR payload = (PUCHAR)ExAllocatePoolWithTag(PagedPool, Request->DataSize, SDP_POOL_TAG);
    if (payload == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(payload, Request->Data + 1, Request->DataSize - 1);

    if (readSize < 4)
    {
        readSize = 4;
    }
    RtlCopyMemory(maxReturnSize, &readSize, sizeof(readSize));

    status = AppendCommand(&head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, maxReturnSize, 2, FALSE);
    status |= AppendCommand(&head, SDP_DSI_NODE_READ, packetType, 1, payload, Request->DataSize - 1, FALSE);
    status |= AppendCommand(&head, SDP_DSI_NODE_WRITE, SDP_DSI_DT_SET_MAX_RETURN_SIZE, 2, restoreSize, 2, FALSE);

    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
    }
    else
    {
        RtlZeroMemory(Output, readSize);
        status = ProcessSequence(Panel, &head, Output, readSize);
    }

    ExFreePoolWithTag(payload, SDP_POOL_TAG);
    return status;
}

// IOCTL_SDP_DSI_WRITE: Data[0] is the packet type, the rest the payload
NTSTATUS
ProcessIoctlWriteBuffer(_In_ PPANEL_CONTEXT Panel, _In_reads_bytes_(*Size) PUCHAR Data, _In_ PULONG Size)
{
    SDP_DSI_NODE head;
    ULONG size = *Size;
    UCHAR packetType = Data[0];

    RtlZeroMemory(&head, sizeof(head));

    NTSTATUS status = AppendCommand(&head, SDP_DSI_NODE_WRITE, packetType, 2, Data + 1, size - 1, FALSE);
    if (!NT_SUCCESS(status))
    {
        Free_DisplaySequence(head.Next);
    }
    else
    {
        status = SendSequence(Panel, &head);
    }
    return status;
}

// Marks a persistent list head
VOID
PREDEFINED_DISPLAY_SEQ_INIT(_Inout_ SDP_DSI_NODE* Head)
{
    if (Head != NULL)
    {
        Head->Persistent = TRUE;
    }
}
