#include "XBL.h"

EFI_STATUS
EFIAPI
BootMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol =
            getGraphicsOutputProtocol(ImageHandle);
    //tryToReadFile(ImageHandle);


    VideoConfig videoConfig;
    initVideo(ImageHandle, graphicsOutputProtocol, &videoConfig);

    //drawLogo(ImageHandle, graphicsOutputProtocol);
    //for (int i = 1; i < 11;i++){
    //}

    UINTN Count;
    BOOTABLE *bootables = NULL;
    GetBootables(ImageHandle, &bootables, &Count);

    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->ClearScreen(gST->ConOut);


    EFI_STATUS Status;
    UINTN EventIndex;
    EFI_INPUT_KEY Key;
    UINTN Choose = 0;
    UINTN ChoosableCount = Count + 2;

    INT32 FirstIndexRow = -1;
    UINTN OldChosenEntry = Choose;

    do {
        if (FirstIndexRow != -1) {
            //
            // Incrementally update menu
            //
            if ((OldChosenEntry >= 0) && (OldChosenEntry != Choose)) {
                gST->ConOut->SetCursorPosition(gST->ConOut, 2, 2 + OldChosenEntry);
                gST->ConOut->OutputString(gST->ConOut, L" ");
            }

            gST->ConOut->SetCursorPosition(gST->ConOut, 2, 2 + Choose);
            gST->ConOut->OutputString(gST->ConOut, L"-");
        } else {

            gST->ConOut->ClearScreen(gST->ConOut);
            gST->ConOut->OutputString(gST->ConOut, L"XBL Boot Loader Boot Menu\r\n\r\n");

            FirstIndexRow = gST->ConOut->Mode->CursorRow;

            for (int i = 0; i < Count; i++) {
                if (i == Choose) {
                    Print(L"  - ");
                } else {
                    Print(L"    ");
                }
                Print(L"%d. %s\n", i + 1, bootables[i].PartitionName);
            }

            if (Choose == Count) {
                Print(L"  -    %s\n", L"Reboot");
            } else {
                Print(L"       %s\n", L"Reboot");
            }

            if (Choose == Count + 1) {
                Print(L"  -    %s\n", L"Power Off");
            } else {
                Print(L"       %s\n", L"Power Off");
            }
        }


        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        ZeroMem(&Key, sizeof(Key));
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (EFI_ERROR (Status)) {
            if (Status == EFI_NOT_READY) {
                continue;
            }
            break;
        }
        UINT16 ScanCode = Key.ScanCode;
        CHAR16 UnicodeChar = Key.UnicodeChar;
        if (UnicodeChar == 0x000d) {
            if (Choose == Count) {
                gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
            } else if (Choose == Count + 1) {
                gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            } else if (Choose < Count) {
                gST->ConOut->ClearScreen(gST->ConOut);
                BOOTABLE bootable = bootables[Choose];
                loadKernel(ImageHandle, &videoConfig, bootable.FileSystemProtocol, bootable.ElfFileProtocol);
                while (1) {
                }
                break;
            }
            continue;
        }

        OldChosenEntry = Choose;
        if (ScanCode == 0x0001) {
            if (Choose > 0) {
                Choose--;
            }
            continue;
        } else if (ScanCode == 0x0002) {
            if (Choose + 1 < ChoosableCount) {
                Choose++;
            }
            continue;
        }
    } while (TRUE);

    return EFI_SUCCESS;
}

void GetBootables(
        IN EFI_HANDLE ImageHandle,
        OUT BOOTABLE **bootables,
        OUT UINTN *bootablesCount) {
    UINTN Count;
    EFI_HANDLE *Buffer = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    getAllPartitions(&Count, &Buffer);
    *bootablesCount = 0;
    for (int i = 0; i < Count; i++) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs;
        Status = gBS->OpenProtocol(
                Buffer[i],
                &gEfiSimpleFileSystemProtocolGuid,
                (VOID **) &sfs,
                ImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        if (EFI_ERROR(Status)) {
            continue;
        }
        EFI_FILE_PROTOCOL *fileProtocol = getFileProtocol(sfs, KERNEL, EFI_FILE_MODE_READ, &Status);
        if (EFI_ERROR(Status)) {
            continue;
        }
        BOOTABLE bootable = {
                .PartitionName=GetVolumeLabel(sfs),
                .FileSystemProtocol=sfs,
                .ElfFileProtocol=fileProtocol
        };
        (*bootables)[(*bootablesCount)++] = bootable;

    }
}

/*EFI_STATUS
executeKernel(IN EFI_HANDLE ImageHandle){
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp=getSimpleFileSystemProtocol(ImageHandle);
    EFI_STATUS Status = EFI_SUCCESS;
    if(sfsp==NULL){
        if(0)Print(L"Failed to load kernel: Unable to getSimpleFileSystemProtocol\n");
        return EFI_LOAD_ERROR;
    }
    EFI_FILE_PROTOCOL *file = getFileProtocol(sfsp, SIMPLE_KERNEL, EFI_FILE_MODE_READ, &Status);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Unable to open kernel: Failed to getFileProtocol.\n");
        return Status;
    }
    EFI_PHYSICAL_ADDRESS kernelAddress;
    Status = ReadFile(file, SIMPLE_KERNEL, &kernelAddress);
    if (EFI_ERROR(Status))
    {
        if(0)Print(L"Unable to open kernel: Failed to readFile.\n");
        return Status;
    }
    asm("jmp %0": : "m"(kernelAddress));
    return Status;
}*/

/*EFI_STATUS
EFIAPI
tryToReadFile(IN EFI_HANDLE ImageHandle) {

    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE *SimpleFileSystemBuffer = NULL;
    UINTN HandlersCount = 0;
    Status = gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            &HandlersCount,
            &SimpleFileSystemBuffer
    );//视频真机测试，SimpleFileSystemBuffer会返回两个，首个就是efi文件所在的盘
    if (EFI_ERROR(Status)) {
        if (isPrintSB())
            Print(L"Error: Failed to locate handle buffer: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    Status = gBS->OpenProtocol(
            SimpleFileSystemBuffer[0],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID **) &FileSystem,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
        if (isPrintSB())
            Print(L"Error: Failed to open protocol: \"%s\" Status: %d\n", L"SimpleFileSystemProtocol", Status);
        return Status;
    }
    EFI_FILE_PROTOCOL *file = NULL;
    Status = FileSystem->OpenVolume(FileSystem, &file);

    EFI_FILE_PROTOCOL *iconFile = NULL;
    Status = file->Open(file, &iconFile, BOOT_LOGO, EFI_FILE_MODE_READ, EFI_FILE_ARCHIVE);


    if (EFI_ERROR(Status)) {
        if (statusCodeEqualsTo(Status, 14)) {
            if (isPrintSB())Print(L"Error: Failed to open file: \"%s\" (not found).\n", BOOT_LOGO);
        } else {
            if (isPrintSB())Print(L"Error: Failed to open file: \"%s\" Status: %d\n", BOOT_LOGO, Status);
        }
    }
    return Status;

}*/

int isPrintSB() {
#ifdef PRINT_LOGS
    return 1;
#else
    return 0;
#endif
}