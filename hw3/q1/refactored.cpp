static void makePythonFile(const char* fileName, std::string fileExtension,
                           const std::function<void(fileinfo&)>& writeFileContents) {
  fileinfo file = { NULL, NULL, NULL };

  openLibraryHelperFile(&file, fileName, fileExtension);

  if (file.fptr == NULL) {
    closeLibraryHelperFile(&file, false);
    return;
  }

  FILE* save_cfile = gGenInfo->cfile;

  gGenInfo->cfile = pxd.fptr;

  writeFileContents(file);

  gGenInfo->cfile = save_cfile;

  closeLibraryHelperFile(&pxd, false);
}

static void makePXDFile(std::vector<FnSymbol*> functions) {
  makePythonFile(pxdName.c_str(), "pxd", [&](fileinfo& pxd) {
    fprintf(pxd.fptr, "from libc.stdint cimport *\n");
    fprintf(pxd.fptr, "from chplrt cimport *\n\n");

    fprintf(pxd.fptr, "cdef extern from \"%s.h\":\n", libmodeHeadername);

    for_vector(FnSymbol, fn, functions) {
      if (isUserRoutine(fn)) {
        fn->codegenPython(C_PXD);
      }
    }
  });
}

static void makePYXFile(std::vector<FnSymbol*> functions) {
  makePythonFile(pythonModulename, "pyx", [&](fileinfo& pyx) {
    fprintf(pyx.fptr, "#!python\n");
    fprintf(pyx.fptr, "#cython: language_level=3\n");
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
    for_vector(FnSymbol, fn, functions) {
      if (!isUserRoutine(fn) || !fn->hasFlag(FLAG_EXPORT)) {
        continue;
      }

      if (first) {
        first = false;
      } else {
        fprintf(pyx.fptr, ", ");
      }

      if (fn->hasFlag(FLAG_MODULE_INIT)) {
        fprintf(pyx.fptr, "%s", fn->cname);
        moduleInits.push_back(fn);
      } else {
        fprintf(pyx.fptr, "%s as chpl_%s", fn->cname, fn->cname);
        exported.push_back(fn);
      }
    }
    fprintf(pyx.fptr, "\n\n");

    fprintf(pyx.fptr, "import numpy\n");
    fprintf(pyx.fptr, "cimport numpy\n");
    fprintf(pyx.fptr, "import ctypes\n");
    fprintf(pyx.fptr, "from libc.stdint cimport intptr_t\n\n");

    makePYXSetupFunctions(moduleInits);
    makeOpaqueArrayClass();

    for_vector(FnSymbol, fn, exported) {
      fn->codegenPython(PYTHON_PYX);
    }
  })
}

static void makePYFile() {
  writePythonFile(pythonModulename, "py", [](fileinfo& py) {
    std::string libname = "";
    int libLength = strlen("lib");
    bool startsWithLib = strncmp(executableFilename, "lib", libLength) == 0;
    if (startsWithLib) {
      libname += &executableFilename[libLength];
    } else {
      libname = executableFilename;
    }

    fprintf(py.fptr, "from setuptools import setup\n");
    fprintf(py.fptr, "from setuptools import Extension\n");
    fprintf(py.fptr, "from Cython.Build import cythonize\n");
    fprintf(py.fptr, "import numpy\n\n");

    fprintf(py.fptr, "chpl_libraries=[");
    bool first = true;
    for_vector(const char, libName, libFiles) {
      if (first) {
        first = false;
      } else {
        fprintf(py.fptr, ", ");
      }
      fprintf(py.fptr, "\"%s\"", libName);
    }
    std::string libraries = getCompilelineOption("libraries");

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

    fprintf(py.fptr, "]\n");

    fprintf(py.fptr, "setup(name = '%s library',\n", pythonModulename);
    fprintf(py.fptr, "\text_modules = cythonize(\n");
    fprintf(py.fptr, "\t\tExtension(\"%s\",\n", pythonModulename);
    fprintf(py.fptr, "\t\t\tinclude_dirs=[numpy.get_include()],\n");
    fprintf(py.fptr, "\t\t\tsources=[\"%s.pyx\"],\n", pythonModulename);
    fprintf(py.fptr, "\t\t\tlibraries=[\"%s\"] + chpl_libraries + "
                      "[\"%s\"])))\n",
                      libname.c_str(), libname.c_str());
  })
}
