cmake_minimum_required (VERSION 2.6)
project(hermes)

# ----------------------------------------------------------------------------
# Library and Binary
# ----------------------------------------------------------------------------
file(GLOB_RECURSE HERMES_INCLUDES RELATIVE ${CMAKE_SOURCE_DIR} include/*.h)
include_directories(include)

#set(SOURCE_FILES
#)

add_library(hermes SHARED
	src/Random.cpp
	src/HEALPix.cpp
	src/Skymap.cpp
	src/integrators/Integrator.cpp
	src/integrators/RMIntegrator.cpp
	src/magneticField/JF12Field.cpp
	src/magneticField/MagneticField.cpp
	src/magneticField/MagneticFieldGrid.cpp
)

add_executable(example src/example.cpp)
target_link_libraries(example ${PROJECT_NAME})

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------
if(ENABLE_TESTING)
        enable_testing()

        add_executable(testUnits test/testUnits.cpp)
        target_link_libraries(testUnits hermes gtest gtest_main pthread)
        add_test(testUnits testUnits)
        
	add_executable(testHEALPix test/testHEALPix.cpp)
        target_link_libraries(testHEALPix hermes gtest gtest_main pthread)
        add_test(testHEALPix testHEALPix)

endif(ENABLE_TESTING)
