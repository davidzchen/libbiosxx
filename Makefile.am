EXTRA_DIST = m4
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_MAKEFLAGS = --no-print-directory
AM_CPPFLAGS = -I${top_srcdir}/src
AM_CXXFLAGS = ${my_CXXFLAGS}

include make/pkg.am
include make/libbiospp.am
include make/test.am
include make/doc.am
