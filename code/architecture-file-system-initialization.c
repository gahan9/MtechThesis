// Firmware volume initialization entry point – returns TRUE if FFS driver can use this firmware volume.
BOOLEAN FvCheck(Fv) {
    // first check out firmware volume header
    if (FvHeaderCheck(Fv) == FALSE) {
        FAILURE();// corrupted firmware volume header
    }
    if (!((Fv->FvFileSystemId == EFI_FIRMWARE_FILE_SYSTEM2_GUID) || (Fv->FvFileSystemId == EFI_FIRMWARE_FILE_SYSTEM3_GUID))){
        return (FALSE); // This firmware volume is not formatted with FFS
    }
    // next walk files and verify the FFS is in good shape
    for (FilePtr = FirstFile; Exists(Fv, FilePtr); FilePtr = NextFile(Fv, FilePtr)) {
        if (FileCheck (Fv, FilePtr) != 0) {
            FAILURE(); // inconsistent file system
        }
    }
    if (CheckFreeSpace (Fv, FilePtr) != 0) {
        FAILURE();
    }
    return (TRUE); // this firmware volume can be used by the FFS
    // driver and the file system is OK
}

// FvHeaderCheck – returns TRUE if FvHeader checksum is OK.
BOOLEAN FvHeaderCheck (Fv) {
    return (Checksum (Fv.FvHeader) == 0);
}

// Exists – returns TRUE if any bits are set in the file header
BOOLEAN Exists(Fv, FilePtr) {
    return (BufferErased (Fv.ErasePolarity,
    FilePtr, sizeof (EFI_FIRMWARE_VOLUME_HEADER) == FALSE);
}

// BufferErased – returns TRUE if no bits are set in buffer
BOOLEAN BufferErased (ErasePolarity, BufferPtr, BufferSize) {
    UINTN Count;
    if (Fv.ErasePolarity == 1) {
        ErasedByte = 0xff;
    } else {
        ErasedByte = 0;
    }
    for (Count = 0; Count < BufferSize; Count++) {
        if (BufferPtr[Count] != ErasedByte) {
            return FALSE;
        }
    }
    return TRUE;
}

// GetFileState – returns high bit set of state field.
UINT8 GetFileState (Fv, FilePtr) {
    UINT8 FileState;
    UINT8 HighBit;
    FileState = FilePtr->State;
    if (Fv.ErasePolarity != 0) {
        FileState = ~FileState;
    }
    HighBit = 0x80;
    while (HighBit != 0 && (HighBit & FileState) == 0) {
        HighBit = HighBit >> 1;
    }
    return HighBit;
}

// FileCheck – returns TRUE if the file is OK
BOOLEAN FileCheck (Fv, FilePtr) {
    switch (GetFileState (Fv, FilePtr)) {
        case EFI_FILE_HEADER_CONSTRUCTION:
            SetHeaderBit (Fv, FilePtr, EFI_FILE_HEADER_INVALID);
            break;
        case EFI_FILE_HEADER_VALID:
            if (VerifyHeaderChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            SetHeaderBit (Fv, FilePtr, EFI_FILE_DELETED);
            Break;
        case EFI_FILE_DATA_VALID:
            if (VerifyHeaderChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            if (VerifyFileChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            if (DuplicateFileExists (Fv, FilePtr,
                EFI_FILE_DATA_VALID) != NULL) {
                return (FALSE);
            }
            break;
        case EFI_FILE_MARKED_FOR_UPDATE:
            if (VerifyHeaderChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            if (VerifyFileChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            if (FilePtr->State & EFI_FILE_DATA_VALID) == 0) {
                return (FALSE);
            }
            if (FilePtr->Type == EFI_FV_FILETYPE_FFS_PAD) {
                SetHeaderBit (Fv, FilePtr, EFI_FILE_DELETED);
            }
            else {
                if (DuplicateFileExists (Fv, FilePtr, EFI_FILE_DATA_VALID)) {
                    SetHeaderBit (Fv, FilePtr, EFI_FILE_DELETED);
                }
                else {
                    if (Fv->Attributes & EFI_FVB_STICKY_WRITE) {
                        CopyFile (Fv, FilePtr);
                        SetHeaderBit (Fv, FilePtr, EFI_FILE_DELETED);
                    }
                    else {
                        ClearHeaderBit (Fv, FilePtr, EFI_FILE_MARKED_FOR_UPDATE);
                    }
                }
            }
            break;
        case EFI_FILE_DELETED:
            if (VerifyHeaderChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            if (VerifyFileChecksum (FilePtr) != TRUE) {
                return (FALSE);
            }
            break;
        case EFI_FILE_HEADER_INVALID:
            break;
}
    return (TRUE);
}

// FFS_FILE_PTR * DuplicateFileExists (Fv, FilePtr, StateBit)
// This function searches the firmware volume for another occurrence
// of the file described by FilePtr, in which the duplicate files
// high state bit that is set is defined by the parameter StateBit.
// It returns a pointer to a duplicate file if it exists and NULL
// if it does not. If the file type is EFI_FV_FILETYPE_FFS_PAD
// then NULL must be returned.

// CopyFile (Fv, FilePtr)
// The purpose of this function is to clear the
// EFI_FILE_MARKED_FOR_UPDATE bit from FilePtr->State
// in firmware volumes that have EFI_FVB_STICKY_WRITE == TRUE.
// The file is copied exactly header and all, except that the
// EFI_FILE_MARKED_FOR_UPDATE bit in the file header of the
// new file is clear.
// VerifyHeaderChecksum (FilePtr)
// The purpose of this function is to verify the file header
// sums to zero. See IntegrityCheck.Checksum.Header definition
// for details.
// VerifyFileChecksum (FilePtr)
// The purpose of this function is to verify the file integrity
// check. See IntegrityCheck.Checksum.File definition for details.