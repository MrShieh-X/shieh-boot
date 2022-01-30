#include "File.h"

EFI_STATUS ReadFile(
    IN EFI_FILE_PROTOCOL *FileProtocol,
    IN CHAR16 *FileName,
    OUT EFI_PHYSICAL_ADDRESS *FileBase)
{
    //if(0)Print(L"File: Reading file: %s\n", FileName);
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_INFO *FileInfo;

    //if(0)Print(L"File: Allocating pool for file info (file: %s)\n", FileName);

    UINTN InfoSize = sizeof(EFI_FILE_INFO) + 128;
    Status = gBS->AllocatePool(
        EfiLoaderData,
        InfoSize,
        (VOID **)&FileInfo);

    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Error: Failed to allocate pool for file info (file: %s). Status: %d\n", FileName, Status);
        return Status;
    }

    Status = FileProtocol->GetInfo(
        FileProtocol,
        &gEfiFileInfoGuid,
        &InfoSize,
        FileInfo);

    //if(0)Print(L"Getting file info of file: %s\n",FileName);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Error: Failed to get the info of file: %s. Status: %d\n",FileName, Status);
        return Status;
    }

    UINTN FilePageSize = (FileInfo->FileSize >> 12) + 1;

    EFI_PHYSICAL_ADDRESS FileBufferAddress;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        FilePageSize,
        &FileBufferAddress);

    //if(0)Print(L"Allocating pages for file: %s\n", FileName);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Error: Failed to allocate pages for file: %s. Status: %d\n",FileName, Status);
        return Status;
    }


    UINTN ReadSize = FileInfo->FileSize;
    Status = FileProtocol->Read(
        FileProtocol,
        &ReadSize,
        (VOID *)FileBufferAddress);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Error: Failed to read file\n");
        return Status;
    }
    if(0)Print(L"File: File %s is read, size=%d.\n", FileName, ReadSize);

    gBS->FreePool(FileInfo);
    *FileBase = FileBufferAddress;
    return Status;
}
