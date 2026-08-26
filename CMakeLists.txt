cmake_minimum_required(VERSION 3.20)
project(hermes VERSION 1.1.0 LANGUAGES C CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(HERMES_RELEASE_VERSION ${PROJECT_VERSION})

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(HERMES_EXTRA_SOURCES)
set(HERMES_PUBLIC_LIBRARIES GSL::gsl GSL::gslcblas)
set(HERMES_PRIVATE_LIBRARIES Threads::Threads ZLIB::ZLIB kiss cparamlib ${CMAKE_DL_LIBS})
set(HERMES_PUBLIC_DEFINITIONS)
set(HERMES_PRIVATE_COMPILE_OPTIONS)
set(HERMES_PRIVATE_LINK_OPTIONS)

set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build-type to release to enable performance improvements
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

option(HERMES_ENABLE_FAST_MATH "Enable non-IEEE fast-math optimizations" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT APPLE)
    list(APPEND HERMES_PRIVATE_LINK_OPTIONS "-Wl,--as-needed")
    message(STATUS "Use --as-needed linker flags!")
endif()

# ----------------------------------------------------------------------------
# Relocatable runtime paths
# ----------------------------------------------------------------------------
option(USE_ABSOLUTE_RPATH "Embed the configured install library directory" OFF)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
if(USE_ABSOLUTE_RPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
else()
    set(CMAKE_INSTALL_RPATH "")
endif()

# ---------------------------------------------------------------------------
# Download data files
# ----------------------------------------------------------------------------
option(DOWNLOAD_DATA "Download the pinned HERMES data archive during configuration" OFF)
set(HERMES_DATA_URL
    "https://github.com/HERMES-SkyMaps/hermes-data/releases/download/v1.0.0/hermes-data-1.0.0.tar.gz"
    CACHE STRING "URL of the HERMES data archive")
set(HERMES_DATA_SHA256
    "cf7d27aef8eca54c00e194f5b56eb23f982e0e8c40a3db374308c98d90e801d6"
    CACHE STRING "SHA-256 checksum of the HERMES data archive")
set(HERMES_DATA_ARCHIVE "${CMAKE_BINARY_DIR}/hermes-data-1.0.0.tar.gz"
    CACHE FILEPATH "Local HERMES data archive")
set(HERMES_DATA_SOURCE_DIR "${CMAKE_BINARY_DIR}/data"
    CACHE PATH "Directory containing unpacked HERMES runtime data")

if(DOWNLOAD_DATA)
    message(STATUS "Downloading the pinned HERMES data archive (~282 MiB)")
    file(DOWNLOAD
        "${HERMES_DATA_URL}"
        "${HERMES_DATA_ARCHIVE}"
        EXPECTED_HASH "SHA256=${HERMES_DATA_SHA256}"
        STATUS HERMES_DATA_DOWNLOAD_STATUS
        TIMEOUT 600
        INACTIVITY_TIMEOUT 60
        TLS_VERIFY ON
        SHOW_PROGRESS)
    list(GET HERMES_DATA_DOWNLOAD_STATUS 0 HERMES_DATA_DOWNLOAD_CODE)
    list(GET HERMES_DATA_DOWNLOAD_STATUS 1 HERMES_DATA_DOWNLOAD_MESSAGE)
    if(NOT HERMES_DATA_DOWNLOAD_CODE EQUAL 0)
        message(FATAL_ERROR "HERMES data download failed: ${HERMES_DATA_DOWNLOAD_MESSAGE}")
    endif()

    message(STATUS "Extracting the HERMES data archive")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${HERMES_DATA_ARCHIVE}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        RESULT_VARIABLE HERMES_DATA_EXTRACT_STATUS)
    if(NOT HERMES_DATA_EXTRACT_STATUS EQUAL 0)
        message(FATAL_ERROR "Failed to extract ${HERMES_DATA_ARCHIVE}")
    endif()
elseif(EXISTS "${HERMES_DATA_ARCHIVE}" AND NOT IS_DIRECTORY "${HERMES_DATA_SOURCE_DIR}")
    file(SHA256 "${HERMES_DATA_ARCHIVE}" HERMES_LOCAL_DATA_SHA256)
    if(HERMES_LOCAL_DATA_SHA256 STREQUAL HERMES_DATA_SHA256)
        message(STATUS "Extracting the existing verified HERMES data archive")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${HERMES_DATA_ARCHIVE}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)
    else()
        message(WARNING "Ignoring ${HERMES_DATA_ARCHIVE}: checksum mismatch")
    endif()
endif()

if(IS_DIRECTORY "${HERMES_DATA_SOURCE_DIR}")
    message(STATUS "HERMES runtime data: ${HERMES_DATA_SOURCE_DIR}")
else()
    message(STATUS "HERMES runtime data not present; use -DDOWNLOAD_DATA=ON or set HERMES_DATA_PATH at runtime")
endif()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# C++ Threads required
find_package(Threads REQUIRED)

# GSL required
find_package(GSL REQUIRED)

# zlib required / zstream-cpp provided
find_package(ZLIB REQUIRED)

# googletest (provided, see https://code.google.com/p/googletest/wiki/FAQ
#     Why is it not recommended use a pre-compiled copy of Google Test?)
option(ENABLE_TESTING "Build tests and enable test target" OFF)
if(ENABLE_TESTING)
    set(GOOGLETEST_VERSION 1.10.0)
    add_subdirectory(lib/gtest/googletest)
endif()

# FFTW3F (optional for turbulent magnetic fields)
find_package(FFTW3F)
if(FFTW3F_FOUND)
    list(APPEND HERMES_PUBLIC_LIBRARIES FFTW3F::FFTW3F)
    list(APPEND HERMES_PUBLIC_DEFINITIONS HERMES_HAVE_FFTW3F)
endif()

# CFITSIO is part of the core interface: RingModel and the FITS output classes
# are used by sources that are always built and by installed public headers.
find_package(CFITSIO REQUIRED)
list(APPEND HERMES_PUBLIC_LIBRARIES CFITSIO::CFITSIO)
list(APPEND HERMES_PUBLIC_DEFINITIONS HERMES_HAVE_CFITSIO)

# Unmaintained HDF5/PICARD support. Keep this opt-in so ordinary builds and
# installations do not expose or package the legacy reader, model, or test data.
option(ENABLE_SYS_HDF5 "Build and install unmaintained PICARD/HDF5 support" OFF)
if(ENABLE_SYS_HDF5)
    message(WARNING "PICARD/HDF5 support is unmaintained and enabled explicitly")
    find_package(HDF5 REQUIRED COMPONENTS C)
    list(APPEND HERMES_PUBLIC_LIBRARIES HDF5::HDF5)
    list(APPEND HERMES_PUBLIC_DEFINITIONS HERMES_HAVE_HDF5)
    list(APPEND HERMES_EXTRA_SOURCES
        src/Hdf5Reader.cpp
        src/cosmicrays/Picard3D.cpp)
    message(STATUS "Parallel HDF5: ${HDF5_IS_PARALLEL}")
    if(HDF5_IS_PARALLEL)
        find_package(MPI REQUIRED COMPONENTS CXX)
        list(APPEND HERMES_PUBLIC_LIBRARIES MPI::MPI_CXX)
    endif()
endif()

# kiss (provided)
add_subdirectory(lib/kiss)

# cparamlib (provided)
# p-p cross-sections
add_subdirectory(lib/cparamlib)

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
        list(APPEND HERMES_PRIVATE_COMPILE_OPTIONS --coverage)
        list(APPEND HERMES_PRIVATE_LINK_OPTIONS --coverage)

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

if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(HERMES_ABSOLUTE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
    set(HERMES_ABSOLUTE_INSTALL_LIBDIR "/${CMAKE_INSTALL_LIBDIR}")
endif()
if(IS_ABSOLUTE "${CMAKE_INSTALL_DATADIR}")
    set(HERMES_ABSOLUTE_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}")
else()
    set(HERMES_ABSOLUTE_INSTALL_DATADIR "/${CMAKE_INSTALL_DATADIR}")
endif()
file(RELATIVE_PATH HERMES_INSTALL_DATA_RELATIVE_TO_LIBRARY
    "${HERMES_ABSOLUTE_INSTALL_LIBDIR}"
    "${HERMES_ABSOLUTE_INSTALL_DATADIR}/hermes/data")

add_library(hermes SHARED
    src/Common.cpp
    src/FITSWrapper.cpp
    src/GridTools.cpp
    src/HEALPixBits.cpp
    src/ProgressBar.cpp
    src/Random.cpp
    src/Signals.cpp
    src/ionizedgas/HII_Cordes91.cpp
    src/ionizedgas/NE2001Simple.cpp
    src/ionizedgas/YMW16.cpp
    src/cosmicrays/Dragon2D.cpp
    src/cosmicrays/Dragon3D.cpp
    src/cosmicrays/DummyCR.cpp
    src/cosmicrays/SimpleCR.cpp
    src/cosmicrays/Sun08.cpp
    src/cosmicrays/TabulatedCR.cpp
    src/cosmicrays/UHECR.cpp
    src/cosmicrays/WMAP07.cpp
    src/darkmatter/HDMSpectraSpectrum.cpp
    src/darkmatter/NFWGProfile.cpp
    src/darkmatter/PPPC4DMIDSpectrum.cpp
    src/integrators/BremsstrahlungIntegrator.cpp
    src/integrators/DarkMatterIntegrator.cpp
    src/integrators/DarkMatterNeutronIntegrator.cpp
    src/integrators/DispersionMeasureIntegrator.cpp
    src/integrators/FreeFreeIntegrator.cpp
    src/integrators/InverseComptonIntegrator.cpp
    src/integrators/PiZeroAbsorptionIntegrator.cpp
    src/integrators/PiZeroIntegrator.cpp
    src/integrators/PiZeroNeutronDecayIntegrator.cpp
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
    src/interactions/UheYields.cpp
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
add_library(Hermes::hermes ALIAS hermes)

target_compile_features(hermes PUBLIC cxx_std_17)
target_include_directories(hermes
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/cparamlib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/kiss/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/zstream-cpp)
target_compile_definitions(hermes
    PUBLIC ${HERMES_PUBLIC_DEFINITIONS}
    PRIVATE
        HERMES_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
        HERMES_INSTALL_DATA_RELATIVE_TO_LIBRARY="${HERMES_INSTALL_DATA_RELATIVE_TO_LIBRARY}"
        HERMES_BUILD_DATA_PATH="${HERMES_DATA_SOURCE_DIR}")
target_link_libraries(hermes
    PUBLIC ${HERMES_PUBLIC_LIBRARIES}
    PRIVATE ${HERMES_PRIVATE_LIBRARIES})
if(HERMES_PRIVATE_COMPILE_OPTIONS)
    target_compile_options(hermes PRIVATE ${HERMES_PRIVATE_COMPILE_OPTIONS})
endif()
if(HERMES_PRIVATE_LINK_OPTIONS)
    target_link_options(hermes PRIVATE ${HERMES_PRIVATE_LINK_OPTIONS})
endif()
if(HERMES_ENABLE_FAST_MATH AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(hermes PRIVATE -ffast-math)
endif()
set_target_properties(hermes PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME hermes)

# ----------------------------------------------------------------------------
# Python
# ----------------------------------------------------------------------------
option(ENABLE_PYTHON "Create python library via Pybind11" ON)
if(ENABLE_PYTHON)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    set(PYBIND11_FINDPYTHON ON)
    add_subdirectory(lib/pybind11)

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
    target_compile_options(pyhermes PRIVATE
        $<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>>:-Wno-unused-value>)

    set(HERMES_PYTHON_INSTALL_DIR
        "${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages"
        CACHE STRING "Prefix-relative installation directory for pyhermes")
    if(NOT USE_ABSOLUTE_RPATH)
        if(APPLE)
            set_target_properties(pyhermes PROPERTIES INSTALL_RPATH "@loader_path/../..")
        elseif(UNIX)
            set_target_properties(pyhermes PROPERTIES INSTALL_RPATH "$ORIGIN/../..")
        endif()
    endif()
    install(TARGETS pyhermes
        LIBRARY DESTINATION "${HERMES_PYTHON_INSTALL_DIR}"
        COMPONENT Python)
endif()

# ----------------------------------------------------------------------------
# Testing
# ----------------------------------------------------------------------------
if(ENABLE_TESTING)
    enable_testing()

    if(ENABLE_PYTHON)
        add_test(NAME testPythonImport
            COMMAND "${Python3_EXECUTABLE}" -c
                "import pyhermes; assert hasattr(pyhermes.cosmicrays, 'TabulatedCR')")
        set_tests_properties(testPythonImport PROPERTIES
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:pyhermes>")

        add_test(NAME testPythonBindings
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/test/testPythonBindings.py")
        set_tests_properties(testPythonBindings PROPERTIES
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:pyhermes>")
    endif()

    if(ENABLE_SYS_HDF5)
        add_executable(testPicard3D test/testPicard3D.cpp)
        target_link_libraries(testPicard3D PRIVATE Hermes::hermes gtest_main Threads::Threads)
        add_test(testPicard3D testPicard3D)

        add_executable(testHdf5Reader test/testHdf5Reader.cpp)
        target_link_libraries(testHdf5Reader PRIVATE Hermes::hermes gtest_main Threads::Threads)
        add_test(testHdf5Reader testHdf5Reader)
    endif()

    add_executable(testUnits test/testUnits.cpp)
    target_link_libraries(testUnits PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testUnits testUnits)

    add_executable(testCommon test/testCommon.cpp)
    target_link_libraries(testCommon PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testCommon testCommon)

    add_executable(testNumericalIntegration test/testNumericalIntegration.cpp)
    target_link_libraries(testNumericalIntegration PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testNumericalIntegration testNumericalIntegration)

    add_executable(testCacheTools test/testCacheTools.cpp)
    target_link_libraries(testCacheTools PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testCacheTools testCacheTools)

    add_executable(testVector3 test/testVector3.cpp)
    target_link_libraries(testVector3 PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testVector3 testVector3)

    add_executable(testGridTools test/testGridTools.cpp)
    target_link_libraries(testGridTools PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testGridTools testGridTools)

    add_executable(testHEALPix test/testHEALPix.cpp)
    target_link_libraries(testHEALPix PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testHEALPix testHEALPix)

    add_executable(testIntegrationMethods test/testIntegrationMethods.cpp)
    target_link_libraries(testIntegrationMethods PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testIntegrationMethods testIntegrationMethods)

    add_executable(testSkymap test/testSkymap.cpp)
    target_link_libraries(testSkymap PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testSkymap testSkymap)

    add_executable(testFITS test/testFITS.cpp)
    target_link_libraries(testFITS PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testFITS testFITS)

    add_executable(testCosmicRays test/testCosmicRays.cpp)
    target_link_libraries(testCosmicRays PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testCosmicRays testCosmicRays)

    add_executable(testTabulatedCR test/testTabulatedCR.cpp)
    target_link_libraries(testTabulatedCR PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testTabulatedCR testTabulatedCR)

    add_executable(testUHECR test/testUHECR.cpp)
    target_link_libraries(testUHECR PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testUHECR testUHECR)

    add_executable(testUheYields test/testUheYields.cpp)
    target_link_libraries(testUheYields PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testUheYields testUheYields)

    add_executable(testYMW16 test/testYMW16.cpp)
    target_link_libraries(testYMW16 PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testYMW16 testYMW16)

    add_executable(testDispersionMeasureIntegrator test/testDispersionMeasureIntegrator.cpp)
    target_link_libraries(testDispersionMeasureIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testDispersionMeasureIntegrator testDispersionMeasureIntegrator)

    add_executable(testRotationMeasureIntegrator test/testRotationMeasureIntegrator.cpp)
    target_link_libraries(testRotationMeasureIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testRotationMeasureIntegrator testRotationMeasureIntegrator)

    add_executable(testSynchroIntegrator test/testSynchroIntegrator.cpp)
    target_link_libraries(testSynchroIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testSynchroIntegrator testSynchroIntegrator)

    add_executable(testFreeFreeIntegrator test/testFreeFreeIntegrator.cpp)
    target_link_libraries(testFreeFreeIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testFreeFreeIntegrator testFreeFreeIntegrator)

    add_executable(testMFieldModels test/testMFieldModels.cpp)
    target_link_libraries(testMFieldModels PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testMFieldModels testMFieldModels)

    add_executable(testSynchroAbsorptionIntegrator test/testSynchroAbsorptionIntegrator.cpp)
    target_link_libraries(testSynchroAbsorptionIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testSynchroAbsorptionIntegrator testSynchroAbsorptionIntegrator)

    add_executable(testInteractions test/testInteractions.cpp)
    target_link_libraries(testInteractions PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testInteractions testInteractions)

    add_executable(testRingModel test/testRingModel.cpp)
    target_link_libraries(testRingModel PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testRingModel testRingModel)

    add_executable(testPiZeroIntegrator test/testPiZeroIntegrator.cpp)
    target_link_libraries(testPiZeroIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testPiZeroIntegrator testPiZeroIntegrator)

    add_executable(testBremsstrahlungIntegrator test/testBremsstrahlungIntegrator.cpp)
    target_link_libraries(testBremsstrahlungIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testBremsstrahlungIntegrator testBremsstrahlungIntegrator)

    add_executable(testPhotonField test/testPhotonField.cpp)
    target_link_libraries(testPhotonField PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testPhotonField testPhotonField)

    add_executable(testInverseComptonIntegrator test/testInverseComptonIntegrator.cpp)
    target_link_libraries(testInverseComptonIntegrator PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testInverseComptonIntegrator testInverseComptonIntegrator)

    add_executable(testBreitWheeler test/testBreitWheeler.cpp)
    target_link_libraries(testBreitWheeler PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testBreitWheeler testBreitWheeler)

    add_executable(testDarkMatter test/testDarkMatter.cpp)
    target_link_libraries(testDarkMatter PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testDarkMatter testDarkMatter)

    add_executable(testPixelTimes test/testPixelTimes.cpp)
    target_link_libraries(testPixelTimes PRIVATE Hermes::hermes gtest_main Threads::Threads)
    add_test(testPixelTimes testPixelTimes)
endif(ENABLE_TESTING)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
set(HERMES_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Hermes"
    CACHE STRING "Installation directory for the HERMES CMake package")
option(HERMES_INSTALL_DATA "Install available HERMES runtime data" ON)

install(TARGETS hermes
    EXPORT HermesTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development)

if(ENABLE_SYS_HDF5)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Development FILES_MATCHING PATTERN "*.h")
else()
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Development
        FILES_MATCHING PATTERN "*.h"
        PATTERN "Hdf5Reader.h" EXCLUDE
        PATTERN "Picard3D.h" EXCLUDE)
endif()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
    FILES_MATCHING PATTERN "*.h")

if(HERMES_INSTALL_DATA)
    if(ENABLE_SYS_HDF5)
        install(DIRECTORY "${HERMES_DATA_SOURCE_DIR}/"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/hermes/data
            COMPONENT Data
            OPTIONAL
            PATTERN ".git" EXCLUDE)
    else()
        install(DIRECTORY "${HERMES_DATA_SOURCE_DIR}/"
            DESTINATION ${CMAKE_INSTALL_DATADIR}/hermes/data
            COMPONENT Data
            OPTIONAL
            PATTERN ".git" EXCLUDE
            PATTERN "Picard3DTest" EXCLUDE)
    endif()
endif()

configure_package_config_file(
    cmake/HermesConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HermesConfig.cmake
    INSTALL_DESTINATION ${HERMES_INSTALL_CMAKEDIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/HermesConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(EXPORT HermesTargets
    FILE HermesTargets.cmake
    NAMESPACE Hermes::
    DESTINATION ${HERMES_INSTALL_CMAKEDIR}
    COMPONENT Development)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/HermesConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HermesConfigVersion.cmake
    cmake/FindCFITSIO.cmake
    cmake/FindFFTW3F.cmake
    DESTINATION ${HERMES_INSTALL_CMAKEDIR}
    COMPONENT Development)
install(FILES README.md INSTALL.md LICENSE
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT Documentation)

set(CPACK_PACKAGE_NAME "HERMES")
set(CPACK_PACKAGE_VENDOR "HERMES Development Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-Energy Radiative MESsengers")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR TGZ)
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/build[^/]*/"
    "/\\.idea/"
    "/\\.vscode/"
    "~$")
include(CPack)
