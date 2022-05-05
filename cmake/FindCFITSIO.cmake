# - Try to find CFITSIO
# Once done this will define
#
#  CFITSIO_FOUND - system has CFITSIO
#  CFITSIO_INCLUDE_DIR - the CFITSIO include directory
#  CFITSIO_LIBRARIES - Link these to use CFITSIO
#  CFITSIO_VERSION_STRING - Human readable version number of cfitsio
#  CFITSIO_VERSION_MAJOR  - Major version number of cfitsio
#  CFITSIO_VERSION_MINOR  - Minor version number of cfitsio

# Copyright (c) 2006, Jasem Mutlaq <mutlaqja@ikarustech.com>
# Based on FindLibfacile by Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# JM: Packages from different distributions have different suffixes
find_path(CFITSIO_INCLUDE_DIR fitsio.h
	PATH_SUFFIXES libcfitsio3 libcfitsio0 cfitsio include inc
	PATHS $ENV{CFITSIO} $ENV{CFITSIO_BASE} ${_obIncDir} ${GNUWIN32_DIR}
	)

find_library(CFITSIO_LIBRARIES NAMES cfitsio
	PATH_SUFFIXES lib
	PATHS $ENV{CFITSIO} ${CFITSIO_BASE} ${_obLinkDir} ${GNUWIN32_DIR}
	)

if(CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)
	set(CFITSIO_FOUND TRUE)
else (CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)
	set(CFITSIO_FOUND FALSE)
endif(CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)


if (CFITSIO_FOUND)

	# Find the version of the cfitsio header
	FILE(READ "${CFITSIO_INCLUDE_DIR}/fitsio.h" FITSIO_H)
	STRING(REGEX REPLACE ".*#define CFITSIO_VERSION[^0-9]*([0-9]+)\\.([0-9]+).*" "\\1.\\2" CFITSIO_VERSION_STRING "${FITSIO_H}")
	STRING(REGEX REPLACE "^([0-9]+)[.]([0-9]+)" "\\1" CFITSIO_VERSION_MAJOR ${CFITSIO_VERSION_STRING})
	STRING(REGEX REPLACE "^([0-9]+)[.]([0-9]+)" "\\2" CFITSIO_VERSION_MINOR ${CFITSIO_VERSION_STRING})
	message(STATUS "found version string ${CFITSIO_VERSION_STRING}")

	if (NOT CFITSIO_FIND_QUIETLY)
		message(STATUS "Found CFITSIO ${CFITSIO_VERSION_MAJOR}.${CFITSIO_VERSION_MINOR}: ${CFITSIO_LIBRARIES}")
	endif (NOT CFITSIO_FIND_QUIETLY)
else (CFITSIO_FOUND)
	if (CFITSIO_FIND_REQUIRED)
		message(STATUS "CFITSIO not found.")
	endif (CFITSIO_FIND_REQUIRED)
endif (CFITSIO_FOUND)

mark_as_advanced(CFITSIO_INCLUDE_DIR CFITSIO_LIBRARIES)
