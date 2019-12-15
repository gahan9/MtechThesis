// Screen files to ensure we get the right one in case
// of an inconsistency.
FFS_FILE_PTR EarlyFfsUpdateCheck(FFS_FILE_PTR * FilePtr) {
  FFS_FILE_PTR * FilePtr2;
    if (VerifyHeaderChecksum (FilePtr) != TRUE) {
    return (FALSE);
  }
  if (VerifyFileChecksum (FilePtr) != TRUE) {
    return (FALSE);
  }
  switch (GetFileState (Fv, FilePtr)) {
    case EFI_FILE_DATA_VALID:
      return (FilePtr);
      break;
    case EFI_FILE_MARKED_FOR_UPDATE:
      FilePtr2 = DuplicateFileExists (Fv, FilePtr,
      EFI_FILE_DATA_VALID);
      if (FilePtr2 != NULL) {
        if (VerifyHeaderChecksum (FilePtr) != TRUE) {
          return (FALSE);
        }
        if (VerifyFileChecksum (FilePtr) != TRUE) {
          return (FALSE);
        }
        return (FilePtr2);
      } else {
        return (FilePtr);
      }
      break;
  }
}