## Process this file with automake to produce Makefile.in
#
# Author: niklas@slac.stanford.edu
#

cparamlibdir = $(includedir)/cparamlib

cparamlib_HEADERS = cparamlib.h
cparamlib_SOURCE = antinue.c antinumu.c elec.c gamma.c nue.c numu.c posi.c sigma.c
LIB_VERSION = 2:3:0

library_includedir=$(includedir)/cparamlib

AM_CPPFLAGS = -I$(top_srcdir)

lib_LTLIBRARIES= libcparamlib.la
libcparamlib_la_SOURCES= $(cparamlib_HEADERS) $(cparamlib_SOURCE)
libcparamlib_la_LDFLAGS= -version-info $(LIB_VERSION)
