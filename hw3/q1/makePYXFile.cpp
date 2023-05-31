// Generate the .pyx file for the library.  This will also be used when
// creating the Python module.
static void makePYXFile(std::vector<FnSymbol*> functions) {
  fileinfo pyx = { NULL, NULL, NULL };

  openLibraryHelperFile(&pyx, pythonModulename, "pyx");

  if (pyx.fptr != NULL) {
    FILE* save_cfile = gGenInfo->cfile;

    gGenInfo->cfile = pyx.fptr;

    fprintf(pyx.fptr, "#!python\n");
    fprintf(pyx.fptr, "#cython: language_level=3\n");
    // Make import statement at top of .pyx file for chpl_library_init and
    // chpl_library_finalize
    fprintf(pyx.fptr, "from chplrt cimport chpl_library_init, ");
    fprintf(pyx.fptr, "chpl_library_finalize, chpl_external_array, ");
    fprintf(pyx.fptr, "chpl_make_external_array, chpl_make_external_array_ptr");
    fprintf(pyx.fptr, ", chpl_free_external_array, chpl_opaque_array, ");
    fprintf(pyx.fptr, "cleanupOpaqueArray, chpl_free, ");
    fprintf(pyx.fptr, "chpl_byte_buffer, chpl_byte_buffer_free, ");
    fprintf(pyx.fptr, "PyBytes_FromStringAndSize\n");

    std::vector<FnSymbol*> moduleInits;
    std::vector<FnSymbol*> exported;

    fprintf(pyx.fptr, "from %s cimport ", pxdName.c_str());
    bool first = true;
    // Make import statement at top of .pyx file for exported functions
    for_vector(FnSymbol, fn, functions) {
      if (isUserRoutine(fn)) {
        if (fn->hasFlag(FLAG_EXPORT)) {
          if (first) {
            first = false;
          } else {
            fprintf(pyx.fptr, ", ");
          }

          // Module initialization functions get handled together, other
          // exported functions will have their own definition.
          if (fn->hasFlag(FLAG_MODULE_INIT)) {
            // No need to rename the module init function, the user won't see it
            fprintf(pyx.fptr, "%s", fn->cname);
            moduleInits.push_back(fn);
          } else {
            // On import, rename the exported function so that we can use its
            // original name in the Python module to avoid confusion.
            fprintf(pyx.fptr, "%s as chpl_%s", fn->cname, fn->cname);
            exported.push_back(fn);
          }
        }
      }
    }
    fprintf(pyx.fptr, "\n\n");

    // Necessary for using numpy types
    fprintf(pyx.fptr, "import numpy\n");
    fprintf(pyx.fptr, "cimport numpy\n");
    // Necessary for supporting pointers
    fprintf(pyx.fptr, "import ctypes\n");
    fprintf(pyx.fptr, "from libc.stdint cimport intptr_t\n\n");

    makePYXSetupFunctions(moduleInits);
    makeOpaqueArrayClass();

    // Add Python wrapper for the exported functions, to translate the types
    // appropriately
    for_vector(FnSymbol, fn, exported) {
      fn->codegenPython(PYTHON_PYX);
    }

    gGenInfo->cfile = save_cfile;
  }
  // Don't "beautify", it will remove the tabs
  closeLibraryHelperFile(&pyx, false);
}
