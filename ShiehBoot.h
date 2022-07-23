#ifndef __SHIEH_BOOT_H__
#define __SHIEH_BOOT_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "constants/ConstantFiles.h"
#include "utils/Utils.h"
#include "utils/Protocols.h"
#include "utils/Part.h"
//#include "ui/Video.h"
#include "elf/Elf.h"

#define PRINT_LOGS


/*EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle);*/

EFI_STATUS
executeKernel(IN EFI_HANDLE ImageHandle);

int isPrintSB();

#endif