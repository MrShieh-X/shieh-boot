#ifndef __UTILS_UTILS_H__
#define __UTILS_UTILS_H__

#include <Uefi.h>

#define PRINT_LOGS

int statusCodeEqualsTo(int statusCode, int code);

UINTN GetValue(
        IN EFI_PHYSICAL_ADDRESS StartAddress,
        IN UINTN Offset,
        IN UINTN Size);

int isPrint();

#endif