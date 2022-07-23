#ifndef __UTILS_PROTOCOLS_H__
#define __UTILS_PROTOCOLS_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "utils/Utils.h"
#include <Protocol/LoadedImage.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL *getGraphicsOutputProtocol(
        IN EFI_HANDLE ImageHandle
);

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *getSimpleFileSystemProtocol(
        IN EFI_HANDLE ImageHandle
);

/**
 * 获得EFI文件所在分区的 SimpleFileSystemProtocol
 **/
EFI_FILE_PROTOCOL *getFileProtocol(
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemProtocol,
        CHAR16 *FileName,
        UINT64 OpenMode,
        UINTN *Status);

#endif