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

# Checks for header files.
AC_CHECK_HEADER([iio.h], [], [
  AC_MSG_ERROR([required header iio.h not found])
])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
