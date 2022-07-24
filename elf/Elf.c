#include "Elf.h"

EFI_STATUS loadKernel(
        IN EFI_HANDLE ImageHandle,
        IN VideoConfig *videoConfig,
        IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystemProtocol,
        IN EFI_FILE_PROTOCOL *ElfFileProtocol) {
    EFI_PHYSICAL_ADDRESS KernelEntryPoint;
    EFI_STATUS Status = Relocate(ImageHandle, &KernelEntryPoint,ElfFileProtocol);
    if (EFI_ERROR(Status)) {
        return Status;
    }


    BMPConfig asciiaa = getAscii(ImageHandle,FileSystemProtocol);
    MEMORY_MAP memoryMap;
    BootConfig bootConfig = {//.FrameBufferBase = videoConfig->FrameBufferBase,
            //.FrameBufferSize = videoConfig->FrameBufferSize,
            //.HorizontalResolution = videoConfig->HorizontalResolution,
            //.VerticalResolution = videoConfig->VerticalResolution,
            //.AsciiPixelStart=asciiaa.PixelStart,
            .videoConfig = *videoConfig,
            .AsciiBmp = &asciiaa,
            .memoryMap=memoryMap};

    /*for (UINT64 i; i <  640;i++)
    {
        UINT64 *start;
    *start    = bootConfig.AsciiBmp.PixelStart + 6 * 640 + i;
        Print("%d,",((lo)(*start)));
    }*/

    //Print(L"AsciiAddress: %lld\n", bootConfig.AsciiBmp->PixelStart);

    {
        UINTN Index;
        Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);

        EFI_INPUT_KEY Key;
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

    }

    bootConfig.memoryMap.BufferSize = 4096;
    bootConfig.memoryMap.Buffer = NULL;
    bootConfig.memoryMap.MapSize = 4096;
    bootConfig.memoryMap.MapKey = 0;
    bootConfig.memoryMap.DescriptorSize = 0;
    bootConfig.memoryMap.DescriptorVersion = 0;
    Status = ExitBootServices(ImageHandle, &bootConfig.memoryMap);

    if (isPrint()) Print(L"Executing kernel...\n");

    UINT64(*KernelEntry)(BootConfig * bootConfig) = (UINT64(*)(BootConfig *bootConfig)) KernelEntryPoint;
    UINT64 x = KernelEntry(&bootConfig);

    Print(L"Returned value from kernel: %d\n", x);
    return EFI_SUCCESS;
}

EFI_STATUS ExitBootServices(EFI_HANDLE ImageHandle, OUT MEMORY_MAP *MemoryMap) {
    EFI_STATUS Status = EFI_SUCCESS;
    Status = gBS->AllocatePool(EfiLoaderData, MemoryMap->BufferSize, &MemoryMap->Buffer);
    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Failed to allocate memory to get memory map. Status: %d\n", Status);
        return Status;
    }
    Status = gBS->GetMemoryMap(
            &MemoryMap->MapSize,
            (EFI_MEMORY_DESCRIPTOR *) MemoryMap->Buffer,
            &MemoryMap->MapKey,
            &MemoryMap->DescriptorSize,
            &MemoryMap->DescriptorVersion);

    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Failed to get memory map. Status: %d\n", Status);
        return Status;
    }

    Status = gBS->ExitBootServices(ImageHandle, MemoryMap->MapKey);
    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Failed to exit boot services. Status: %d\n", Status);
    }
    return Status;
}

BMPConfig getAscii(
        IN EFI_HANDLE ImageHandle,
        IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystemProtocol) {

    EFI_STATUS Status = EFI_SUCCESS;
    //EFI_STATUS Status;
    EFI_FILE_PROTOCOL *fp = getFileProtocol(FileSystemProtocol, ASCII, EFI_FILE_MODE_READ, &Status);
    EFI_PHYSICAL_ADDRESS address;

    BMPConfig config;

    if ((fp) == NULL) {
        if (isPrint()) Print(L"Unable to read ASCII bmp file: Unable to get file protocol, status: %d\n", Status);
        config.Height = -1;
        config.Width = -1;
        return config;
    }

    UINTN FileSize;
    Status = ReadFile(fp, ASCII, &address, &FileSize);
    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Unable to read ASCII bmp file: Unable to read file\n");
        config.Height = -1;
        config.Width = -1;
        return config;
    }

    Status = BmpTransform(address, &config, ASCII);
    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Unable to read ASCII bmp file: Unable to execute bmp (BmpTransform)\n");
        config.Height = -1;
        config.Width = -1;
        return config;
    }
    return config;
}

EFI_STATUS Relocate(
        IN EFI_HANDLE ImageHandle,
        OUT EFI_PHYSICAL_ADDRESS *KernelEntry,
        IN EFI_FILE_PROTOCOL *ElfFileProtocol
) {
    if (isPrint()) Print(L"Reading kernel...\n");

    EFI_STATUS Status = EFI_SUCCESS;
    /*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = getSimpleFileSystemProtocol(ImageHandle);
    if (sfsp == NULL) {
        if (isPrint()) Print(L"Failed to load kernel: Unable to getSimpleFileSystemProtocol\n");
        return EFI_LOAD_ERROR;
    }
    EFI_FILE_PROTOCOL *FILE = getFileProtocol(sfsp, KERNEL, EFI_FILE_MODE_READ, &Status);
    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Unable to load kernel: Failed to getFileProtocol.\n");
        return Status;
    }*/
    EFI_PHYSICAL_ADDRESS kernelAddress;
    UINTN FileSize;
    Status = ReadFile(ElfFileProtocol, KERNEL, &kernelAddress, &FileSize);
    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Unable to load kernel: Failed to readFile.\n");
        return Status;
    }
    if (isPrint()) Print(L"Checking kernel...\n");

    Status = CheckELF(kernelAddress, FileSize);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    if (isPrint()) Print(L"Loading segments for kernel...\n");

    Status = LoadSegments(kernelAddress, KernelEntry);
    return Status;
}

EFI_STATUS CheckELF(
        IN EFI_PHYSICAL_ADDRESS KernelBuffer,
        IN UINTN KernelFileSize) {
    EFI_STATUS Status = EFI_SUCCESS;

    UINT32 Magic = GetValue(KernelBuffer, 0x00, 4);
    if (Magic != 0x464c457F) {
        if (isPrint()) Print(L"Unable to load kernel: Kernel is not an ELF file!\n");
        Status = NOT_ELF;
        return Status;
    }
    UINT8 Format = GetValue(KernelBuffer, 0x04, 1);
    if (Format != ELF_64) {
        if (isPrint()) Print(L"Unable to load kernel: Kernel is not 64 bit!\n");
        Status = NOT_64_BIT;
        return Status;
    }
    /*UINT8 Sign1 = GetValue(KernelBuffer,KernelFileSize-5,1);
    Print(L"183: %d\n",Sign1);

    if(Sign1!=0x534f6865696853){
        if (isPrint()) Print(L"Unable to load kernel: This kernel is not ShiehOS's!\n");
        Status = NOT_SHIEHOS;
        return Status;
    }
*/

    return Status;
}

EFI_STATUS LoadSegments(
        IN EFI_PHYSICAL_ADDRESS KernelBufferBase,
        OUT EFI_PHYSICAL_ADDRESS *KernelEntry) {
    EFI_STATUS Status = EFI_SUCCESS;
    ELF_HEADER_64 *ElfHeader = (ELF_HEADER_64 *) KernelBufferBase;
    PROGRAM_HEADER_64 *PHeader = (PROGRAM_HEADER_64 *) (KernelBufferBase + ElfHeader->Phoff);

    EFI_PHYSICAL_ADDRESS LowAddr = 0xFFFFFFFFFFFFFFFF;
    EFI_PHYSICAL_ADDRESS HighAddr = 0;

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++) {
        if (PHeader[i].Type == PT_LOAD) {
            if (LowAddr > PHeader[i].PAddress) {
                LowAddr = PHeader[i].PAddress;
            }
            if (HighAddr < (PHeader[i].PAddress + PHeader[i].SizeInMemory)) {
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

    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Error: Failed to allocate pages for kernelRelocateBuffer. Status: %d\n", Status);
        return Status;
    }
    UINT64 RelocateOffset = KernelRelocateBase - LowAddr;
    UINT64 * ZeroStart = (UINT64 *) KernelRelocateBase;
    for (UINTN i = 0; i < (PageCount << 9); i++) {
        *ZeroStart = 0x000000000000;
        ZeroStart++;
    }

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++) {

        if (PHeader[i].Type == PT_LOAD) {
            UINT8 * SourceStart = (UINT8 *) KernelBufferBase + PHeader[i].Offset;
            UINT8 * DestStart = (UINT8 *) PHeader[i].VAddress + RelocateOffset;

            for (UINTN j = 0; j < PHeader[i].SizeInFile; j++) {
                *DestStart = *SourceStart;
                SourceStart++;
                DestStart++;
            }
        }
    }
    *KernelEntry = ElfHeader->Entry + RelocateOffset;

    if (EFI_ERROR(Status)) {
        if (isPrint()) Print(L"Error: Failed to load segments for kernel. Status: %d\n", Status);
    }
    return Status;
}