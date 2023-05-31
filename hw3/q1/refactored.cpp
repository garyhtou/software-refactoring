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

// create the Python file which will be used to compile the .pyx, .pxd, library,
// and header files into a Python module.
static void makePYFile() {
  fileinfo py = { NULL, NULL, NULL };

  openLibraryHelperFile(&py, pythonModulename, "py");

  if (py.fptr != NULL) {
    FILE* save_cfile = gGenInfo->cfile;

    gGenInfo->cfile = py.fptr;

    std::string libname = "";
    int libLength = strlen("lib");
    bool startsWithLib = strncmp(executableFilename, "lib", libLength) == 0;
    if (startsWithLib) {
      libname += &executableFilename[libLength];
    } else {
      libname = executableFilename;
    }

    // Imports
    fprintf(py.fptr, "from setuptools import setup\n");
    fprintf(py.fptr, "from setuptools import Extension\n");
    fprintf(py.fptr, "from Cython.Build import cythonize\n");
    fprintf(py.fptr, "import numpy\n\n");

    // Get the static Chapel runtime and third-party libraries
    fprintf(py.fptr, "chpl_libraries=[");
    bool first = true;
    // Get the libraries listed in require statements
    for_vector(const char, libName, libFiles) {
      if (first) {
        first = false;
      } else {
        fprintf(py.fptr, ", ");
      }
      fprintf(py.fptr, "\"%s\"", libName);
    }
    std::string libraries = getCompilelineOption("libraries");

    // Erase trailing newline and append multilocale-only dependencies.
    if (fMultiLocaleInterop) {
      libraries.erase(libraries.length() - 1);
      libraries += " ";
      libraries += getCompilelineOption("multilocale-lib-deps");
    }

    char copyOfLib[libraries.length() + 1];
    libraries.copy(copyOfLib, libraries.length(), 0);
    copyOfLib[libraries.length()] = '\0';
    int prefixLen = strlen("-l");
    char* curSection = strtok(copyOfLib, " \n");
    // Get the libraries from compileline --libraries, taking the `name`
    // portion from all `-lname` parts of that command's output
    while (curSection != NULL) {
      if (strncmp(curSection, "-l", prefixLen) == 0) {
        if (first) {
          first = false;
        } else {
          fprintf(py.fptr, ", ");
        }
        fprintf(py.fptr, "\"%s\"", &curSection[prefixLen]);
      }
      curSection = strtok(NULL, " \n");
    }

    // Fetch addition
    fprintf(py.fptr, "]\n");

    // Cythonize me, Captain!
    fprintf(py.fptr, "setup(name = '%s library',\n", pythonModulename);
    fprintf(py.fptr, "\text_modules = cythonize(\n");
    fprintf(py.fptr, "\t\tExtension(\"%s\",\n", pythonModulename);
    fprintf(py.fptr, "\t\t\tinclude_dirs=[numpy.get_include()],\n");
    fprintf(py.fptr, "\t\t\tsources=[\"%s.pyx\"],\n", pythonModulename);
    fprintf(py.fptr, "\t\t\tlibraries=[\"%s\"] + chpl_libraries + "
                     "[\"%s\"])))\n",
                     libname.c_str(), libname.c_str());

    gGenInfo->cfile = save_cfile;
  }
  // Don't "beautify", it will remove the tabs
  closeLibraryHelperFile(&py, false);
}
