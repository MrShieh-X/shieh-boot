#include "BootingUI.h"

EFI_GRAPHICS_OUTPUT_BLT_PIXEL White = {0xFF, 0xFF, 0xFF, 0};

UINTN Progress = 0;
EFI_STATUS addProgress(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
){
    Progress++;
    EFI_STATUS Status = EFI_SUCCESS;

    UINTN BlockWidth = Gop->Mode->Info->HorizontalResolution >> 5;
    UINTN BlockHeight = Gop->Mode->Info->VerticalResolution >> 6;
    UINTN StartX = (Gop->Mode->Info->HorizontalResolution - (BlockWidth + GAP) * MAX_PROGRESS - GAP) / 2;
    UINTN StartY = (Gop->Mode->Info->VerticalResolution * 3) >> 2;

    UINTN X = StartX + (BlockWidth + GAP) * (Progress-1);

    Status = Gop->Blt(Gop, &White, EfiBltVideoFill, 0, 0, X, StartY, BlockWidth, BlockHeight, 0);


    if (EFI_ERROR(Status))
    {
        Print(L"Error: Failed to Blt progress. Progress: %d Status: %d\n", Progress, Status);
        return Status;
    }
    //Print(L"Draw Progress: %d/10.\n", Step);


    return Status;
}

EFI_STATUS drawLogo(IN EFI_HANDLE ImageHandle, 
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
    Print(L"Drawing boot logo...\n");
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simpleFileSystemProtocol=getSimpleFileSystemProtocol(ImageHandle);

    CHAR16 *Logo = BOOT_LOGO;

    UINTN Horizontal = gop->Mode->Info->HorizontalResolution;
    UINTN Vertical = gop->Mode->Info->VerticalResolution;

    if (Horizontal * 3 == Vertical * 4)
    {
        Logo = BOOT_LOGO_4X3;
    }

    EFI_STATUS Status;
    EFI_FILE_PROTOCOL *fp = getFileProtocol(simpleFileSystemProtocol, Logo, EFI_FILE_MODE_READ, &Status);
    EFI_PHYSICAL_ADDRESS LogoAddress;

    if((fp)==NULL){
        Print(L"Unable to draw boot logo: Unable to get file protocol, status: %d\n",Status);
        return Status;
    }

    Status = ReadFile(fp,Logo, &LogoAddress);
    if(EFI_ERROR(Status)){
        Print(L"Unable to draw boot logo: Unable to read file\n");
        return Status;
    }
    addProgress(gop);

    BMPConfig config;
    Status = BmpTransform(LogoAddress, &config, Logo);
    if (EFI_ERROR(Status))
    {
        Print(L"Unable to draw boot logo: Unable to execute bmp (BmpTransform)\n");
        return Status;
    }
    addProgress(gop);

    UINTN X = (Horizontal - config.Width) / 2;
    UINTN Y = (Vertical - config.Height) / 2 - (Vertical / 7);

    Status = DrawBmp(gop, config, X, Y);
    addProgress(gop);

    return Status;
}

EFI_STATUS BmpTransform(
    IN EFI_PHYSICAL_ADDRESS BmpBase,
    OUT BMPConfig *BmpConfig,
    IN CHAR16 *FileName)
{
    EFI_STATUS Status = EFI_SUCCESS;
    //struct bmp_header *bheader = (struct bmp_header *)BmpBase;
    Print(L"Reading configurations from bmp file: %s\n",FileName);
    BmpConfig->Size = GetValue(BmpBase, 0x02, 4);
    BmpConfig->PageSize = (BmpConfig->Size >> 12) + 1;
    BmpConfig->Offset = GetValue(BmpBase, 0x0A, 4);

    BmpConfig->Width = GetValue(BmpBase, 0x12, 4);
    BmpConfig->Height = GetValue(BmpBase, 0x16, 4);

    Print(L"%s BMP: size = %08lx, pageSize = %08lx, offset = %08lx, width = %d, height = %d\n", FileName, BmpConfig->Size, BmpConfig->PageSize, BmpConfig->Offset, BmpConfig->Width, BmpConfig->Height);

    EFI_PHYSICAL_ADDRESS PixelStart;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        BmpConfig->PageSize,
        &PixelStart);

    Print(L"Allocating pages for PixelArea\n");

    if (EFI_ERROR(Status))
    {
        Print(L"Error: Failed to AllocatePages for PixelArea. Status: %d\n",Status);
        return Status;
    }
    //Print(L"SUCCESS:Memory for PixelArea is ready.\n");


    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelFromFile = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)(BmpBase + BmpConfig->Offset + BmpConfig->Width * BmpConfig->Height * 4);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelToBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)PixelStart;
    for (UINTN i = 0; i < BmpConfig->Height; i++)
    {
        PixelFromFile -= BmpConfig->Width;
        for (UINTN j = 0; j < BmpConfig->Width; j++)
        {
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
    IN UINTN Y)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = Gop->Blt(
        Gop,
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)BmpConfig.PixelStart,
        EfiBltBufferToVideo,
        0, 0,
        X, Y,
        BmpConfig.Width, BmpConfig.Height, 0);

    if (EFI_ERROR(Status))
    {
        Print(L"Error: Failed to Blt BMP. Status: %d.\n", Status);
        return Status;
    }
    return Status;
}
