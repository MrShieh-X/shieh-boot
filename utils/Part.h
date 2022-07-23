#ifndef __UTILS_PART_H__
#define __UTILS_PART_H__

#include <Uefi.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>

#include <Protocol/SimpleFileSystem.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <string.h>

#if defined (__has_builtin)
#if __has_builtin (__builtin_add_overflow) && __has_builtin (__builtin_sub_overflow) && __has_builtin (__builtin_mul_overflow)
#define HAS_TYPE_GENERIC_BUILTINS  1
#endif
#endif

#if defined (__GNUC__) || defined (__clang__)
#if defined (MDE_CPU_AARCH64) || defined (MDE_CPU_X64)
#define HAS_TYPE_SPECIFIC_BUILTINS_64  1
#elif defined (MDE_CPU_ARM) || defined (MDE_CPU_IA32)
#define HAS_TYPE_SPECIFIC_BUILTINS_32  1
#endif

#define HAS_TYPE_SPECIFIC_BUILTINS  1
#endif

#define MAX_VOLUME_LABEL_SIZE  64

EFI_STATUS
getAllPartitions(
        OUT UINTN *Count,
        OUT EFI_HANDLE **Buffer
);

BOOLEAN
OverflowAddU64(
        UINT64 A,
        UINT64 B,
        UINT64 *Result
);

BOOLEAN
OverflowAddUN(
        UINTN A,
        UINTN B,
        UINTN *Result
);

BOOLEAN
OverflowAddU32(
        UINT32 A,
        UINT32 B,
        UINT32 *Result
);

VOID
UnicodeFilterString(
        IN OUT CHAR16 *String,
        IN     BOOLEAN SingleLine
);

CHAR16 *
GetVolumeLabel(
        IN     EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem
);

VOID *
GetFileInfo(
        IN  EFI_FILE_PROTOCOL *File,
        IN  EFI_GUID *InformationType,
        IN  UINTN MinFileInfoSize,
        OUT UINTN *RealFileInfoSize  OPTIONAL
);

BOOLEAN
EFIAPI
CompareGuid2(
        IN  CONST GUID *Guid1,
        IN  CONST GUID *Guid2
);

INTN
EFIAPI
CompareMem2(
        IN  CONST VOID *DestinationBuffer,
        IN  CONST VOID *SourceBuffer,
        IN  UINTN Length
);

#endif