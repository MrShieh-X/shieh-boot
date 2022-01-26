#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "constants/ConstantFiles.h"
#include "utils/Utils.h"
#include "utils/Protocols.h"
#include "ui/Video.h"
#include "ui/BootingUI.h"

EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle);