#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "ConstantFiles.h"
#include "Utils.h"
#include "Protocols.h"
#include "Video.h"

EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle);