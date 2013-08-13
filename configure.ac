#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([libbios++], [1.0.0], [d.zhekai.chen@gmail.com])
AC_CONFIG_SRCDIR([src/bios])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

#------------------------------------------------------------------------------
# Initialize automake
#------------------------------------------------------------------------------
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

#------------------------------------------------------------------------------
# Initialize libtool
#------------------------------------------------------------------------------
LT_INIT
AC_SUBST([biosxx_SO_VERSION], [1:0:0])

#------------------------------------------------------------------------------
# Checks for programs.
#------------------------------------------------------------------------------
AC_PROG_SED
AC_PROG_MKDIR_P
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
	AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([USE_DOXYGEN], [test -n "$DOXYGEN"])
AC_LANG([C++], [C])

#------------------------------------------------------------------------------
# Checks for libraries.
#------------------------------------------------------------------------------
AC_CHECK_LIB([m], [log], [], [AC_MSG_ERROR([Cannot find standard math library])])
AC_CHECK_LIB([gslcblas], [cblas_dgemm], [], [AC_MSG_ERROR([Cannot find cblas library])])
AC_CHECK_LIB([gsl], [gsl_ran_hypergeometric_pdf], [], [AC_MSG_ERROR([Cannot find gsl library])])
AX_BOOST_BASE([1.5], [], [AC_MSG_ERROR([Cannot find boost library])])
AX_BOOST_IOSTREAMS

#------------------------------------------------------------------------------
# Google-test
#------------------------------------------------------------------------------
GTEST_VERSION=1.6.0
if test "x$have_pthread" = "xyes"; then
    GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=1 -I\$(top_srcdir)/ext/gtest -I\$(top_srcdir)/ext/gtest/include"
    GTEST_CXXFLAGS="$PTHREAD_CFLAGS"
    GTEST_LDFLAGS="-L\$(top_builddir)/ext/gtest"
    GTEST_LIBS="$PTHREAD_LIBS"
else
    GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=0 -I\$(top_srcdir)/ext/gtest -I\$(top_srcdir)/ext/gtest/include"
    GTEST_CXXFLAGS=
    GTEST_LDFLAGS="-L\$(top_builddir)/ext/gtest"
    GTEST_LIBS=
fi
AC_SUBST([GTEST_VERSION])
AC_SUBST([GTEST_CPPFLAGS])
AC_SUBST([GTEST_CXXFLAGS])
AC_SUBST([GTEST_LDFLAGS])
AC_SUBST([GTEST_LIBS])

#------------------------------------------------------------------------------
# Checks for header files.
#------------------------------------------------------------------------------
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

#------------------------------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.
#------------------------------------------------------------------------------
AC_TYPE_SIZE_T

#------------------------------------------------------------------------------
# Checks for library functions.
#------------------------------------------------------------------------------
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow sqrt strchr strdup strerror strrchr strstr strtol memmem])

#------------------------------------------------------------------------------
# Compile flags
#------------------------------------------------------------------------------
my_CXXFLAGS="-Wall -Werror"
AC_SUBST([my_CXXFLAGS])

#------------------------------------------------------------------------------
# Output
#------------------------------------------------------------------------------
AM_COND_IF([USE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
AC_MSG_RESULT([
    $PACKAGE $VERSION
    =====

    prefix:       ${prefix}
    sysconfdir:   ${sysconfdir}
    libdir:       ${libdir}
    includedir:   ${includedir}

    compiler:     ${CXX}
    cxxflags:     ${CXXFLAGS}
    ldflags:      ${LDFLAGS}
])
