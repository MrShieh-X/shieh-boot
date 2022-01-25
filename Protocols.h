#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL *getGraphicsOutputProtocol(IN EFI_HANDLE ImageHandle);