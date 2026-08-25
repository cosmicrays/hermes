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

include(FindPackageHandleStandardArgs)


if(CFITSIO_INCLUDE_DIR AND CFITSIO_LIBRARIES)

	# Find the version of the cfitsio header
	FILE(READ "${CFITSIO_INCLUDE_DIR}/fitsio.h" FITSIO_H)
	STRING(REGEX REPLACE ".*#define CFITSIO_VERSION[^0-9]*([0-9]+)\\.([0-9]+).*" "\\1.\\2" CFITSIO_VERSION_STRING "${FITSIO_H}")
	STRING(REGEX REPLACE "^([0-9]+)[.]([0-9]+)" "\\1" CFITSIO_VERSION_MAJOR ${CFITSIO_VERSION_STRING})
	STRING(REGEX REPLACE "^([0-9]+)[.]([0-9]+)" "\\2" CFITSIO_VERSION_MINOR ${CFITSIO_VERSION_STRING})
	message(STATUS "found version string ${CFITSIO_VERSION_STRING}")

endif()

find_package_handle_standard_args(CFITSIO
	REQUIRED_VARS CFITSIO_LIBRARIES CFITSIO_INCLUDE_DIR
	VERSION_VAR CFITSIO_VERSION_STRING)

if(CFITSIO_FOUND AND NOT TARGET CFITSIO::CFITSIO)
	add_library(CFITSIO::CFITSIO UNKNOWN IMPORTED)
	set_target_properties(CFITSIO::CFITSIO PROPERTIES
		IMPORTED_LOCATION "${CFITSIO_LIBRARIES}"
		INTERFACE_INCLUDE_DIRECTORIES "${CFITSIO_INCLUDE_DIR}")
endif()

mark_as_advanced(CFITSIO_INCLUDE_DIR CFITSIO_LIBRARIES)
