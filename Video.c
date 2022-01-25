#include "Video.h"

EFI_STATUS
EFIAPI
initVideo(IN EFI_HANDLE ImageHandle, EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol, VideoConfig *config)
{
   Print(L"Start to init video\n");
   EFI_STATUS Status = EFI_SUCCESS;
   Status = setResolution(graphicsOutputProtocol,ImageHandle);

   Print(L"Video: Writing video configurations to VideoConfig...\n");
   config->FrameBufferBase = graphicsOutputProtocol->Mode->FrameBufferBase;
   config->FrameBufferSize = graphicsOutputProtocol->Mode->FrameBufferSize;
   config->HorizontalResolution = graphicsOutputProtocol->Mode->Info->HorizontalResolution;
   config->VerticalResolution = graphicsOutputProtocol->Mode->Info->VerticalResolution;
   config->PixelsPerScanLine = graphicsOutputProtocol->Mode->Info->PixelsPerScanLine;
   Print(L"Video: FrameBufferSize: %d\n", config->FrameBufferSize);
   Print(L"Video: HorizontalResolution: %d\n", config->HorizontalResolution);
   Print(L"Video: VerticalResolution: %d\n", config->VerticalResolution);
   Print(L"Video: PixelsPerScanLine: %d\n", config->PixelsPerScanLine);

   return Status;
}

EFI_STATUS setResolution(IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Protocol, IN EFI_HANDLE ImageHandle)
{
   EFI_STATUS Status = EFI_SUCCESS;

   EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo;
   UINTN ModeInfoSize = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
   UINTN H = 0;
   UINTN V = 0;
   UINTN ModeIndex = 0;


   Print(L"Video: Looking for a appropriate screen resolution...\n");

   for (UINTN i = 1; i < Protocol->Mode->MaxMode; i++)
   {
      Status = Protocol->QueryMode(Protocol, i, &ModeInfoSize, &ModeInfo);
      H = ModeInfo->HorizontalResolution;
      V = ModeInfo->VerticalResolution;
      if (((H == 1024) && (V == 768)) || ((H == 1440) && (V == 900)))
      {
         ModeIndex = i;
         break;
      }
   }

   Status = Protocol->SetMode(Protocol, ModeIndex);

   if (EFI_ERROR(Status)){
      //Print(L"Video: Error: Failed to set resolution. Index: %d, width: %d, height: %d, status: %d\n", ModeIndex, H, V, Status);
      return Status;
   }
   Print(L"Video: Set Resolution to index: %d, width: %d, height: %d.\n", ModeIndex, H, V);

   return Status;
}