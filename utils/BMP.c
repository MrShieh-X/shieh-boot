#include "BMP.h"

EFI_GRAPHICS_OUTPUT_BLT_PIXEL White = {0xFF, 0xFF, 0xFF, 0};

EFI_STATUS BmpTransform(
        IN EFI_PHYSICAL_ADDRESS BmpBase,
        OUT BMPConfig *BmpConfig,
        IN CHAR16 *FileName) {
    EFI_STATUS Status = EFI_SUCCESS;
    //struct bmp_header *bheader = (struct bmp_header *)BmpBase;
    if (isPrint())Print(L"Reading configurations from bmp file: %s\n", FileName);
    BmpConfig->Size = GetValue(BmpBase, 0x02, 4);
    BmpConfig->PageSize = (BmpConfig->Size >> 12) + 1;
    BmpConfig->Offset = GetValue(BmpBase, 0x0A, 4);

    BmpConfig->Width = GetValue(BmpBase, 0x12, 4);
    BmpConfig->Height = GetValue(BmpBase, 0x16, 4);

    if (isPrint())
        Print(L"%s BMP: size = %08lx, pageSize = %08lx, offset = %08lx, width = %d, height = %d\n", FileName,
              BmpConfig->Size, BmpConfig->PageSize, BmpConfig->Offset, BmpConfig->Width, BmpConfig->Height);

    EFI_PHYSICAL_ADDRESS PixelStart;
    Status = gBS->AllocatePages(
            AllocateAnyPages,
            EfiLoaderData,
            BmpConfig->PageSize,
            &PixelStart);

    if (isPrint())Print(L"Allocating pages for PixelArea\n");

    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Error: Failed to AllocatePages for PixelArea. Status: %d\n", Status);
        return Status;
    }
    //if(0)Print(L"SUCCESS:Memory for PixelArea is ready.\n");


    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelFromFile = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (BmpBase + BmpConfig->Offset +
                                                                                      BmpConfig->Width *
                                                                                      BmpConfig->Height * 4);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelToBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) PixelStart;
    for (UINTN i = 0; i < BmpConfig->Height; i++) {
        PixelFromFile -= BmpConfig->Width;
        for (UINTN j = 0; j < BmpConfig->Width; j++) {
            *PixelToBuffer = *PixelFromFile;
            PixelToBuffer++;
            PixelFromFile++;
        }
        PixelFromFile -= BmpConfig->Width;
    }

    BmpConfig->PixelStart = PixelStart;

    return Status;
}

EFI_STATUS DrawBmp(
        IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
        IN BMPConfig BmpConfig,
        IN UINTN X,
        IN UINTN Y) {
    EFI_STATUS Status = EFI_SUCCESS;

    Status = Gop->Blt(
            Gop,
            (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) BmpConfig.PixelStart,
            EfiBltBufferToVideo,
            0, 0,
            X, Y,
            BmpConfig.Width, BmpConfig.Height, 0);

    if (EFI_ERROR(Status)) {
        if (isPrint())Print(L"Error: Failed to Blt BMP. Status: %d.\n", Status);
        return Status;
    }
    return Status;
}
