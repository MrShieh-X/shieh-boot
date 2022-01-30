#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "utils/Protocols.h"
#include "constants/ConstantFiles.h"
#include "utils/File.h"

#define GAP 1
#define MAX_PROGRESS 10

typedef struct
{
    UINT64 PixelStart;
    UINTN Size;
    UINTN PageSize;
    UINTN Width;
    UINTN Height;
    UINTN Offset;
} BMPConfig;

EFI_STATUS addProgress(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop);
    
EFI_STATUS drawLogo(
    IN EFI_HANDLE ImageHandle,
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);

EFI_STATUS BmpTransform(
    IN EFI_PHYSICAL_ADDRESS BmpBase,
    OUT BMPConfig *BmpConfig,
    IN CHAR16 *FileName);

EFI_STATUS DrawBmp(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
    IN BMPConfig BmpConfig,
    IN UINTN X,
    IN UINTN Y);