#include <Uefi.h>

int statusCodeEqualsTo(int statusCode, int code);

UINTN GetValue(
    IN EFI_PHYSICAL_ADDRESS StartAddress,
    IN UINTN Offset,
    IN UINTN Size);