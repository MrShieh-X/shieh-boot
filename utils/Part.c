#include "Part.h"

EFI_STATUS
getAllPartitions(
        OUT UINTN *Count,
        OUT EFI_HANDLE **Buffer
) {
    return gBS->LocateHandleBuffer(
            ByProtocol,
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            Count,
            Buffer);
}


/**

OcGuardLib

Copyright (c) 2018, vit9696

All rights reserved.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
BOOLEAN
OverflowAddU64(
        UINT64 A,
        UINT64 B,
        UINT64 *Result
) {
#if defined (HAS_TYPE_GENERIC_BUILTINS)
    return __builtin_add_overflow (A, B, Result);
#elif defined (HAS_TYPE_SPECIFIC_BUILTINS)
    return __builtin_uaddll_overflow(A, B, Result);
#else
    UINT64  Temp;

    Temp    = A + B;
    *Result = Temp;
    if (Temp >= A) {
        return FALSE;
    }

    return TRUE;
#endif
}


/**

OcGuardLib

Copyright (c) 2018, vit9696

All rights reserved.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
BOOLEAN
OverflowAddUN(
        UINTN A,
        UINTN B,
        UINTN *Result
) {
#if defined (HAS_TYPE_GENERIC_BUILTINS)
    return __builtin_add_overflow (A, B, Result);
#elif defined (HAS_TYPE_SPECIFIC_BUILTINS_64)
    return __builtin_uaddll_overflow(A, B, Result);
#elif defined (HAS_TYPE_SPECIFIC_BUILTINS_32)
    return __builtin_uadd_overflow (A, B, Result);
#else
    if (sizeof (UINTN) == sizeof (UINT64)) {
        return OverflowAddU64 (A, B, (UINT64 *)Result);
    }

    return OverflowAddU32 ((UINT32)A, (UINT32)B, (UINT32 *)Result);
#endif
}


/**

OcGuardLib

Copyright (c) 2018, vit9696

All rights reserved.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
BOOLEAN
OverflowAddU32(
        UINT32 A,
        UINT32 B,
        UINT32 *Result
) {
#if defined (HAS_TYPE_GENERIC_BUILTINS)
    return __builtin_add_overflow (A, B, Result);
#elif defined (HAS_TYPE_SPECIFIC_BUILTINS_64)
    return __builtin_uadd_overflow(A, B, Result);
#else
    UINT32  Temp;

    //
    // I believe casting will be faster on X86 at least.
    //
    Temp    = A + B;
    *Result = Temp;
    if (Temp >= A) {
        return FALSE;
    }

    return TRUE;
#endif
}

/**
  Copyright (C) 2016, The HermitCrabs Lab. All rights reserved.

  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
VOID
UnicodeFilterString(
        IN OUT CHAR16 *String,
        IN     BOOLEAN SingleLine
) {
    while (*String != L'\0') {
        if ((*String & 0x7FU) != *String) {
            //
            // Remove all unicode characters.
            //
            *String = L'_';
        } else if (SingleLine && ((*String == L'\r') || (*String == L'\n'))) {
            //
            // Stop after printing one line.
            //
            *String = L'\0';
            break;
        } else if ((*String < 0x20) || (*String == 0x7F)) {
            //
            // Drop all unprintable spaces but space including tabs.
            //
            *String = L'_';
        }

        ++String;
    }
}

/**
  Copyright (C) 2019, vit9696. All rights reserved.

  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
CHAR16 *
GetVolumeLabel(
        IN     EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem
) {
    EFI_STATUS Status;

    EFI_FILE_HANDLE Volume;
    EFI_FILE_SYSTEM_VOLUME_LABEL *VolumeInfo;
    UINTN VolumeLabelSize;

    ASSERT (FileSystem != NULL);

    Volume = NULL;
    Status = FileSystem->OpenVolume(
            FileSystem,
            &Volume
    );

    if (EFI_ERROR (Status)) {
        return NULL;
    }

    VolumeInfo = GetFileInfo(
            Volume,
            &gEfiFileSystemVolumeLabelInfoIdGuid,
            sizeof(EFI_FILE_SYSTEM_VOLUME_LABEL),
            &VolumeLabelSize
    );

    Volume->Close(Volume);

    STATIC_ASSERT(
            OFFSET_OF (EFI_FILE_SYSTEM_VOLUME_LABEL, VolumeLabel) == 0,
            "Expected EFI_FILE_SYSTEM_VOLUME_LABEL to represent CHAR16 string!"
    );

    if (VolumeInfo != NULL) {
        if ((VolumeLabelSize >= sizeof(CHAR16)) && (VolumeInfo->VolumeLabel[0] != L'\0')) {
            //
            // The spec requires disk label to be NULL-terminated, but it
            // was unclear whether the size should contain terminator or not.
            // Some old HFS Plus drivers provide volume label size without
            // terminating \0 (though they do append it). These drivers must
            // not be used, but we try not to die when debugging is off.
            //
            if ((VolumeLabelSize > MAX_VOLUME_LABEL_SIZE * sizeof(CHAR16))
                || (VolumeInfo->VolumeLabel[VolumeLabelSize / sizeof(CHAR16) - 1] != '\0')) {
                //Found unterminated or too long volume label
                FreePool(VolumeInfo);
                return AllocateCopyPool(sizeof(L"INVALID"), L"INVALID");
            } else {
                UnicodeFilterString(VolumeInfo->VolumeLabel, TRUE);
                return VolumeInfo->VolumeLabel;
            }
        }

        FreePool(VolumeInfo);
    }

    return AllocateCopyPool(sizeof(L"NULL"), L"NULL");
}

/**
  Copyright (C) 2019, vit9696. All rights reserved.

  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
VOID *
GetFileInfo(
        IN  EFI_FILE_PROTOCOL *File,
        IN  EFI_GUID *InformationType,
        IN  UINTN MinFileInfoSize,
        OUT UINTN *RealFileInfoSize  OPTIONAL
) {
    VOID *FileInfoBuffer;

    UINTN FileInfoSize;
    EFI_STATUS Status;

    FileInfoSize = 0;
    FileInfoBuffer = NULL;

    Status = File->GetInfo(
            File,
            InformationType,
            &FileInfoSize,
            NULL
    );

    if ((Status == EFI_BUFFER_TOO_SMALL) && (FileInfoSize >= MinFileInfoSize)) {
        //
        // Some drivers (i.e. built-in 32-bit Apple HFS driver) may possibly omit null terminators from file info data.
        //
        if (CompareGuid2(InformationType, &gEfiFileInfoGuid) &&
            OverflowAddUN(FileInfoSize, sizeof(CHAR16), &FileInfoSize)) {
            return NULL;
        }

        FileInfoBuffer = AllocateZeroPool(FileInfoSize);

        if (FileInfoBuffer != NULL) {
            Status = File->GetInfo(
                    File,
                    InformationType,
                    &FileInfoSize,
                    FileInfoBuffer
            );

            if (!EFI_ERROR (Status)) {
                if (RealFileInfoSize != NULL) {
                    *RealFileInfoSize = FileInfoSize;
                }
            } else {
                FreePool(FileInfoBuffer);

                FileInfoBuffer = NULL;
            }
        }
    }

    return FileInfoBuffer;
}

/**
  Copyright (c) 2020, PMheart. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
**/
BOOLEAN
EFIAPI
CompareGuid2(
        IN  CONST GUID *Guid1,
        IN  CONST GUID *Guid2
) {
    ASSERT (Guid1 != NULL);
    ASSERT (Guid2 != NULL);

    return CompareMem2(Guid1, Guid2, sizeof(GUID)) == 0;
}

/**
  Copyright (c) 2020, PMheart. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
**/
INTN
EFIAPI
CompareMem2(
        IN  CONST VOID *DestinationBuffer,
        IN  CONST VOID *SourceBuffer,
        IN  UINTN Length
) {
    ASSERT (DestinationBuffer != NULL);
    ASSERT (SourceBuffer != NULL);

    return CompareMem(DestinationBuffer, SourceBuffer, (UINTN) Length);
}