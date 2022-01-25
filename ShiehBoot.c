#include "ShiehBoot.h"

EFI_STATUS
EFIAPI
BootMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol=
    getGraphicsOutputProtocol(ImageHandle);
    //tryToReadFile(ImageHandle);


    VideoConfig videoConfig;
    initVideo(ImageHandle,graphicsOutputProtocol,&videoConfig);
    //drawLogo(ImageHandle);
    
    return 0;
}

EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle){

    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE *SimpleFileSystemBuffer=NULL;
    UINTN HandlersCount=0;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandlersCount,
        &SimpleFileSystemBuffer
    );//视频真机测试，SimpleFileSystemBuffer会返回两个，首个就是efi文件所在的盘
    if(EFI_ERROR(Status)){
        Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    Status=gBS->OpenProtocol(
        SimpleFileSystemBuffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if(EFI_ERROR(Status)){
        Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }
    EFI_FILE_PROTOCOL *file=NULL;
    Status=FileSystem->OpenVolume(FileSystem,&file);
    
    EFI_FILE_PROTOCOL *iconFile=NULL;
    Status=file->Open(file,&iconFile,BOOT_ICON,EFI_FILE_MODE_READ,EFI_FILE_ARCHIVE);




    if(EFI_ERROR(Status)){
        if(statusCodeEqualsTo(Status,14)){
            Print(L"Error: Failed to open file: \"%s\" (not found).\n", BOOT_ICON);
        }else{
            Print(L"Error: Failed to open file: \"%s\" Status: %d\n", BOOT_ICON, Status);
        }
    }
    return Status;

}