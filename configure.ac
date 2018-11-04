#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([adalm-beacon], [1.0], [andrew@vaelen.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_SEARCH_LIBS([iio_context_find_device], [iio], [], [
  AC_MSG_ERROR([required library libiio not found])
])
AC_SEARCH_LIBS([sin], [m], [], [
  AC_MSG_ERROR([required library libm not found])
])
AC_SEARCH_LIBS([ad9361_set_bb_rate], [ad9361], [], [
  AC_MSG_ERROR([required library libad9361 not found])
])

# Checks for header files.
AC_CHECK_HEADER([iio.h], [], [
  AC_MSG_ERROR([required header iio.h not found])
])
AC_CHECK_HEADER([math.h], [], [
  AC_MSG_ERROR([required header math.h not found])
])
AC_CHECK_HEADER([ad9361.h], [], [
  AC_MSG_ERROR([required header ad9361.h not found])
])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
