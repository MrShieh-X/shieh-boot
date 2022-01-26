#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS ReadFile(
    IN EFI_FILE_PROTOCOL *FileProtocol,
    IN CHAR16 *FileName,
    OUT EFI_PHYSICAL_ADDRESS *FileBase);