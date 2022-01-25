#include "Protocols.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *getGraphicsOutputProtocol(IN EFI_HANDLE ImageHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandlesCount=0;
    EFI_HANDLE *Buffer=NULL;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &HandlesCount,
        &Buffer
    );
    if(EFI_ERROR(Status)){
        Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", "GraphicsOutputProtocol", Status);
        return NULL;
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutputProtocol;
    Status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&GraphicsOutputProtocol,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    //Print(L"Status=%d\n",Status);
    if(EFI_ERROR(Status)){
        Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", "GraphicsOutputProtocol", Status);
        return NULL;
    }
    return GraphicsOutputProtocol;
}