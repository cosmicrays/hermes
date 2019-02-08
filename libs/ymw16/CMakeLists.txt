cmake_minimum_required(VERSION 2.8)

project(YMW16 LANGUAGES C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ymw16 STATIC
	dmdtau.c
	dora.c
	fermibubble.c
	frb_d.c
	galcen.c
	gum.c
	lmc.c
	localbubble.c
	ne_crd.c
	nps.c
	smc.c
	spiral.c
	thick.c
	thin.c
	ymw16.c
	ymw16_ne.c
	ymw16par_h.c
)

set_target_properties(ymw16 PROPERTIES COMPILE_FLAGS -fPIC)
