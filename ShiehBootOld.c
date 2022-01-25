#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
BootMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN NoHandles=0;
    EFI_HANDLE *Buffer=NULL;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &NoHandles,
        &Buffer
    );
    Print(L"Status = %d.\n",Status);
    if(EFI_ERROR(Status)){
        Print(L"Failed to LocateHandleBuffer. The handle is \"%s\" Status: %d\n", "gEfiGraphicsOutputProtocol", Status);
        return Status;
    }
    Print(L"Hello World!\n");
    Print(L"NumberHandles=%d\n",NoHandles);
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutputProtocol;
    Status=gBS->OpenProtocol(
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&GraphicsOutputProtocol,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    Print(L"Status=%d\n",Status);
    if(EFI_ERROR(Status)){
        Print(L"Failed to OpenProtocol. The handle is \"%s\" Status: %d\n", "gEfiGraphicsOutputProtocol", Status);
        return Status;
    }

    UINTN SizeOfInfo=0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    for(UINTN i=0;i<GraphicsOutputProtocol->Mode->MaxMode;i++){
        Status=GraphicsOutputProtocol->QueryMode(
            GraphicsOutputProtocol,
            i,
            &SizeOfInfo,
            &Info
        );
        if(EFI_ERROR(Status)){
            Print(L"Failed to querymode. Status: %d\n", Status);
            return Status;
        }
        Print(L"Mode %d, H=%d, V=%d.\n",i,Info->HorizontalResolution,Info->VerticalResolution);
    }


    /*Status=GraphicsOutputProtocol->SetMode(GraphicsOutputProtocol,22*//*1920*1080*//*);
    if(EFI_ERROR(Status)){
        Print(L"Failed to set resolution. Status: %d\n", Status);
         return Status;
    }*/
    //if(1)return 0;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Blue={
        .Blue=255,
        .Green=0,
        .Red=0,
        .Reserved=0};//视频的是简略写法：{255,0,0,0}，且视频里是红色
    Status=GraphicsOutputProtocol->Blt(
        GraphicsOutputProtocol,
        &Blue,
        EfiBltVideoFill,
        0,0,
        0,0,
        100,100,
        0
    );
    if(EFI_ERROR(Status)){
        Print(L"Failed to draw. Status: %d\n", Status);
        return Status;
    }









    
    EFI_HANDLE *SimpleFileSystemBuffer=NULL;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &NoHandles,
        &SimpleFileSystemBuffer
    );//视频真机测试，SimpleFileSystemBuffer会返回两个，首个就是efi文件所在的盘
    if(EFI_ERROR(Status)){
        Print(L"Failed to LocateHandleBuffer. The handle is \"%s\" Status: %d\n", "gEfiSimpleFileSystemProtocol", Status);
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
        Print(L"Failed to OpenProtocol. The handle is \"%s\" Status: %d\n", "gEfiSimpleFileSystemProtocol", Status);
        return Status;
    }
    Status=FileSystem->OpenVolume(NULL,NULL);
    //volume.Root();



    
    return Status;
}