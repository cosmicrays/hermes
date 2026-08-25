# Find FFTW3 with single precision. Sets
# FFTW3F_FOUND = true if fftw3f is found
# FFTW3F_INCLUDE_DIR = fftw3.h
# FFTW3F_LIBRARIES = libfftw3f.a .so


find_path(FFTW3F_INCLUDE_DIR fftw3.h
        PATH_SUFFIXES include inc
        PATHS $ENV{FFTW_BASE} $ENV{FFTW}
        )
find_library(FFTW3F_LIBRARIES fftw3f
        PATH_SUFFIXES lib
        PATHS  $ENV{FFTW_BASE} $ENV{FFTW}
        )
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3F
    REQUIRED_VARS FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIR)

if(FFTW3F_FOUND AND NOT TARGET FFTW3F::FFTW3F)
    add_library(FFTW3F::FFTW3F UNKNOWN IMPORTED)
    set_target_properties(FFTW3F::FFTW3F PROPERTIES
        IMPORTED_LOCATION "${FFTW3F_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFTW3F_INCLUDE_DIR}")
endif()

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARIES FFTW3F_FOUND)
