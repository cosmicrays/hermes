# Find FFTW3 with single precision. Sets
# FFTW3F_FOUND = true if fftw3f is found
# FFTW3F_INCLUDE_DIR = fftw3.h
# FFTW3F_LIBRARIES = libfftw3f.a .so


find_path(FFTW3F_INCLUDE_DIR fftw3.h
        PATH_SUFFIXES include inc
        PATHS $ENV{FFTW_BASE} $ENV{FFTW}
        )
message(STATUS "FFTW3F header => ${FFTW3F_INCLUDE_DIR}")

find_library(FFTW3F_LIBRARIES fftw3f
        PATH_SUFFIXES lib
        PATHS  $ENV{FFTW_BASE} $ENV{FFTW}
        )
message(STATUS "FFTW3F libs => ${FFTW3F_LIBRARIES}")

set(FFTW3F_FOUND FALSE)
if(FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARIES)
    set(FFTW3F_FOUND TRUE)
    MESSAGE(STATUS "FFTW3 with single precision (FFTW3F): Found!")
else()
    MESSAGE(STATUS "FFTW3 with single precision (FFTW3F): NOT Found!")
endif()

MESSAGE(STATUS "  Include:     ${FFTW3F_INCLUDE_DIR}")
MESSAGE(STATUS "  Library:     ${FFTW3F_LIBRARIES}")

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARIES FFTW3F_FOUND)
