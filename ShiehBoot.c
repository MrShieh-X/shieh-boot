#include "ShiehBoot.h"

EFI_STATUS
EFIAPI
BootMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol =
            getGraphicsOutputProtocol(ImageHandle);
    //tryToReadFile(ImageHandle);

    addProgress(graphicsOutputProtocol);

    VideoConfig videoConfig;
    initVideo(ImageHandle, graphicsOutputProtocol, &videoConfig);

    drawLogo(ImageHandle, graphicsOutputProtocol);
    //for (int i = 1; i < 11;i++){
    //}


    //executeKernel(ImageHandle);
    //EFI_PHYSICAL_ADDRESS KernelEntry;
    //Relocate(ImageHandle, &KernelEntry);
    loadKernel(ImageHandle, &videoConfig, graphicsOutputProtocol);
    return 0;
}

/*EFI_STATUS
executeKernel(IN EFI_HANDLE ImageHandle){
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp=getSimpleFileSystemProtocol(ImageHandle);
    EFI_STATUS Status = EFI_SUCCESS;
    if(sfsp==NULL){
        if(0)Print(L"Failed to load kernel: Unable to getSimpleFileSystemProtocol\n");
        return EFI_LOAD_ERROR;
    }
    EFI_FILE_PROTOCOL *file = getFileProtocol(sfsp, SIMPLE_KERNEL, EFI_FILE_MODE_READ, &Status);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Unable to open kernel: Failed to getFileProtocol.\n");
        return Status;
    }
    EFI_PHYSICAL_ADDRESS kernelAddress;
    Status = ReadFile(file, SIMPLE_KERNEL, &kernelAddress);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Unable to open kernel: Failed to readFile.\n");
        return Status;
    }
    asm("jmp %0": : "m"(kernelAddress));
    return Status;
}*/

/*EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle) {

    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE *SimpleFileSystemBuffer = NULL;
    UINTN HandlersCount = 0;
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &HandlersCount,
            &SimpleFileSystemBuffer
    );//视频真机测试，SimpleFileSystemBuffer会返回两个，首个就是efi文件所在的盘
    if (EFI_ERROR(Status)) {
        if (isPrintSB())
            Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    Status = gBS->OpenProtocol(
            SimpleFileSystemBuffer[0],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID **) &FileSystem,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        if (isPrintSB())
            Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }
    EFI_FILE_PROTOCOL *file = NULL;
    Status = FileSystem->OpenVolume(FileSystem, &file);

    EFI_FILE_PROTOCOL *iconFile = NULL;
    Status = file->Open(file, &iconFile, BOOT_LOGO, EFI_FILE_MODE_READ, EFI_FILE_ARCHIVE);


    if (EFI_ERROR(Status)) {
        if (statusCodeEqualsTo(Status, 14)) {
            if (isPrintSB())Print(L"Error: Failed to open file: \"%s\" (not found).\n", BOOT_LOGO);
        } else {
            if (isPrintSB())Print(L"Error: Failed to open file: \"%s\" Status: %d\n", BOOT_LOGO, Status);
        }
    }
    return Status;

}*/

int isPrintSB() {
#ifdef PRINT_LOGS
    return 1;
#else
    return 0;
#endif
}