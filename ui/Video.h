#include <Library/BaseMemoryLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "ui/BootingUI.h"

#pragma pack(1)
typedef struct
{
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINTN                      FrameBufferSize;
    UINT32                     HorizontalResolution;
    UINT32                     VerticalResolution;
    UINT32                     PixelsPerScanLine;
}VideoConfig;
#pragma pack()

//VideoConfig *videoConfig;

EFI_STATUS
EFIAPI
initVideo(IN EFI_HANDLE ImageHandle, EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol, VideoConfig *videoConfig);

EFI_STATUS setResolution(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *protocol,
    IN EFI_HANDLE ImageHandle);