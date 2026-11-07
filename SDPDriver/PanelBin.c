// Panel bin files ("DisplayRefreshRate-PanelN.bin" on the DPP partition) and
// the calibration blob parser.

#include "SdpDriver.h"
#include "sfpd.h"
#include "PanelBin.tmh"

// Status codes of the panel bin helpers (not NTSTATUS values).
#define SDP_BIN_OK              0
#define SDP_BIN_PATH_FAILED     2
#define SDP_BIN_OPEN_FAILED     3
#define SDP_BIN_READ_FAILED     4
#define SDP_BIN_QUERY_FAILED    7
#define SDP_BIN_BAD_PARAMETER   9
#define SDP_BIN_NO_MEMORY       10

#define SDP_PATH_BYTES          0x208
#define SDP_PATH_CCH_LIMIT      0x82    // limit used by the binary when appending to the path
#define SDP_MAX_DISKS           6

static const WCHAR g_PanelBinNames[SDP_PANEL_COUNT][30] = {
    L"DisplayRefreshRate-Panel0.bin",
    L"DisplayRefreshRate-Panel1.bin",
};
static const WCHAR g_PanelBinDirectory[] = L"OEM";

// Microsoft basic data partition named "DPP".
static const GUID g_BasicDataPartitionType = {
    0xebd0a0a2, 0xb9e5, 0x4433, {0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7}};
static const SDP_PARTITION_QUERY g_DppPartition = {&g_BasicDataPartitionType, L"DPP"};

// Looks for the GPT partition of the given type/name on the first disks and
// returns "\Device\HarddiskN\PartitionM\" and its length
NTSTATUS
GetGPTVolumePathAndSizeEx(
    _In_ WDFDEVICE Device,
    _In_ const SDP_PARTITION_QUERY* Query,
    _In_ ULONG PartitionIndexHint,
    _Out_writes_(0x104) PWCHAR Path,
    _Out_ PULONGLONG PartitionLength)
{
    UNREFERENCED_PARAMETER(PartitionIndexHint);

    NTSTATUS status = STATUS_SUCCESS;
    WDFIOTARGET target = NULL;
    WDFREQUEST request = NULL;
    WDFMEMORY memory = NULL;
    PDRIVE_LAYOUT_INFORMATION_EX layout = NULL;
    ULONG entryCapacity = 5;
    ULONG diskIndex = 0;
    ULONG partitionNumber = 0;
    ULONGLONG length = 0;
    BOOLEAN found = FALSE;
    PWCHAR diskPath = NULL;

    if (Path == NULL || PartitionLength == NULL || Query == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    *PartitionLength = 0;

    if (!NT_SUCCESS(RtlStringCbPrintfW(Path, SDP_PATH_BYTES, L"INVALID PATH")))
    {
        return STATUS_INTERNAL_ERROR;
    }

    for (;;)
    {
        diskPath = (PWCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, SDP_PATH_BYTES, SDP_POOL_TAG_PARTITION);
        if (diskPath == NULL)
        {
            status = STATUS_MEMORY_NOT_ALLOCATED;
            goto Done;
        }

        if (!NT_SUCCESS(RtlStringCbPrintfW(diskPath, SDP_PATH_BYTES, L"\\Device\\Harddisk%u\\Partition0", diskIndex)))
        {
            status = STATUS_INTERNAL_ERROR;
            ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
            goto Done;
        }

        UNICODE_STRING diskName;
        RtlInitUnicodeString(&diskName, diskPath);

        WDF_OBJECT_ATTRIBUTES attributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = Device;

        if (!NT_SUCCESS(WdfIoTargetCreate(Device, &attributes, &target)))
        {
            status = STATUS_IO_DEVICE_ERROR;
            ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
            goto Done;
        }

        WDF_IO_TARGET_OPEN_PARAMS openParams;
        RtlZeroMemory(&openParams, sizeof(openParams));
        openParams.Size = sizeof(openParams);
        openParams.Type = WdfIoTargetOpenByName;
        openParams.TargetDeviceName = diskName;
        openParams.DesiredAccess = GENERIC_READ | GENERIC_WRITE;
        openParams.ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE;
        openParams.CreateDisposition = FILE_OPEN;
        openParams.CreateOptions = FILE_NON_DIRECTORY_FILE;

        // The disk can be busy during boot: retry a few times.
        ULONG openAttempts = 0;
        for (;;)
        {
            status = WdfIoTargetOpen(target, &openParams);
            if (status != STATUS_SHARING_VIOLATION)
            {
                break;
            }

            LARGE_INTEGER delay;
            delay.QuadPart = WDF_REL_TIMEOUT_IN_MS(500);
            if (!NT_SUCCESS(KeDelayExecutionThread(KernelMode, FALSE, &delay)))
            {
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }
            if (++openAttempts > 2)
            {
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }
        }
        if (status != STATUS_SUCCESS)
        {
            ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
            goto Done;
        }

        // IOCTL_DISK_GET_DRIVE_LAYOUT_EX with a buffer that grows until it fits.
        do
        {
            WDF_OBJECT_ATTRIBUTES requestAttributes;
            WDF_OBJECT_ATTRIBUTES_INIT(&requestAttributes);
            requestAttributes.ParentObject = target;

            if (!NT_SUCCESS(WdfRequestCreate(&requestAttributes, target, &request)))
            {
                status = STATUS_REQUEST_ABORTED;
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }

            WDF_OBJECT_ATTRIBUTES memoryAttributes;
            WDF_OBJECT_ATTRIBUTES_INIT(&memoryAttributes);
            memoryAttributes.ParentObject = request;

            SIZE_T layoutSize = entryCapacity * sizeof(PARTITION_INFORMATION_EX) + 0xC0;
            if (!NT_SUCCESS(WdfMemoryCreate(
                    &memoryAttributes, NonPagedPoolNx, SDP_POOL_TAG_PARTITION, layoutSize, &memory, (PVOID*)&layout)))
            {
                status = STATUS_MEMORY_NOT_ALLOCATED;
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }

            if (!NT_SUCCESS(WdfIoTargetFormatRequestForIoctl(
                    target, request, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, NULL, memory, NULL)))
            {
                status = STATUS_IO_DEVICE_ERROR;
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }

            WDF_REQUEST_SEND_OPTIONS sendOptions;
            WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

            if (!WdfRequestSend(request, target, &sendOptions))
            {
                status = WdfRequestGetStatus(request);
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }

            status = WdfRequestGetStatus(request);
            if (status != STATUS_BUFFER_TOO_SMALL && status != STATUS_INSUFFICIENT_RESOURCES && status < 0)
            {
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
                goto Done;
            }

            if (status >= 0)
            {
                break;
            }

            entryCapacity <<= 1;
            WdfObjectDelete(memory);
            memory = NULL;
            WdfObjectDelete(request);
            request = NULL;
        } while (TRUE);

        if (layout->PartitionStyle == PARTITION_STYLE_GPT && layout->PartitionCount != 0)
        {
            for (ULONG i = 0; i < layout->PartitionCount; i++)
            {
                PPARTITION_INFORMATION_EX entry = &layout->PartitionEntry[i];

                if (RtlCompareMemory(&entry->Gpt.PartitionType, Query->Type, sizeof(GUID)) == sizeof(GUID))
                {
                                        if (Query->Name == NULL || wcsncmp(entry->Gpt.Name, Query->Name, 36) == 0)
                    {
                        found = TRUE;
                        partitionNumber = entry->PartitionNumber;
                        length = entry->PartitionLength.QuadPart;
                        break;
                    }
                }
            }
        }

        WdfObjectDelete(memory);
        memory = NULL;
        WdfObjectDelete(request);
        request = NULL;
        WdfIoTargetClose(target);
        WdfObjectDelete(target);
        target = NULL;
                ExFreePoolWithTag(diskPath, SDP_POOL_TAG_PARTITION);
        diskPath = NULL;

        if (found)
        {
            if (!NT_SUCCESS(
                    RtlStringCbPrintfW(Path, SDP_PATH_BYTES, L"\\Device\\Harddisk%u\\Partition%u\\", diskIndex, partitionNumber)))
            {
                status = STATUS_INTERNAL_ERROR;
            }
            else
            {
                *PartitionLength = length;
                status = STATUS_SUCCESS;
            }
            goto Done;
        }

        if (diskIndex++ >= SDP_MAX_DISKS - 1)
        {
            break;
        }
    }

Done:
    if (target != NULL)
    {
        WdfObjectDelete(target);
    }
    if (memory != NULL)
    {
        WdfObjectDelete(memory);
    }
    if (request != NULL)
    {
        WdfObjectDelete(request);
    }

    if (status == STATUS_SUCCESS)
    {
        status = found ? STATUS_SUCCESS : STATUS_DEVICE_NOT_CONNECTED;
    }
    return status;
}

// "<partition>\<Directory>\<File>" Returns an SDP_BIN_* code.
static ULONG
GenerateDPPPath_KMDF(_In_ WDFDEVICE Device, _Out_ PWCHAR Path, _In_opt_ PCWSTR Directory, _In_ PCWSTR FileName)
{
    ULONGLONG partitionLength;

    if (FileName == NULL || Path == NULL)
    {
        return SDP_BIN_BAD_PARAMETER;
    }

    if (!NT_SUCCESS(GetGPTVolumePathAndSizeEx(Device, &g_DppPartition, 0, Path, &partitionLength)))
    {
        return SDP_BIN_PATH_FAILED;
    }

    if (Directory != NULL)
    {
        if (!NT_SUCCESS(RtlStringCchCatW(Path, SDP_PATH_CCH_LIMIT, Directory)) ||
            !NT_SUCCESS(RtlStringCchCatW(Path, SDP_PATH_CCH_LIMIT, L"\\")))
        {
            return SDP_BIN_PATH_FAILED;
        }
    }

    return NT_SUCCESS(RtlStringCchCatW(Path, SDP_PATH_CCH_LIMIT, FileName)) ? SDP_BIN_OK : SDP_BIN_PATH_FAILED;
}

// Size of a bin file
static ULONG
GetDPPItemSize_KMDF(_In_ WDFDEVICE Device, _Out_ PULONG Size, _In_ PCWSTR Directory, _In_ PCWSTR FileName)
{
    ULONG result;
    HANDLE file = NULL;

    if (Size == NULL || Directory == NULL || FileName == NULL)
    {
        return SDP_BIN_BAD_PARAMETER;
    }

    PWCHAR path = (PWCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, SDP_PATH_BYTES, SDP_POOL_TAG_PANELBIN);
    if (path == NULL)
    {
        return SDP_BIN_NO_MEMORY;
    }

    result = GenerateDPPPath_KMDF(Device, path, Directory, FileName);
    if (result == SDP_BIN_OK)
    {
        UNICODE_STRING name;
        OBJECT_ATTRIBUTES attributes;
        IO_STATUS_BLOCK ioStatus;
        FILE_STANDARD_INFORMATION information;

        RtlInitUnicodeString(&name, path);
        InitializeObjectAttributes(&attributes, &name, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

        NTSTATUS status = ZwCreateFile(
            &file,
            GENERIC_READ,
            &attributes,
            &ioStatus,
            NULL,
            FILE_ATTRIBUTE_NORMAL,
            0,
            FILE_OPEN,
            FILE_SYNCHRONOUS_IO_NONALERT,
            NULL,
            0);
        if (!NT_SUCCESS(status))
        {
            result = SDP_BIN_OPEN_FAILED;
        }
        else
        {
            status = ZwQueryInformationFile(file, &ioStatus, &information, sizeof(information), FileStandardInformation);
            if (!NT_SUCCESS(status))
            {
                result = SDP_BIN_QUERY_FAILED;
            }
            else
            {
                result = SDP_BIN_OK;
                *Size = (ULONG)information.EndOfFile.QuadPart;
            }
        }
    }

        ExFreePoolWithTag(path, SDP_POOL_TAG_PANELBIN);
    if (file != NULL)
    {
        ZwClose(file);
    }
    return result;
}

// Reads a bin file into Buffer
static ULONG
GetDPP_KMDF(
    _In_ WDFDEVICE Device,
    _Out_writes_bytes_(Size) PVOID Buffer,
    _In_ ULONG Size,
    _In_ PCWSTR Directory,
    _In_ PCWSTR FileName)
{
    ULONG result;
    HANDLE file = NULL;

    if (Buffer == NULL || Size == 0 || Directory == NULL || FileName == NULL)
    {
        return SDP_BIN_BAD_PARAMETER;
    }

    PWCHAR path = (PWCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, SDP_PATH_BYTES, SDP_POOL_TAG_PANELBIN);
    if (path == NULL)
    {
        return SDP_BIN_NO_MEMORY;
    }

    result = GenerateDPPPath_KMDF(Device, path, Directory, FileName);
    if (result == SDP_BIN_OK)
    {
        UNICODE_STRING name;
        OBJECT_ATTRIBUTES attributes;
        IO_STATUS_BLOCK ioStatus;

        RtlInitUnicodeString(&name, path);
        InitializeObjectAttributes(&attributes, &name, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

        NTSTATUS status = ZwCreateFile(
            &file,
            GENERIC_READ,
            &attributes,
            &ioStatus,
            NULL,
            FILE_ATTRIBUTE_NORMAL,
            0,
            FILE_OPEN,
            FILE_SYNCHRONOUS_IO_NONALERT,
            NULL,
            0);
        if (!NT_SUCCESS(status))
        {
            result = SDP_BIN_OPEN_FAILED;
        }
        else
        {
            status = ZwReadFile(file, NULL, NULL, NULL, &ioStatus, Buffer, Size, NULL, NULL);
            result = NT_SUCCESS(status) ? SDP_BIN_OK : SDP_BIN_READ_FAILED;
        }
    }

        ExFreePoolWithTag(path, SDP_POOL_TAG_PANELBIN);
    if (file != NULL)
    {
        ZwClose(file);
    }
    return result;
}

// Reads the bin file of each panel and resets the per-panel runtime state
// A missing bin is not an error.
NTSTATUS
InitializePanelContext(_In_ PDEVICE_CONTEXT Device)
{
    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN ok = TRUE;
    ULONG size = 0;

    for (ULONG i = 0; i < SDP_PANEL_COUNT; i++)
    {
        PPANEL_CONTEXT panel = &Device->Panels[i];

        ULONG binStatus = GetDPPItemSize_KMDF(panel->Device, &size, g_PanelBinDirectory, g_PanelBinNames[i]);
        if (size == 0 || binStatus != SDP_BIN_OK)
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Panel bin %u size query failed (%u)", i, binStatus);
            panel->PanelBin = NULL;
            ok = FALSE;
        }

        if (ok)
        {
            WDF_OBJECT_ATTRIBUTES attributes;
            WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
            attributes.ParentObject = panel->Device;

            WDFMEMORY memory;
            status = WdfMemoryCreate(
                &attributes, NonPagedPoolNx, SDP_POOL_TAG, size, &memory, (PVOID*)&panel->PanelBin);
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfMemoryCreate for the panel bin failed %!STATUS!", status);
                panel->PanelBin = NULL;
                size = 0;
            }
            else
            {
                binStatus = GetDPP_KMDF(panel->Device, panel->PanelBin, size, g_PanelBinDirectory, g_PanelBinNames[i]);
                if (binStatus != SDP_BIN_OK)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "Reading panel bin %u failed (%u)", i, binStatus);
                    WdfObjectDelete(memory);
                    panel->PanelBin = NULL;
                    size = 0;
                }
            }
        }

        panel->PanelBinSize = size;
        panel->VsyncPeriodUs = 0xFFFFFFFF;
        panel->VsyncSampleTarget = 5;
        panel->VsyncMeasure = FALSE;
        panel->Brightness = 0x7F;
        panel->ThermalPercent = 100;
        panel->ThermalMaxNits = 300000;
        panel->Caps = (Device->MfgModeCurrent == 0) ? SDP_CAPS_NORMAL_MODE : 0;

        status = InitializePanelRuntimeLut(panel, i == 0);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "InitializePanelRuntimeLut for the panel failed %!STATUS!", status);
            panel->RuntimeCalibration.Valid = FALSE;
            status = STATUS_SUCCESS;
        }

        status = ProcessQdcmCalibration(panel, i == 0 ? PANEL_CALIBRATION_DATA_C3_FILE_PATH : PANEL_CALIBRATION_DATA_R2_FILE_PATH);
        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "LoadQdcmCalibrationFromSfpd for the panel failed %!STATUS!", status);
            status = STATUS_SUCCESS;
        }
    }

    return status;
}

// section bounds are checked with the wrong stride and just traced.
NTSTATUS
ValidateColorRequest(_In_ const SDP_CAL_HEADER* Header, _In_ SIZE_T Length)
{
    NTSTATUS status = STATUS_SUCCESS;

    if (Header->DisplayId < 3 || Header->DisplayId == (ULONG)-1)
    {
        for (ULONG i = 0; i < Header->EntryCount; i++)
        {
            const UCHAR* entry = (const UCHAR*)Header + 0x10 + (SIZE_T)i * 0x90;
            const ULONG* fields = (const ULONG*)entry;

            if ((SIZE_T)((const UCHAR*)Header + Length - 0xC) < (SIZE_T)entry)
            {
                status = STATUS_INVALID_PARAMETER;
            }
            if ((ULONGLONG)((const UCHAR*)Header + Length) < (ULONGLONG)fields[1] + fields[2])
            {
                status = STATUS_INVALID_PARAMETER;
            }
        }

        if (!NT_SUCCESS(status))
        {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Calibration section table out of bounds");
        }
        return STATUS_SUCCESS;
    }

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Calibration blob display id %u invalid", Header->DisplayId);
    return STATUS_INVALID_PARAMETER;
}

// Zero commit configuration referenced by the property config (see below).
static QDI_Panel_CommitConfigType g_CommitConfigSync;

// Copies a calibration blob into the panel's calibration buffer and prepares
// the QDI property config that the worker pushes to the display
// The sections are applied in table order; a section whose
// first word is -1 disables the corresponding QDI property.
NTSTATUS
ProcessColorPayload(_In_ PPANEL_CONTEXT Panel, _In_reads_bytes_(Length) PUCHAR Blob, _In_ SIZE_T Length)
{
    NTSTATUS status = STATUS_SUCCESS;
    const SDP_CAL_HEADER* header = (const SDP_CAL_HEADER*)Blob;

    if (Panel == NULL || Blob == NULL || Length < 0x1C)
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Invalid calibration parameters");
        return STATUS_INVALID_PARAMETER;
    }

    if (!NT_SUCCESS(ValidateColorRequest(header, Length)))
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Calibration validation failed");
        return STATUS_INVALID_PARAMETER;
    }

    SDP_CALIBRATION_BUFFER* buffer = (SDP_CALIBRATION_BUFFER*)Panel->Calibration;
    const UCHAR* dataEnd = Blob + header->DataEnd;

    RtlZeroMemory(&Panel->Properties, sizeof(Panel->Properties));

    for (ULONG i = 0; i < header->EntryCount; i++)
    {
        const SDP_CAL_HEADER_ENTRY* entry = &header->Entries[i];
        const UCHAR* payload = Blob + entry->Offset;

        if (dataEnd < payload)
        {
            return STATUS_INVALID_PARAMETER;
        }

        switch (entry->Type)
        {
            case SDP_CAL_SECTION_GAMUT:
                if (entry->Size != SDP_CAL_SIZE_GAMUT)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Gamut section has the wrong size");
                    return STATUS_INVALID_PARAMETER;
                }
                if (RtlCompareMemory(&buffer->GamutPayload, payload, entry->Size) != entry->Size)
                {
                    RtlCopyMemory(&buffer->GamutPayload, payload, entry->Size);
                    if (buffer->GamutPayload.Id == -1)
                    {
                        Panel->Properties.pGamutMappingTbl = NULL;
                        return status;
                    }

                    buffer->Gamut.eDisplayId = (QDI_Display_IDType)Panel->DisplayId;
                    buffer->Gamut.bEnabled = buffer->GamutPayload.Enabled;
                    buffer->Gamut.bGamutBeforePCCReserved = buffer->GamutPayload.GamutBeforePcc;
                    buffer->Gamut.uNumSamplesPerColor = SDP_CAL_GAMUT_SAMPLES;
                    buffer->Gamut.pRedCr = buffer->GamutPayload.Red;
                    buffer->Gamut.pGreenY = buffer->GamutPayload.Green;
                    buffer->Gamut.pBlueCb = buffer->GamutPayload.Blue;
                    Panel->Properties.pGamutMappingTbl = &buffer->Gamut;
                    buffer->DirtyFlags |= SDP_CAL_DIRTY_GAMUT_MAPPING;
                }
                break;

            case SDP_CAL_SECTION_INV_GAMMA:
                if (entry->Size != SDP_CAL_SIZE_INV_GAMMA)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Inverse gamma section has the wrong size");
                    return STATUS_INVALID_PARAMETER;
                }
                if (RtlCompareMemory(&buffer->InverseGamma, payload, entry->Size) != entry->Size)
                {
                    RtlCopyMemory(&buffer->InverseGamma, payload, entry->Size);
                    if ((LONG)buffer->InverseGamma.eDisplayId == -1)
                    {
                        Panel->Properties.pInverseGammaCorrectionTbl = NULL;
                        return status;
                    }

                    Panel->Properties.pInverseGammaCorrectionTbl = &buffer->InverseGamma;
                    buffer->DirtyFlags |= SDP_CAL_DIRTY_INVERSE_GAMMA;
                }
                break;

            case SDP_CAL_SECTION_LIN_GAMMA:
                if (entry->Size != SDP_CAL_SIZE_LIN_GAMMA)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Linear gamma section has the wrong size");
                    return STATUS_INVALID_PARAMETER;
                }
                if (RtlCompareMemory(&buffer->LinearGammaPayload, payload, entry->Size) != entry->Size)
                {
                    RtlCopyMemory(&buffer->LinearGammaPayload, payload, entry->Size);

                                        if ((LONG)buffer->InverseGamma.eDisplayId == -1)
                    {
                        Panel->Properties.pLinearGammaCorrectionTbl = NULL;
                        return status;
                    }

                    buffer->LinearGamma.eDisplayId = (QDI_Display_IDType)Panel->DisplayId;
                    buffer->LinearGamma.bEnable = buffer->LinearGammaPayload.Enable;
                    buffer->LinearGamma.uNumCoefficients = buffer->LinearGammaPayload.NumCoefficients;
                    buffer->LinearGamma.uCoefficientSizeInBytes = 2;
                    buffer->LinearGamma.pRedCr = buffer->LinearGammaPayload.Red;
                    buffer->LinearGamma.pGreenY = buffer->LinearGammaPayload.Green;
                    buffer->LinearGamma.pBlueCb = buffer->LinearGammaPayload.Blue;
                    Panel->Properties.pLinearGammaCorrectionTbl = &buffer->LinearGamma;
                    buffer->DirtyFlags |= SDP_CAL_DIRTY_LINEAR_GAMMA;
                }
                break;

            case SDP_CAL_SECTION_CCM:
                if (entry->Size != SDP_CAL_SIZE_CCM)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Colour matrix section has the wrong size");
                    return STATUS_INVALID_PARAMETER;
                }
                if (RtlCompareMemory(&buffer->Ccm, payload, entry->Size) != entry->Size)
                {
                    RtlCopyMemory(&buffer->Ccm, payload, entry->Size);
                    if ((LONG)buffer->Ccm.eDisplayId == -1)
                    {
                        Panel->Properties.pColorCorrectionMatrix = NULL;
                        return status;
                    }

                    Panel->Properties.pColorCorrectionMatrix = &buffer->Ccm;
                    buffer->DirtyFlags |= SDP_CAL_DIRTY_COLOR_MATRIX;
                }
                break;

            case SDP_CAL_SECTION_BRIGHTNESS:
                if (entry->Size != SDP_CAL_SIZE_BRIGHTNESS)
                {
                    TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Brightness section has the wrong size");
                    return STATUS_INVALID_PARAMETER;
                }
                if (buffer->Brightness.Nits != *(const ULONG*)payload)
                {
                    RtlCopyMemory(&buffer->Brightness, payload, SDP_CAL_SIZE_BRIGHTNESS);
                    buffer->DirtyFlags |= SDP_CAL_DIRTY_BRIGHTNESS;
                }
                break;

            default:
                TraceEvents(TRACE_LEVEL_ERROR, TRACE_CALIBRATION, "Unknown calibration section %u", entry->Type);
                break;
        }
    }

        Panel->Properties.pCommitConfig = &g_CommitConfigSync;
    return status;
}

NTSTATUS
ProcessQdcmCalibration(
    _In_ PPANEL_CONTEXT Panel,
    _In_ PCWSTR XmlPath
)
{
    NTSTATUS Status;

    SDP_CALIBRATION_BUFFER* Cal =
        (SDP_CALIBRATION_BUFFER*)
        Panel->Calibration;

    if (Panel == NULL ||
        Cal == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(
        &Panel->Properties,
        sizeof(Panel->Properties));

    Status =
        LoadQdcmCalibrationFromSfpd(
            Panel->Device,
            XmlPath,
            Cal);

    if (!NT_SUCCESS(Status))
    {
        TraceEvents(
            TRACE_LEVEL_ERROR,
            TRACE_DRIVER,
            "LoadQdcmCalibrationFromSfpd failed %!STATUS!",
            Status);

        return Status;
    }

    //
    // CCM
    //
    if ((LONG)Cal->Ccm.eDisplayId != -1)
    {
        Panel->Properties.pColorCorrectionMatrix =
            &Cal->Ccm;

        Cal->DirtyFlags |=
            SDP_CAL_DIRTY_COLOR_MATRIX;
    }

    //
    // IGC
    //
    if ((LONG)Cal->InverseGamma.eDisplayId != -1)
    {
        Panel->Properties.pInverseGammaCorrectionTbl =
            &Cal->InverseGamma;

        Cal->DirtyFlags |=
            SDP_CAL_DIRTY_INVERSE_GAMMA;
    }

    //
    // Linear Gamma
    //
    if (Cal->LinearGamma.bEnable)
    {
        Panel->Properties.pLinearGammaCorrectionTbl =
            &Cal->LinearGamma;

        Cal->DirtyFlags |=
            SDP_CAL_DIRTY_LINEAR_GAMMA;
    }

    //
    // Gamut
    //
    if (Cal->Gamut.bEnabled)
    {
        Panel->Properties.pGamutMappingTbl =
            &Cal->Gamut;

        Cal->DirtyFlags |=
            SDP_CAL_DIRTY_GAMUT_MAPPING;
    }

    //
    // Commit trigger
    //
    Panel->Properties.pCommitConfig =
        &g_CommitConfigSync;

    TraceEvents(
        TRACE_LEVEL_INFORMATION,
        TRACE_DRIVER,
        "QDCM calibration loaded DirtyFlags=0x%08X",
        Cal->DirtyFlags);

    return STATUS_SUCCESS;
}