cmake_minimum_required (VERSION 3.5)
project(hermes C CXX)
set(HERMES_RELEASE_VERSION 0.9)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(HERMES_EXTRA_SOURCES)
set(HERMES_EXTRA_INCLUDES)
set(HERMES_EXTRA_LIBRARIES)

set (CMAKE_CXX_STANDARD 17)
cmake_policy(SET CMP0048 NEW)

# Set default build-type to release to enable performance improvements
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    #set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")

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
    message("-- Downloading data file from heat.gssi.it ~ 200 MB")
    file(DOWNLOAD
        https://zenodo.org/record/7603203/files/hermes-data.tar.gz-CHECKSUM
        ${CMAKE_BINARY_DIR}/hermes-data.tar.gz-CHECKSUM)
        file(STRINGS ${CMAKE_BINARY_DIR}/hermes-data.tar.gz-CHECKSUM DATA_CHECKSUM LIMIT_COUNT 1 LENGTH_MINIMUM 32 LENGTH_MAXIMUM 32)
    file(DOWNLOAD
        https://zenodo.org/record/7603203/files/hermes-data.tar.gz 
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

# C++ Threads required
find_package(Threads REQUIRED)
list(APPEND HERMES_EXTRA_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

# GSL required
find_package(GSL REQUIRED)
list(APPEND HERMES_EXTRA_INCLUDES ${GSL_INCLUDE_DIRS})
list(APPEND HERMES_EXTRA_LIBRARIES ${GSL_LIBRARIES})

# zlib required / zstream-cpp provided
find_package(ZLIB REQUIRED)
list(APPEND HERMES_EXTRA_INCLUDES ${ZLIB_INCLUDE_DIRS})
list(APPEND HERMES_EXTRA_LIBRARIES ${ZLIB_LIBRARIES})
list(APPEND HERMES_EXTRA_INCLUDES "lib/zstream-cpp")

# googletest (provided, see https://code.google.com/p/googletest/wiki/FAQ
#     Why is it not recommended use a pre-compiled copy of Google Test?)
option(ENABLE_TESTING "Build tests and enable test target" OFF)
if(ENABLE_TESTING)
    set(GOOGLETEST_VERSION 1.10.0)
    include_directories(lib/gtest/googletest/include)
    add_subdirectory(lib/gtest/googletest)
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")
    endif(APPLE)
endif(ENABLE_TESTING)

# FFTW3F (optional for turbulent magnetic fields)
find_package(FFTW3F)
if(FFTW3F_FOUND)
    list(APPEND HERMES_EXTRA_INCLUDES ${FFTW3F_INCLUDE_DIR})
    list(APPEND HERMES_EXTRA_LIBRARIES ${FFTW3F_LIBRARIES})
    add_definitions(-DHERMES_HAVE_FFTW3F)
    list(APPEND HERMES_SWIG_DEFINES -DHERMES_HAVE_FFTW3F)
endif(FFTW3F_FOUND)

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
    else()
        message(FATAL_ERROR "CFITSIO is required.")
    endif(CFITSIO_FOUND)
endif(ENABLE_SYS_CFITSIO)

# HDF5 for importing cosmic ray fluxes from Picard
option(ENABLE_SYS_HDF5 "System HDF5 for hdf5 input" ON)
if(ENABLE_SYS_HDF5)
    find_package(HDF5)
    if(${HDF5_FOUND})
        list(APPEND HERMES_EXTRA_INCLUDES ${HDF5_INCLUDE_DIR})
        list(APPEND HERMES_EXTRA_LIBRARIES ${HDF5_LIBRARIES})
        MESSAGE(STATUS "Parallel hdf5: ${HDF5_IS_PARALLEL} ")
        if(${HDF5_IS_PARALLEL})
            find_package(MPI)
            list(APPEND HERMES_EXTRA_INCLUDES ${MPI_CXX_INCLUDE_PATH})
            list(APPEND HERMES_EXTRA_LIBRARIES ${MPI_CXX_LIBRARIES})
        endif(${HDF5_IS_PARALLEL})
        add_definitions(-DHERMES_HAVE_HDF5)
    endif(${HDF5_FOUND})
endif(ENABLE_SYS_HDF5)

# kiss (provided)
add_subdirectory(lib/kiss)
list(APPEND HERMES_EXTRA_LIBRARIES kiss)
list(APPEND HERMES_EXTRA_INCLUDES lib/kiss/include)

# cparamlib (provided)
# p-p cross-sections
add_subdirectory(lib/cparamlib)
list(APPEND HERMES_EXTRA_INCLUDES lib/cparamlib)
list(APPEND HERMES_EXTRA_LIBRARIES cparamlib)

# ----------------------------------------------------------------------------
# Version info from Git
# ----------------------------------------------------------------------------
# Embed the information about HERMES version into the binaries for easier
# debugging (with a reproducibility in mind)
option(ENABLE_GIT "Embedding information about Hermes version from git" ON)

if(ENABLE_GIT)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    git_describe(GIT_DESC --tags)
endif(ENABLE_GIT)
IF(NOT ENABLE_GIT OR (${GIT_SHA1} STREQUAL "GITDIR-NOTFOUND"))
    set(GIT_REFSPEC "")
    set(GIT_SHA1 "")
    set(GIT_DESC "${HERMES_RELEASE_VERSION}-no-git")
endif()

message(STATUS "Hermes version: ${GIT_DESC} ${GIT_SHA1} ${GIT_REFSPEC}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/src/Version.cpp" @ONLY)
list(APPEND HERMES_EXTRA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/src/Version.cpp")

# ----------------------------------------------------------------------------
# Lint and Covarage 
# ----------------------------------------------------------------------------

# clang-tidy config based on
# https://gitlab.kitware.com/cmake/cmake/issues/18926#note_518804
if(CMAKE_VERSION VERSION_GREATER 3.6)
    option(CMake_RUN_CLANG_TIDY "Run clang-tidy with the compiler." OFF)
    if(CMake_RUN_CLANG_TIDY)
        if(CMake_SOURCE_DIR STREQUAL CMake_BINARY_DIR)
            message(FATAL_ERROR "CMake_RUN_CLANG_TIDY requires an out-of-source build!")
        endif()
        find_program(CLANG_TIDY_COMMAND NAMES "clang-tidy" DOC "Path to clang-tidy executable")
        if(NOT CLANG_TIDY_COMMAND)
            message(WARNING "CMake_RUN_CLANG_TIDY is ON but clang-tidy is not found!")
            set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE)
        else()
            set(CLANG_TIDY_CHECKS "-*,clang-diagnostic-*,clang-analyzer-*,bugprone-*,google-readability-*,performance-*")
            #      set(CLANG_TIDY_CHECKS "-*,performance-*")
            set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND};-checks=${CLANG_TIDY_CHECKS};-config=\
            {\"CheckOptions\":[\
            {\"key\": \"readability-braces-around-statements.ShortStatementLines\", \"value\": 2},\
            {\"key\": \"google-readability-braces-around-statements.ShortStatementLines\", \"value\": 2}\
            ]};-header-filter='${CMAKE_SOURCE_DIR}/src/*'")
        endif()

        # Create a preprocessor definition that depends on .clang-tidy content so
        # the compile command will change when .clang-tidy changes.  This ensures
        # that a subsequent build re-runs clang-tidy on all sources even if they
        # do not otherwise need to be recompiled.  Nothing actually uses this
        # definition.  We add it to targets on which we run clang-tidy just to
        # get the build dependency on the .clang-tidy file.
        file(SHA1 ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy clang_tidy_sha1)
        set(CLANG_TIDY_DEFINITIONS "CLANG_TIDY_SHA1=${clang_tidy_sha1}")
        unset(clang_tidy_sha1)
    endif(CMake_RUN_CLANG_TIDY)
    configure_file(.clang-tidy .clang-tidy COPYONLY)
endif()

OPTION(ENABLE_COVERAGE "Add code for coverage analysis." OFF)
if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    if(LCOV_PATH AND GENHTML_PATH)
        message("Enabling coverage report via $make coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        list(APPEND HERMES_EXTRA_LIBRARIES "-lgcov")
        list(APPEND HERMES_EXTRA_LIBRARIES "-fprofile-arcs")

        if(ENABLE_TESTING)
            SET(COVERAGE_LIBS "-coverage -lgcov")
            add_custom_target(coverage_clean
                COMMAND ${LCOV_PATH} --directory . --zerocounters
                )
            add_custom_target(coverage

                # generate coverage data
                COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info VERBATIM

                # clean external lib
                COMMAND ${LCOV_PATH} --remove coverage.info "/usr/include/*" "/usr/lib/*" "*/lib/*" "*/lib/eigen3/*" "*/test/*" "*/build/*" -o coverage.info.cleaned VERBATIM

                # Generate html output
                COMMAND ${GENHTML_PATH} -o coverageReport coverage.info.cleaned VERBATIM
                COMMAND echo "Generated coverage report in coverageReport/index.html"
                )
        endif(ENABLE_TESTING)

    else(LCOV_PATH AND GENHTML_PATH)
        if(NOT LCOV_PATH)
            message(WARNING "lcov not found, coverage report generation  not possible!")
        endif(NOT LCOV_PATH)
        if(NOT GENHTML_PATH)
            message(WARNING "genhtml not found, coverage report generation not possible!")
        endif(NOT GENHTML_PATH)
    endif(LCOV_PATH AND GENHTML_PATH)
endif(ENABLE_COVERAGE)

# ----------------------------------------------------------------------------
# Library and Binary
# ----------------------------------------------------------------------------

# Configuration of the Units system
option(HERMES_UNIT_SYSTEM_OPTION "Physical units dimensionality checks in variables" ON)
if(HERMES_UNIT_SYSTEM_OPTION)
    message("Units dimensionality check enabled.")
    set(HERMES_UNIT_SYSTEM_DISABLE "" CACHE INTERNAL "A definition string that disables part of Units header")
else(HERMES_UNIT_SYSTEM_OPTION)
    message("Units dimensionality check disabled.")
    set(HERMES_UNIT_SYSTEM_DISABLE "#define HERMES_UNITSDISABLE" CACHE INTERNAL "A definition string that disables part of Units header")
endif(HERMES_UNIT_SYSTEM_OPTION)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/hermes/Units.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/hermes/Units.h" @ONLY)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")
file(GLOB_RECURSE HERMES_INCLUDES RELATIVE ${CMAKE_SOURCE_DIR} include/*.h)
include_directories(include ${HERMES_EXTRA_INCLUDES})

add_library(hermes SHARED
    src/Common.cpp
    src/FITSWrapper.cpp
    src/GridTools.cpp
    src/HEALPixBits.cpp
    src/Hdf5Reader.cpp
    src/ProgressBar.cpp
    src/Random.cpp
    src/Signals.cpp
    src/ionizedgas/HII_Cordes91.cpp
    src/ionizedgas/NE2001Simple.cpp
    src/ionizedgas/YMW16.cpp
    src/cosmicrays/Dragon2D.cpp
    src/cosmicrays/Dragon3D.cpp
    src/cosmicrays/DummyCR.cpp
    src/cosmicrays/Picard3D.cpp
    src/cosmicrays/SimpleCR.cpp
    src/cosmicrays/Sun08.cpp
    src/cosmicrays/WMAP07.cpp
    src/darkmatter/NFWGProfile.cpp
    src/darkmatter/PPPC4DMIDSpectrum.cpp
    src/integrators/BremsstrahlungIntegrator.cpp
    src/integrators/DarkMatterIntegrator.cpp
    src/integrators/DispersionMeasureIntegrator.cpp
    src/integrators/FreeFreeIntegrator.cpp
    src/integrators/InverseComptonIntegrator.cpp
    src/integrators/PiZeroAbsorptionIntegrator.cpp
    src/integrators/PiZeroIntegrator.cpp
    src/integrators/RotationMeasureIntegrator.cpp
    src/integrators/SynchroAbsorptionIntegrator.cpp
    src/integrators/SynchroIntegrator.cpp
    src/interactions/AAfragGamma.cpp
    src/interactions/BreitWheeler.cpp
    src/interactions/BremsstrahlungGALPROP.cpp
    src/interactions/BremsstrahlungTsai74.cpp
    src/interactions/DifferentialCrossSection.cpp
    src/interactions/Kamae06Gamma.cpp
    src/interactions/Kamae06Neutrino.cpp
    src/interactions/KelnerAharonianGamma.cpp
    src/interactions/KelnerAharonianNeutrino.cpp
    src/interactions/KleinNishina.cpp
    src/magneticfields/JF12.cpp
    src/magneticfields/MagneticField.cpp
    src/magneticfields/MagneticFieldGrid.cpp
    src/magneticfields/PT11.cpp
    src/magneticfields/Sun08.cpp
    src/magneticfields/WMAP07.cpp
    src/neutralgas/Nakanishi06.cpp
    src/neutralgas/RingData.cpp
    src/neutralgas/RingModel.cpp
    src/outputs/CTAFormat.cpp
    src/outputs/HEALPixFormat.cpp
    src/photonfields/ISRF.cpp
    src/skymaps/GammaSkymapRange.cpp
    src/skymaps/RadioSkymapRange.cpp
    src/skymaps/Skymap.cpp
    src/skymaps/SkymapMask.cpp
    ${HERMES_EXTRA_SOURCES}
    )
target_link_libraries(hermes ${HERMES_EXTRA_LIBRARIES})

# ----------------------------------------------------------------------------
# Python
# ----------------------------------------------------------------------------
option(ENABLE_PYTHON "Create python library via Pybind11" OFF)
include(FindPython3)
find_package(Python3 COMPONENTS Interpreter Development)

if(ENABLE_PYTHON AND Python3_Development_FOUND)
    include_directories(${Python3_INCLUDE_DIRS})
    set(PYBIND11_PYTHON_VERSION ${Python3_VERSION}) 
    add_subdirectory(lib/pybind11)
    # for PYBIND11_EXPAND_SIDE_EFFECTS in pybind11.h - temporary,
    #   should be removed after the fix lands to the upstream
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wno-unused-value")

    pybind11_add_module(pyhermes
        python/Common.cpp
        python/ParticleID.cpp
        python/PyHermes.cpp
        python/Quantity.cpp
        python/Skymaps.cpp
        python/Vectors.cpp
        python/ionizedgas.cpp
        python/cosmicrays.cpp
        python/darkmatter.cpp
        python/integrators.cpp
        python/interactions.cpp
        python/magneticfields.cpp
        python/neutralgas.cpp
        python/outputs.cpp
        python/photonfields.cpp
        python/units.cpp
        )

    set_target_properties(pyhermes PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pyhermes"
        LINKER_LANGUAGE CXX)
    target_link_libraries(pyhermes PRIVATE hermes)

    install(TARGETS pyhermes LIBRARY DESTINATION "${Python3_SITELIB}")
endif(ENABLE_PYTHON AND Python3_Development_FOUND)

# ------------------------------------------------------------------
# Documentation
# ------------------------------------------------------------------
find_package(Doxygen)
find_program(SPHINX_EXECUTABLE
    NAMES sphinx-build
    DOC "Path to sphinx-build executable")

if(DOXYGEN_FOUND AND SPHINX_EXECUTABLE)
    MESSAGE(STATUS "Found Doxygen and Sphinx to build documentation")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxygenLayout.xml ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml COPYONLY)
    add_custom_target(doxy ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc COMMENT "Generating API documentation with Doxygen" VERBATIM
        )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Sphinx
        "Failed to find sphinx-build executable"
        SPHINX_EXECUTABLE)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/doc)
    set(SPHINX_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
    add_custom_target(doc
        ${SPHINX_EXECUTABLE} -b html -j auto
        -Dbreathe_projects.hermes=${CMAKE_CURRENT_BINARY_DIR}/doc/xml
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx")
    add_dependencies(doc doxy)
else()
    MESSAGE(STATUS "Doxygen and Sphinx not found: cannot build documentation")
endif(DOXYGEN_FOUND AND SPHINX_EXECUTABLE)

# ----------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------
if(ENABLE_TESTING)
    enable_testing()

    add_executable(testPicard3D test/testPicard3D.cpp)
    target_link_libraries(testPicard3D hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testPicard3D testPicard3D)

    add_executable(testHdf5Reader test/testHdf5Reader.cpp)
    target_link_libraries(testHdf5Reader hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testHdf5Reader testHdf5Reader)

    add_executable(testUnits test/testUnits.cpp)
    target_link_libraries(testUnits hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testUnits testUnits)

    add_executable(testCommon test/testCommon.cpp)
    target_link_libraries(testCommon hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testCommon testCommon)

    add_executable(testNumericalIntegration test/testNumericalIntegration.cpp)
    target_link_libraries(testNumericalIntegration hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testNumericalIntegration testNumericalIntegration)

    add_executable(testCacheTools test/testCacheTools.cpp)
    target_link_libraries(testCacheTools hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testCacheTools testCacheTools)

    add_executable(testVector3 test/testVector3.cpp)
    target_link_libraries(testVector3 hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testVector3 testVector3)

    add_executable(testHEALPix test/testHEALPix.cpp)
    target_link_libraries(testHEALPix hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testHEALPix testHEALPix)

    add_executable(testIntegrationMethods test/testIntegrationMethods.cpp)
    target_link_libraries(testIntegrationMethods hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testIntegrationMethods testIntegrationMethods)

    add_executable(testSkymap test/testSkymap.cpp)
    target_link_libraries(testSkymap hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testSkymap testSkymap)

    add_executable(testFITS test/testFITS.cpp)
    target_link_libraries(testFITS hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testFITS testFITS)

    add_executable(testCosmicRays test/testCosmicRays.cpp)
    target_link_libraries(testCosmicRays hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testCosmicRays testCosmicRays)

    add_executable(testYMW16 test/testYMW16.cpp)
    target_link_libraries(testYMW16 hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testYMW16 testYMW16)

    add_executable(testDispersionMeasureIntegrator test/testDispersionMeasureIntegrator.cpp)
    target_link_libraries(testDispersionMeasureIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testDispersionMeasureIntegrator testDispersionMeasureIntegrator)

    add_executable(testRotationMeasureIntegrator test/testRotationMeasureIntegrator.cpp)
    target_link_libraries(testRotationMeasureIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testRotationMeasureIntegrator testRotationMeasureIntegrator)

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

    add_executable(testRingModel test/testRingModel.cpp)
    target_link_libraries(testRingModel hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testRingModel testRingModel)

    add_executable(testPiZeroIntegrator test/testPiZeroIntegrator.cpp)
    target_link_libraries(testPiZeroIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testPiZeroIntegrator testPiZeroIntegrator)

    add_executable(testBremsstrahlungIntegrator test/testBremsstrahlungIntegrator.cpp)
    target_link_libraries(testBremsstrahlungIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testBremsstrahlungIntegrator testBremsstrahlungIntegrator)

    add_executable(testPhotonField test/testPhotonField.cpp)
    target_link_libraries(testPhotonField hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testPhotonField testPhotonField)

    add_executable(testInverseComptonIntegrator test/testInverseComptonIntegrator.cpp)
    target_link_libraries(testInverseComptonIntegrator hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testInverseComptonIntegrator testInverseComptonIntegrator)

    add_executable(testBreitWheeler test/testBreitWheeler.cpp)
    target_link_libraries(testBreitWheeler hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testBreitWheeler testBreitWheeler)

    add_executable(testDarkMatter test/testDarkMatter.cpp)
    target_link_libraries(testDarkMatter hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testDarkMatter testDarkMatter)

    add_executable(testPixelTimes test/testPixelTimes.cpp)
    target_link_libraries(testPixelTimes hermes gtest gtest_main pthread ${HERMES_EXTRA_LIBRARIES})
    add_test(testPixelTimes testPixelTimes)
endif(ENABLE_TESTING)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
add_definitions(-DHERMES_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
install(TARGETS hermes DESTINATION lib)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/data/ DESTINATION share/hermes/data PATTERN ".git" EXCLUDE)
install(DIRECTORY lib/cparamlib/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
install(DIRECTORY lib/kiss/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)

