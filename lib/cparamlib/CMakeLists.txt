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

target_include_directories(cparamlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(cparamlib PROPERTIES POSITION_INDEPENDENT_CODE ON)
