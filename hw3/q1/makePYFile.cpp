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
