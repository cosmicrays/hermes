cmake_minimum_required (VERSION 3.1)
project(hermes C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_BUILD_TYPE Debug)

set(HERMES_EXTRA_SOURCES)
set(HERMES_EXTRA_INCLUDES)
set(HERMES_EXTRA_LIBRARIES)

set (CMAKE_CXX_STANDARD 14)
cmake_policy(SET CMP0048 NEW)

# Set default build-type to release to enable performance improvements
if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--as-needed")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
        message(STATUS "Use --as-needed linker flags!")
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE)

# ----------------------------------------------------------------------------
# Fix Apple RPATH
# ----------------------------------------------------------------------------
set(CMAKE_MACOSX_RPATH 1)
option(USE_ABSOLUTE_RPATH "Add absolute rpath to all libraries and executables" ON)
if(APPLE OR USE_ABSOLUTE_RPATH)
	set(CMAKE_SKIP_BUILD_RPATH  FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(ABSOLUTE_RPATH "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
	if(NOT IS_ABSOLUTE ${ABSOLUTE_RPATH})
		set(ABSOLUTE_RPATH ${CMAKE_BINARY_DIR}/${ABSOLUTE_RPATH})
	endif()

	list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${ABSOLUTE_RPATH}" isSystemDir)
	if("${isSystemDir}" STREQUAL "-1")
		message(STATUS "Use absolute RPATH ${ABSOLUTE_RPATH}")
		set(CMAKE_INSTALL_RPATH "${ABSOLUTE_RPATH}")
	endif("${isSystemDir}" STREQUAL "-1")
endif()

# Fix for linker error on mac, Issue: #147
if(APPLE)
	add_definitions(-arch x86_64)
endif(APPLE)

# ---------------------------------------------------------------------------
# Download data files
# ----------------------------------------------------------------------------
OPTION(DOWNLOAD_DATA "Download HERMES data files" ON)
if(DOWNLOAD_DATA)
	message("-- Downloading data file from heat.gssi.it ~ 130 MB")
	file(DOWNLOAD
		https://heat.gssi.it/hermes/data/hermes-data.tar.gz-CHECKSUM
		${CMAKE_BINARY_DIR}/hermes-data.tar.gz-CHECKSUM)
	file(STRINGS ${CMAKE_BINARY_DIR}/hermes-data.tar.gz-CHECKSUM DATA_CHECKSUM LIMIT_COUNT 1 LENGTH_MINIMUM 32 LENGTH_MAXIMUM 32)
	file(DOWNLOAD
		https://heat.gssi.it/hermes/data/hermes-data.tar.gz
		${CMAKE_BINARY_DIR}/hermes-data.tar.gz
		EXPECTED_MD5 "${DATA_CHECKSUM}")
	message("-- Extracting data file")
else()
	message("-- Downloading of data file disabled")
endif(DOWNLOAD_DATA)
if(EXISTS ${CMAKE_BINARY_DIR}/hermes-data.tar.gz)
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/hermes-data.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
	message(WARNING "HERMES data file not found at ${CMAKE_BINARY_DIR}/hermes-data.tar.gz
	HERMES should compile, but not all components will work without them! Please install data file manually, or use the automatic download which is enabled by default.")
endif()


# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# GSL required dependency
find_package(GSL REQUIRED)
if(GSL_FOUND)
	list(APPEND HERMES_EXTRA_INCLUDES ${GSL_INCLUDE_DIRS})
	list(APPEND HERMES_EXTRA_LIBRARIES ${GSL_LIBRARIES})
endif(GSL_FOUND)


# googletest (provided, see https://code.google.com/p/googletest/wiki/FAQ
#     Why is it not recommended use a pre-compiled copy of Google Test?)
option(ENABLE_TESTING "Build tests and enable test target" ON)
if(ENABLE_TESTING)
        include_directories(libs/gtest/googletest/include)
        add_subdirectory(libs/gtest/googletest)
        if(APPLE)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")
        endif(APPLE)
endif(ENABLE_TESTING)

# OpenMP (optional for shared memory multiprocessing)
option(ENABLE_OPENMP "OpenMP for multithreading" ON)
if(ENABLE_OPENMP)
find_package(OpenMP)
        if(OPENMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        endif(OPENMP_FOUND)
endif(ENABLE_OPENMP)

# FFTW3F (optional for turbulent magnetic fields)
find_package(FFTW3F)
if(FFTW3F_FOUND)
        list(APPEND HERMES_EXTRA_INCLUDES ${FFTW3F_INCLUDE_DIR})
        list(APPEND HERMES_EXTRA_LIBRARIES ${FFTW3F_LIBRARIES})
        add_definitions(-DHERMES_HAVE_FFTW3F)
        list(APPEND HERMES_SWIG_DEFINES -DHERMES_HAVE_FFTW3F)
endif(FFTW3F_FOUND)

# CFITSIO (optional for FITS output format)
option(ENABLE_CFITSIO "CFITSIO for FITS output" OFF)
if(ENABLE_CFITSIO)
	add_subdirectory(libs/cfitsio)
        list(APPEND HERMES_EXTRA_INCLUDES libs/cfitsio)
        list(APPEND HERMES_EXTRA_LIBRARIES cfitsio)
	add_definitions(-DHERMES_HAVE_CFITSIO)
endif(ENABLE_CFITSIO)
	
option(ENABLE_SYS_CFITSIO "System CFITSIO for FITS output" ON)
if(ENABLE_SYS_CFITSIO)
	#set(CFITSIO_EXE_LINKER_FLAGS "-static")
	find_package(CFITSIO REQUIRED)
	if(CFITSIO_FOUND)
                list(APPEND HERMES_EXTRA_INCLUDES ${CFITSIO_INCLUDE_DIR})
                list(APPEND HERMES_EXTRA_LIBRARIES ${CFITSIO_LIBRARIES})
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFITSIO_C_FLAGS}")
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CFITSIO_EXE_LINKER_FLAGS}")
		add_definitions(-DHERMES_HAVE_CFITSIO)
	endif(CFITSIO_FOUND)
endif(ENABLE_SYS_CFITSIO)

# kiss (provided)
add_subdirectory(libs/kiss)
list(APPEND HERMES_EXTRA_LIBRARIES kiss)
list(APPEND HERMES_EXTRA_INCLUDES libs/kiss/include)

# ymw16 (provided)
# YMW16 is a model for the distribution of free electrons
add_subdirectory(libs/ymw16)
list(APPEND HERMES_EXTRA_INCLUDES libs/ymw16)
list(APPEND HERMES_EXTRA_LIBRARIES ymw16)

# cparamlib (provided)
# p-p cross-sections
add_subdirectory(libs/cparamlib)
list(APPEND HERMES_EXTRA_INCLUDES libs/cparamlib)
list(APPEND HERMES_EXTRA_LIBRARIES cparamlib)

# ----------------------------------------------------------------------------
# Library and Binary
# ----------------------------------------------------------------------------
file(GLOB_RECURSE HERMES_INCLUDES RELATIVE ${CMAKE_SOURCE_DIR} include/*.h)
include_directories(include ${HERMES_EXTRA_INCLUDES})

add_library(hermes SHARED
	src/Random.cpp
	src/Common.cpp
	src/HEALPixBits.cpp
	src/FITSWrapper.cpp
	src/ProgressBar.cpp
	src/GridTools.cpp
	src/Signals.cpp
	src/skymaps/Skymap.cpp
	src/skymaps/SkymapMask.cpp
	src/skymaps/GammaSkymapRange.cpp
	src/skymaps/RadioSkymapRange.cpp
	src/integrators/GenericIntegrator.cpp
	src/integrators/DMIntegrator.cpp
	src/integrators/RMIntegrator.cpp
	src/integrators/SynchroIntegrator.cpp
	src/integrators/FreeFreeIntegrator.cpp
	src/integrators/SynchroAbsorptionIntegrator.cpp
	src/integrators/PiZeroIntegrator.cpp
	src/integrators/InverseComptonIntegrator.cpp
	src/magneticField/MagneticField.cpp
	src/magneticField/MagneticFieldGrid.cpp
	src/magneticField/WMAP07Field.cpp
	src/magneticField/Sun08Field.cpp
	src/magneticField/PT11Field.cpp
	src/magneticField/JF12Field.cpp
	src/chargedGasDensity/HII_Cordes91.cpp
	src/chargedGasDensity/YMW16.cpp
	src/neutralGasDensity/RingModelDensity.cpp
	src/cosmicRayDensity/SimpleCRDensity.cpp
	src/cosmicRayDensity/WMAP07CRDensity.cpp
	src/cosmicRayDensity/Sun08CRDensity.cpp
	src/cosmicRayDensity/DragonCRDensity.cpp
	src/photonField/ISRF.cpp
	src/interactions/Kamae06.cpp
	src/interactions/KleinNishina.cpp
	src/outputs/FITSOutput.cpp
	${HERMES_EXTRA_SOURCES}
)
target_link_libraries(hermes ${HERMES_EXTRA_LIBRARIES})

# ----------------------------------------------------------------------------
# Python
# ----------------------------------------------------------------------------
option(ENABLE_PYTHON "Create python library via Pybind11" ON)
find_package(PythonInterp)
find_package(PythonLibs)

if(ENABLE_PYTHON AND PYTHONLIBS_FOUND)
	include_directories(${PYTHON_INCLUDE_DIRS})
	include(cmake/FindPython.cmake)
	add_subdirectory(libs/pybind11)
	
	pybind11_add_module(pyhermes
		python/Units.cpp
		python/Vectors.cpp
		python/Outputs.cpp
		python/Skymaps.cpp
		python/Integrators.cpp
		python/MagneticFields.cpp
		python/ChargedGasDensity.cpp
		python/PyHermes.cpp
	)

	set_target_properties(pyhermes PROPERTIES
				        PREFIX ""
					OUTPUT_NAME "pyhermes"
				        LINKER_LANGUAGE CXX)
	target_link_libraries(pyhermes PRIVATE hermes)

	install(TARGETS pyhermes LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}")
endif(ENABLE_PYTHON AND PYTHONLIBS_FOUND)

# ------------------------------------------------------------------
# Documentation
# ------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxygenLayout.xml ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml COPYONLY)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Mainpage.md.in ${CMAKE_CURRENT_BINARY_DIR}/Mainpage.md)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


# ----------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------
if(ENABLE_TESTING)
        enable_testing()

        add_executable(testUnits test/testUnits.cpp)
        target_link_libraries(testUnits hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testUnits testUnits)
        
        add_executable(testCommon test/testCommon.cpp)
        target_link_libraries(testCommon hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testCommon testCommon)
        
	add_executable(testCacheTools test/testCacheTools.cpp)
        target_link_libraries(testCacheTools hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testCacheTools testCacheTools)
        
	add_executable(testVector3 test/testVector3.cpp)
        target_link_libraries(testVector3 hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testVector3 testVector3)

	add_executable(testHEALPix test/testHEALPix.cpp)
        target_link_libraries(testHEALPix hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testHEALPix testHEALPix)
	
	add_executable(testSkymap test/testSkymap.cpp)
	target_link_libraries(testSkymap hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testSkymap testSkymap)
	
	add_executable(testFITS test/testFITS.cpp)
	target_link_libraries(testFITS hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testFITS testFITS)

	add_executable(testRMIntegrator test/testRMIntegrator.cpp)
	target_link_libraries(testRMIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testRMIntegrator testRMIntegrator)
	
	add_executable(testSynchroIntegrator test/testSynchroIntegrator.cpp)
	target_link_libraries(testSynchroIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testSynchroIntegrator testSynchroIntegrator)
	
	add_executable(testFreeFreeIntegrator test/testFreeFreeIntegrator.cpp)
	target_link_libraries(testFreeFreeIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testFreeFreeIntegrator testFreeFreeIntegrator)

	add_executable(testMFieldModels test/testMFieldModels.cpp)
	target_link_libraries(testMFieldModels hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testMFieldModels testMFieldModels)
	
	add_executable(testSynchroAbsorptionIntegrator test/testSynchroAbsorptionIntegrator.cpp)
	target_link_libraries(testSynchroAbsorptionIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testSynchroAbsorptionIntegrator testSynchroAbsorptionIntegrator)
	
	add_executable(testInteractions test/testInteractions.cpp)
	target_link_libraries(testInteractions hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testInteractions testInteractions)

	add_executable(testRingModelDensity test/testRingModelDensity.cpp)
	target_link_libraries(testRingModelDensity hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testRingModelDensity testRingModelDensity)

	add_executable(testPiZeroIntegrator test/testPiZeroIntegrator.cpp)
	target_link_libraries(testPiZeroIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testPiZeroIntegrator testPiZeroIntegrator)

	add_executable(testPhotonField test/testPhotonField.cpp)
	target_link_libraries(testPhotonField hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testPhotonField testPhotonField)
	
	add_executable(testInverseComptonIntegrator test/testInverseComptonIntegrator.cpp)
	target_link_libraries(testInverseComptonIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testInverseComptonIntegrator testInverseComptonIntegrator)
endif(ENABLE_TESTING)

# ----------------------------------------------------------------------------
# Playground
# ----------------------------------------------------------------------------
add_executable(example src/example.cpp)
target_link_libraries(example ${PROJECT_NAME} ${HERMES_EXTRA_LIBRARIES})

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
add_definitions(-DHERMES_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
install(TARGETS hermes DESTINATION lib)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/data/ DESTINATION share/hermes/data PATTERN ".git" EXCLUDE)
install(DIRECTORY libs/ymw16/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
install(DIRECTORY libs/cparamlib/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
install(DIRECTORY libs/kiss/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)

