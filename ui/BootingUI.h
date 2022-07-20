#ifndef __UI_BOOTING_UI_H__
#define __UI_BOOTING_UI_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "utils/Protocols.h"
#include "constants/ConstantFiles.h"
#include "utils/File.h"

#define GAP 1
#define MAX_PROGRESS 10

#pragma pack(1)
typedef struct {
    UINTN Size;
    UINTN PageSize;
    UINTN Width;
    UINTN Height;
    UINTN Offset;
    UINT64 PixelStart;
} BMPConfig;

#pragma pack()

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

#endif