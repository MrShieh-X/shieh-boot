#include "Video.h"

EFI_STATUS
EFIAPI
initVideo(IN EFI_HANDLE ImageHandle, EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol, VideoConfig *videoConfig) {
    if (isPrint())Print(L"Start to init video\n");
    EFI_STATUS Status = EFI_SUCCESS;
    Status = setResolution(graphicsOutputProtocol, ImageHandle);
    addProgress(graphicsOutputProtocol);

    if (isPrint())Print(L"Video: Writing video configurations to VideoConfig...\n");
    videoConfig->FrameBufferBase = graphicsOutputProtocol->Mode->FrameBufferBase;
    videoConfig->FrameBufferSize = graphicsOutputProtocol->Mode->FrameBufferSize;
    videoConfig->HorizontalResolution = graphicsOutputProtocol->Mode->Info->HorizontalResolution;
    videoConfig->VerticalResolution = graphicsOutputProtocol->Mode->Info->VerticalResolution;
    videoConfig->PixelsPerScanLine = graphicsOutputProtocol->Mode->Info->PixelsPerScanLine;

    addProgress(graphicsOutputProtocol);
    if (isPrint())Print(L"Video: FrameBufferSize: %d\n", videoConfig->FrameBufferSize);
    if (isPrint())Print(L"Video: HorizontalResolution: %d\n", videoConfig->HorizontalResolution);
    if (isPrint())Print(L"Video: VerticalResolution: %d\n", videoConfig->VerticalResolution);
    if (isPrint())Print(L"Video: PixelsPerScanLine: %d\n", videoConfig->PixelsPerScanLine);

    return Status;
}

EFI_STATUS setResolution(IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Protocol, IN EFI_HANDLE ImageHandle) {
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION * ModeInfo;
    UINTN ModeInfoSize = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
    UINTN H = 0;
    UINTN V = 0;
    UINTN ModeIndex = 0;


    if (isPrint())Print(L"Video: Looking for a appropriate screen resolution...\n");

    for (UINTN i = 1; i < Protocol->Mode->MaxMode; i++) {
        Status = Protocol->QueryMode(Protocol, i, &ModeInfoSize, &ModeInfo);
        H = ModeInfo->HorizontalResolution;
        V = ModeInfo->VerticalResolution;
        if (/*((H == 1920) && (V == 1080)) ||*/ ((H == 1440) && (V == 900))) {
            ModeIndex = i;
            break;
        }
    }

    if (H == Protocol->Mode->Info->HorizontalResolution && V == Protocol->Mode->Info->VerticalResolution) {
        if (isPrint())Print(L"Video: No need to modify screen resolution.\n");
        return EFI_SUCCESS;
    }

    Status = Protocol->SetMode(Protocol, ModeIndex);

    if (EFI_ERROR(Status)) {
        if (isPrint())
            Print(L"Video: Error: Failed to set resolution. Index: %d, width: %d, height: %d, status: %d\n", ModeIndex,
                  H, V, Status);
        return Status;
    }
    if (isPrint())Print(L"Video: Set screen resolution to index: %d, width: %d, height: %d.\n", ModeIndex, H, V);

    return Status;
}