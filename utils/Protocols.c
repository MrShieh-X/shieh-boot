#include "Protocols.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *getGraphicsOutputProtocol(
        IN EFI_HANDLE ImageHandle
) {
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandlesCount = 0;
    EFI_HANDLE *Buffer = NULL;
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            &HandlesCount,
            &Buffer
    );
    if (EFI_ERROR(Status)) {
        if (isPrint())
            Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", "GraphicsOutputProtocol", Status);
        return NULL;
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutputProtocol;
    Status = gBS->OpenProtocol(
            Buffer[0],
            &gEfiGraphicsOutputProtocolGuid,
            (VOID **) &GraphicsOutputProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    //if(0)Print(L"Status=%d\n",Status);
    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", "GraphicsOutputProtocol", Status);
        return NULL;
    }
    return GraphicsOutputProtocol;
}

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *getSimpleFileSystemProtocol(
        IN EFI_HANDLE ImageHandle
) {
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandlesCount = 0;
    EFI_HANDLE *Buffer = NULL;
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &HandlesCount,
            &Buffer);
    if (EFI_ERROR(Status)) {
        if (isPrint())
            Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", "SimpleFileSystemProtocol", Status);
        return NULL;
    }


    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs;
    Status = gBS->OpenProtocol(
            Buffer[5],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID **) &sfs,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    //if(0)Print(L"Status=%d\n",Status);
    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", "SimpleFileSystemProtocol", Status);
        return NULL;
    }
    return sfs;
}

/**
 * @brief Get the File Protocol object (Root->Open)
 * 
 * @param SimpleFileSystemProtocol 通过getSimpleFileSystemProtocol
 * @param FileName 前面要加“L”
 * @param OpenMode EFI_FILE_MODE_READ, EFI_FILE_MODE_WRITE, EFI_FILE_MODE_CREATE
 * @return EFI_FILE_PROTOCOL* 
 */
EFI_FILE_PROTOCOL *getFileProtocol(
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemProtocol,
        CHAR16 *FileName,
        UINT64 OpenMode,
        UINTN *Status) {
    EFI_FILE_PROTOCOL *Root;

    //if(0)Print(L"File: Opening the volume for file: %s\n",FileName);
    *Status = SimpleFileSystemProtocol->OpenVolume(
            SimpleFileSystemProtocol,
            &Root);
    if (EFI_ERROR(*Status)) {
        if (isPrint())Print(L"Error: Failed to open volume for file: %s. Status: %d\n", FileName, *Status);
        return NULL;
    }
    //if(0)Print(L"Opened the volume\n");

    //if(0)Print(L"File: Opening the file: %s\n",FileName);
    EFI_FILE_PROTOCOL *FileProtocol;
    *Status = Root->Open(
            Root,
            &FileProtocol,
            FileName,
            OpenMode /*EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE*/,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(*Status)) {
        if (statusCodeEqualsTo(*Status, 14)) {
            if (isPrint())Print(L"Error: Failed to open file: %s (not found).\n", FileName);
        } else {
            if (isPrint())Print(L"Error: Failed to open file: %s, Status: %d\n", FileName, *Status);
        }
        return NULL;
    }

    return FileProtocol;
}