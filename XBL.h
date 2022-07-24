#ifndef __XBL_H__
#define __XBL_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "constants/ConstantFiles.h"
#include "utils/Utils.h"
#include "utils/Protocols.h"
#include "utils/Part.h"
//#include "ui/Video.h"
#include "elf/Elf.h"
#include <Library/ResetSystemLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define PRINT_LOGS


/*EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle);

EFI_STATUS
executeKernel(IN EFI_HANDLE ImageHandle);*/

void GetBootables(
        IN EFI_HANDLE ImageHandle,
        OUT BOOTABLE **bootables,
        OUT UINTN *bootablesCount);

int isPrintSB();

#endif