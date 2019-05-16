cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(cparamlib STATIC
    antinue.c
    antinumu.c
    elec.c
    gamma.c
    nue.c
    numu.c
    posi.c
    sigma.c
)

SET_TARGET_PROPERTIES(cparamlib PROPERTIES COMPILE_FLAGS -fPIC)

