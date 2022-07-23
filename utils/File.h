#ifndef __UTILS_FILE_H__
#define __UTILS_FILE_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#define PRINT_LOGS

EFI_STATUS ReadFile(
        IN EFI_FILE_PROTOCOL *FileProtocol,
        IN CHAR16 *FileName,
        OUT EFI_PHYSICAL_ADDRESS *FileBase,
        OUT UINTN *FileSize);

int isPrintF();

#endif