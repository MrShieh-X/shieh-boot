#include "Elf.h"

EFI_STATUS Relocate(
    IN EFI_HANDLE ImageHandle,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry)
{
    Print(L"Reading kernel...\n");
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = getSimpleFileSystemProtocol(ImageHandle);
    EFI_STATUS Status = EFI_SUCCESS;
    if (sfsp == NULL)
    {
        Print(L"Failed to load kernel: Unable to getSimpleFileSystemProtocol\n");
        return EFI_LOAD_ERROR;
    }
    EFI_FILE_PROTOCOL *file = getFileProtocol(sfsp, KERNEL, EFI_FILE_MODE_READ, &Status);
    if (EFI_ERROR(Status))
    {
        Print(L"Unable to load kernel: Failed to getFileProtocol.\n");
        return Status;
    }
    EFI_PHYSICAL_ADDRESS kernelAddress;
    Status = ReadFile(file, KERNEL, &kernelAddress);
    if (EFI_ERROR(Status))
    {
        Print(L"Unable to load kernel: Failed to readFile.\n");
        return Status;
    }

    Print(L"Checking kernel...\n");

    Status = CheckELF(kernelAddress);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Print(L"Loading segments for kernel...\n");

    Status = LoadSegments(kernelAddress, KernelEntry);
    return Status;
}

EFI_STATUS CheckELF(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINT32 Magic = GetValue(KernelBuffer, 0x00, 4);
    if (Magic != 0x464c457F)
    {
        Print(L"Unable to load kernel: Kernel is not an ELF file!\n");
        Status = NOT_ELF;
    return Status;
    }
    UINT8 Format = GetValue(KernelBuffer, 0x04, 1);
    if (Format != ELF_64)
    {
        Print(L"Unable to load kernel: Kernel is not 64 bit!\n");
        Status = NOT_64_BIT;
    }

    return Status;
}

EFI_STATUS LoadSegments(
    IN EFI_PHYSICAL_ADDRESS KernelBufferBase,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry)
{
    EFI_STATUS Status = EFI_SUCCESS;
    ELF_HEADER_64 *ElfHeader = (ELF_HEADER_64 *)KernelBufferBase;
    PROGRAM_HEADER_64 *PHeader = (PROGRAM_HEADER_64 *)(KernelBufferBase + ElfHeader->Phoff);

    EFI_PHYSICAL_ADDRESS LowAddr = 0xFFFFFFFFFFFFFFFF;
    EFI_PHYSICAL_ADDRESS HighAddr = 0;

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {
        if (PHeader[i].Type == PT_LOAD)
        {
            if (LowAddr > PHeader[i].PAddress)
            {
                LowAddr = PHeader[i].PAddress;
            }
            if (HighAddr < (PHeader[i].PAddress + PHeader[i].SizeInMemory))
            {
                HighAddr = PHeader[i].PAddress + PHeader[i].SizeInMemory;
            }
        }
    }

    UINTN PageCount = ((HighAddr - LowAddr) >> 12) + 1;
    EFI_PHYSICAL_ADDRESS KernelRelocateBase;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderCode,
        PageCount,
        &KernelRelocateBase);

    if (EFI_ERROR(Status))
    {
        Print(L"Error: Failed to allocate pages for kernelRelocateBuffer. Status: %d\n",Status);
        return Status;
    }
    UINT64 RelocateOffset = KernelRelocateBase - LowAddr;
    UINT64 *ZeroStart = (UINT64 *)KernelRelocateBase;
    for (UINTN i = 0; i < (PageCount << 9); i++)
    {
        *ZeroStart = 0x000000000000;
        ZeroStart++;
    }

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {

        if (PHeader[i].Type == PT_LOAD)
        {
            UINT8 *SourceStart = (UINT8 *)KernelBufferBase + PHeader[i].Offset;
            UINT8 *DestStart = (UINT8 *)PHeader[i].VAddress + RelocateOffset;

            for (UINTN j = 0; j < PHeader[i].SizeInFile; j++)
            {
                *DestStart = *SourceStart;
                SourceStart++;
                DestStart++;
            }
        }
    }
    *KernelEntry = ElfHeader->Entry + RelocateOffset;


    if (EFI_ERROR(Status))
    {
        Print(L"Error: Failed to load segments for kernel. Status: %d\n", Status);
    }
    return Status;
}