cmake_minimum_required (VERSION 2.6)
project(hermes)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(HERMES_EXTRA_SOURCES)
set(HERMES_EXTRA_INCLUDES)
set(HERMES_EXTRA_LIBRARIES)

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# GSL required dependency
#include(FindGSL)
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
find_package(CFITSIO)
        if(CFITSIO_FOUND)
                list(APPEND HERMES_EXTRA_INCLUDES ${CFITSIO_INCLUDE_DIR})
                list(APPEND HERMES_EXTRA_LIBRARIES ${CFITSIO_LIBRARIES})
		add_definitions(-DHERMES_HAVE_CFITSIO)
        endif(CFITSIO_FOUND)
endif(ENABLE_CFITSIO)

# ----------------------------------------------------------------------------
# Library and Binary
# ----------------------------------------------------------------------------
file(GLOB_RECURSE HERMES_INCLUDES RELATIVE ${CMAKE_SOURCE_DIR} include/*.h)
include_directories(include ${HERMES_EXTRA_INCLUDES})

#set(SOURCE_FILES
#)

add_library(hermes SHARED
	src/Random.cpp
	src/HEALPix.cpp
	src/integrators/Integrator.cpp
	src/integrators/RMIntegrator.cpp
	src/integrators/SynchroIntegrator.cpp
	src/magneticField/JF12Field.cpp
	src/magneticField/MagneticField.cpp
	src/magneticField/MagneticFieldGrid.cpp
	src/gasDensity/HII_Cordes91.cpp
	src/cosmicRayDensity/SimpleCRDensity.cpp
	src/outputs/FITSOutput.cpp
	${HERMES_EXTRA_SOURCES}
)

add_executable(example src/example.cpp)
target_link_libraries(example ${PROJECT_NAME} ${HERMES_EXTRA_LIBRARIES})


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

	add_executable(testHEALPix test/testHEALPix.cpp)
        target_link_libraries(testHEALPix hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testHEALPix testHEALPix)

	add_executable(testIntegrator test/testIntegrator.cpp)
        target_link_libraries(testIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
        add_test(testIntegrator testIntegrator)
	
	add_executable(testSynchroIntegrator test/testSynchroIntegrator.cpp)
	target_link_libraries(testSynchroIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
	add_test(testSynchroIntegrator testSynchroIntegrator)

endif(ENABLE_TESTING)
