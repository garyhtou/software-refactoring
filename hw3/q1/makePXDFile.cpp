// Generate the .pxd file for the library.  This will be used when creating
// the Python module
static void makePXDFile(std::vector<FnSymbol*> functions) {
  fileinfo pxd = { NULL, NULL, NULL };

  openLibraryHelperFile(&pxd, pxdName.c_str(), "pxd");

  if (pxd.fptr != NULL) {
    FILE* save_cfile = gGenInfo->cfile;

    gGenInfo->cfile = pxd.fptr;

    fprintf(pxd.fptr, "from libc.stdint cimport *\n");
    // Get the permanent runtime definitions
    fprintf(pxd.fptr, "from chplrt cimport *\n\n");

    fprintf(pxd.fptr, "cdef extern from \"%s.h\":\n", libmodeHeadername);

    for_vector(FnSymbol, fn, functions) {
      if (isUserRoutine(fn)) {
        fn->codegenPython(C_PXD);
      }
    }

    gGenInfo->cfile = save_cfile;
  }
  // Don't "beautify", it will remove the tabs
  closeLibraryHelperFile(&pxd, false);
}
