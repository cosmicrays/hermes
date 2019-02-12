cmake_minimum_required (VERSION 3.1)
project(hermes)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(HERMES_EXTRA_SOURCES)
set(HERMES_EXTRA_INCLUDES)
set(HERMES_EXTRA_LIBRARIES)

set (CMAKE_CXX_STANDARD 14)

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# GSL required dependency
find_package(GSL REQUIRED)
if(GSL_FOUND)
	list(APPEND HERMES_EXTRA_INCLUDES ${GSL_INCLUDE_DIRS})
	list(APPEND HERMES_EXTRA_LIBRARIES ${GSL_LIBRARIES})
else(GSL_FOUND)
	return()
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

# CFITSIO (optional for FITS output format)
option(ENABLE_CFITSIO "CFITSIO for FITS output" ON)
if(ENABLE_CFITSIO)
	add_subdirectory(libs/cfitsio)
        list(APPEND HERMES_EXTRA_INCLUDES libs/cfitsio)
        list(APPEND HERMES_EXTRA_LIBRARIES cfitsio)
	add_definitions(-DHERMES_HAVE_CFITSIO)
endif(ENABLE_CFITSIO)
	
option(ENABLE_SYS_CFITSIO "System CFITSIO for FITS output" OFF)
if(ENABLE_SYS_CFITSIO)
	find_package(CFITSIO REQUIRED)
	if(CFITSIO_FOUND)
                list(APPEND HERMES_EXTRA_INCLUDES ${CFITSIO_INCLUDE_DIR})
                list(APPEND HERMES_EXTRA_LIBRARIES ${CFITSIO_LIBRARIES})
		add_definitions(-DHERMES_HAVE_CFITSIO)
	endif(CFITSIO_FOUND)
endif(ENABLE_SYS_CFITSIO)

# ymw16 library
# YMW16 is a model for the distribution of free electrons"
add_subdirectory(libs/ymw16)
list(APPEND HERMES_EXTRA_INCLUDES libs/ymw16)
list(APPEND HERMES_EXTRA_LIBRARIES ymw16)

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
	src/skymaps/Skymap.cpp
	src/skymaps/SynchroSkymapRange.cpp
	src/integrators/Integrator.cpp
	src/integrators/RMIntegrator.cpp
	src/integrators/SynchroIntegrator.cpp
	src/magneticField/JF12Field.cpp
	src/magneticField/PT11Field.cpp
	src/magneticField/MagneticField.cpp
	src/magneticField/MagneticFieldGrid.cpp
	src/gasDensity/HII_Cordes91.cpp
	src/gasDensity/YMW16.cpp
	src/cosmicRayDensity/SimpleCRDensity.cpp
	src/cosmicRayDensity/DragonCRDensity.cpp
	src/outputs/FITSOutput.cpp
	${HERMES_EXTRA_SOURCES}
)

add_executable(example src/example.cpp)
target_link_libraries(example ${PROJECT_NAME} ${HERMES_EXTRA_LIBRARIES})

# ----------------------------------------------------------------------------
# Python
# ----------------------------------------------------------------------------
option(ENABLE_PYTHON "Create python library via SWIG" OFF)
find_package(PythonInterp)
find_package(PythonLibs)

if(ENABLE_PYTHON AND PYTHONLIBS_FOUND)

	find_package(SWIG 2.0 REQUIRED)
	include_directories(${PYTHON_INCLUDE_PATH})

	if(ENABLE_SWIG_BUILTIN)
		set(BUILTIN "-builtin")
		set(PY3 "-py3")
	else(ENABLE_SWIG_BUILTIN)
		set(BUILTIN "")
		set(PY3 "")
	endif(ENABLE_SWIG_BUILTIN)

	if(PYTHON_VERSION_STRING VERSION_GREATER 3.0)
		list(APPEND HERMES_SWIG_DEFINES -DSWIG_PYTHON3)
	endif(PYTHON_VERSION_STRING VERSION_GREATER 3.0)

	file(GLOB_RECURSE HERMES_SWIG_INPUTS python/*.i)
	set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/hermes_wrap.cxx PROPERTIES GENERATED true )
	add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hermes_wrap.cxx
		COMMAND swig ${BUILTIN} -c++ -python ${PY3} -I${CMAKE_SOURCE_DIR}/include ${HERMES_SWIG_DEFINES} -dirprot -o ${CMAKE_CURRENT_BINARY_DIR}/hermes_wrap.cxx -outdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/python/hermes.i
		DEPENDS ${HERMES_SWIG_INPUTS} ${HERMES_INCLUDES} ${HERMES_EXTRA_INCLUDES})

	add_library(hermes-swig MODULE ${CMAKE_CURRENT_BINARY_DIR}/hermes_wrap.cxx)
	set_target_properties(hermes-swig PROPERTIES PREFIX "")
	set_target_properties(hermes-swig PROPERTIES OUTPUT_NAME "_hermes")
	target_link_libraries(hermes-swig hermes ${PYTHON_LIBRARIES} ${HERMES_EXTRA_LIBRARIES})

	install(DIRECTORY "${CMAKE_SOURCE_DIR}/python/hermes" DESTINATION "${PYTHON_SITE_PACKAGES}")
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/hermes.py" DESTINATION "${PYTHON_SITE_PACKAGES}/hermes")
	install(TARGETS hermes-swig LIBRARY DESTINATION "${PYTHON_SITE_PACKAGES}/hermes")
	install(FILES ${HERMES_SWIG_INPUTS} DESTINATION share/hermes/swig_interface)
endif(ENABLE_PYTHON AND PYTHONLIBS_FOUND)


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

endif(ENABLE_TESTING)
