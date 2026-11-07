// System event log entries (message table of SDPDriver.sys).

#include "SdpDriver.h"
#include "ErrorLog.tmh"

#define SDP_LOG_STRING_SLOTS_BYTES  0x200   // 8 insertion strings of 32 WCHAR
#define SDP_LOG_STRING_SLOT_CCH     0x20
#define SDP_LOG_BLOCK_BYTES         0x204   // slots + string count
#define SDP_LOG_MAX_PACKET_SIZE     0xF0
#define SDP_LOG_PACKET_BASE_SIZE    0x30

typedef enum _SDP_FORMAT_KIND
{
    SdpFormatInvalid = 0,
    SdpFormatChar = 1,
    SdpFormatInteger = 3,
    SdpFormatPointer = 4,
    SdpFormatNone = 5,
} SDP_FORMAT_KIND;

// Classifies a printf format with at most one conversion
static SDP_FORMAT_KIND
EventLogFormatSizeTypeGet(_In_ PCWSTR Format)
{
    SDP_FORMAT_KIND kind = SdpFormatInvalid;
    BOOLEAN conversionSeen = FALSE;

    for (PCWSTR p = Format; *p != L'\0' && kind == SdpFormatInvalid; p++)
    {
        if (*p == L'%' && p[1] != L'\0')
        {
            WCHAR conversion = p[1];

            conversionSeen = TRUE;
            if (conversion == L'd' || conversion == L'X' || conversion == L'u' || conversion == L'x')
            {
                kind = SdpFormatInteger;
            }
            else if (conversion == L'S' || conversion == L'p' || conversion == L's')
            {
                kind = SdpFormatPointer;
            }
            else if (conversion == L'c')
            {
                kind = SdpFormatChar;
            }
            else if (conversion != L'%')
            {
                break;
            }
        }
    }

    if (!conversionSeen)
    {
        kind = SdpFormatNone;
    }
    return kind;
}

// Formats up to 8 insertion strings, one per format, from the variable
// arguments Returns NULL when out of memory.
static PWCHAR
EventLogInsertionStringTableAllocate(_In_ ULONG ArgumentCount, _In_ va_list Arguments, _In_ ULONG FormatCount, _In_ PCWSTR* Formats)
{
    PWCHAR block = (PWCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, SDP_LOG_BLOCK_BYTES, SDP_POOL_TAG);
    if (block == NULL)
    {
        return NULL;
    }

    RtlZeroMemory(block, SDP_LOG_BLOCK_BYTES);

    for (ULONG i = 0; i < FormatCount; i++)
    {
        PWCHAR slot = block + i * SDP_LOG_STRING_SLOT_CCH;
        PCWSTR format = Formats[i];
        SDP_FORMAT_KIND kind = EventLogFormatSizeTypeGet(format);

        if (kind == SdpFormatChar)
        {
            // The binary reads the first byte of the argument slot, sign-extended
            RtlStringCchPrintfW(slot, SDP_LOG_STRING_SLOT_CCH, format, (int)(CHAR)va_arg(Arguments, int));
        }
        else if (kind == SdpFormatInteger)
        {
            RtlStringCchPrintfW(slot, SDP_LOG_STRING_SLOT_CCH, format, va_arg(Arguments, ULONG));
        }
        else if (kind == SdpFormatPointer)
        {
            RtlStringCchPrintfW(slot, SDP_LOG_STRING_SLOT_CCH, format, va_arg(Arguments, PVOID));
        }
        else if (kind != SdpFormatNone)
        {
            return block;
        }
    }

    *(PULONG)((PUCHAR)block + SDP_LOG_STRING_SLOTS_BYTES) = ArgumentCount;
    return block;
}

// Allocates and writes the IO_ERROR_LOG_PACKET
static VOID
Utility_EventLogEntryWriteToEventLogFile(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ NTSTATUS MessageCode,
    _In_ NTSTATUS Status,
    _In_ ULONG UniqueId,
    _In_ ULONG DumpDataSize,
    _In_opt_ PUCHAR DumpData,
    _In_opt_ PWCHAR Strings)
{
    ULONG dumpBytes = 0;
    ULONG stringsOffset = 0;
    ULONG stringCount = 0;
    ULONG packetSize = SDP_LOG_PACKET_BASE_SIZE;

    if (DumpData != NULL)
    {
        dumpBytes = SDP_LOG_PACKET_BASE_SIZE;
        packetSize = (DumpDataSize + 1) * 2 + SDP_LOG_PACKET_BASE_SIZE;
    }

    if (Strings != NULL)
    {
        stringsOffset = packetSize;
        stringCount = *(PULONG)((PUCHAR)Strings + SDP_LOG_STRING_SLOTS_BYTES);
        for (ULONG i = 0; i < stringCount; i++)
        {
            packetSize += ((ULONG)wcslen(Strings + i * SDP_LOG_STRING_SLOT_CCH) + 1) * sizeof(WCHAR);
        }
    }

    if (packetSize > SDP_LOG_MAX_PACKET_SIZE)
    {
        packetSize = SDP_LOG_PACKET_BASE_SIZE;
    }

    PIO_ERROR_LOG_PACKET packet = (PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(DriverObject, (UCHAR)packetSize);
    if (packet == NULL)
    {
        return;
    }

    packet->MajorFunctionCode = 0;
    packet->RetryCount = 0;
    packet->EventCategory = 0;
    packet->ErrorCode = MessageCode;
    packet->UniqueErrorValue = UniqueId;
    packet->FinalStatus = Status;
    packet->SequenceNumber = 0;
    packet->IoControlCode = 0;

        if (dumpBytes < packetSize && DumpData != NULL)
    {
        packet->DumpDataSize = (USHORT)DumpDataSize;
        RtlCopyMemory(packet->DumpData, DumpData, packetSize - dumpBytes);
    }

    if (stringsOffset < packetSize && stringCount != 0)
    {
        PUCHAR cursor = (PUCHAR)packet + stringsOffset;

        packet->NumberOfStrings = (USHORT)stringCount;
        packet->StringOffset = (USHORT)stringsOffset;

        for (ULONG i = 0; i < stringCount; i++)
        {
            PWCHAR string = Strings + i * SDP_LOG_STRING_SLOT_CCH;
            ULONG bytes = ((ULONG)wcslen(string) + 1) * sizeof(WCHAR);

            RtlCopyMemory(cursor, string, bytes);
            cursor += bytes;
        }
    }
    else
    {
        packet->NumberOfStrings = 0;
        packet->StringOffset = 0;
    }

    IoWriteErrorLogEntry(packet);
}

// Writes an event log entry with up to 8 insertion strings built from
// Formats[0..FormatCount) and the variable arguments The
// binary passes the action / event name as the format, which is never
// substituted, so the strings come out empty.
VOID
Utility_EventLogEntryWrite(
    _In_ WDFDEVICE Device,
    _In_ NTSTATUS MessageCode,
    _In_ NTSTATUS Status,
    _In_ ULONG UniqueId,
    _In_ ULONG DumpDataSize,
    _In_opt_ PUCHAR DumpData,
    _In_ ULONG FormatCount,
    _In_opt_ PCWSTR* Formats,
    _In_ ULONG HasArguments,
    ...)
{
    PWCHAR strings = NULL;

    if (HasArguments != 0)
    {
        va_list arguments;
        va_start(arguments, HasArguments);
        strings = EventLogInsertionStringTableAllocate(HasArguments, arguments, FormatCount, Formats);
        va_end(arguments);
    }

    PDRIVER_OBJECT driverObject = WdfDriverWdmGetDriverObject(WdfDeviceGetDriver(Device));
    Utility_EventLogEntryWriteToEventLogFile(driverObject, MessageCode, Status, UniqueId, DumpDataSize, DumpData, strings);

    if (strings != NULL)
    {
        ExFreePoolWithTag(strings, SDP_POOL_TAG);
    }
}
