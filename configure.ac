# Process this file with autoconf to produce a configure script.

# Init autoconf and automake.
AC_PREREQ(2.59)
AC_INIT(SoTerrain, 0.5, blackhex@post.cz)
AC_CONFIG_SRCDIR([src/SoTerrainTest.cpp])
AC_LANG([C++])
AM_INIT_AUTOMAKE

# Subtitution variables
SO_TERRAIN_VERSION="0.5"
PR_PROFILER_VERSION="0.1"
AC_SUBST([SO_TERRAIN_VERSION])
AC_SUBST([PR_PROFILER_VERSION])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT64_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC
AC_CHECK_FUNCS([gettimeofday putenv sqrt getopt])

# Set compiler flags accordingly to specified libraries
AC_ARG_WITH([coin], [AS_HELP_STRING([--with-coin],
 [set location of Coin library (default=/usr)])], [COIN_PATH=$withval],
 [COIN_PATH=/usr])
AC_SUBST([COIN_PATH])
AC_ARG_WITH([soqt], [AS_HELP_STRING([--with-soqt],
 [set location of SoQt library (default=/usr)])], [SOQT_PATH=$withval],
 [SOQT_PATH=/usr])
AC_SUBST([SOQT_PATH])
AC_ARG_WITH([simage], [AS_HELP_STRING([--with-simage],
 [set location of simage library (default=/usr)])], [SIMAGE_PATH=$withval],
 [SIMAGE_PATH=/usr])
AC_SUBST([SIMAGE_PATH])

# Check for simage-config and soqt-config files
AC_CHECK_FILE([$SIMAGE_PATH/bin/simage-config], [],
  [AC_MSG_FAILURE([simage-config not found. Try to specify correct path to simage libary using --with-simage=<simage_path> option.])])
AC_CHECK_FILE([$SOQT_PATH/bin/soqt-config], [], 
  [AC_MSG_FAILURE([soqt-config not found. Try to specify correct path to SoQt libary using --with-soqt=<soqt_path> option.])])

# Set building variables.
CXXFLAGS="`$SOQT_PATH/bin/soqt-config --cppflags` -I./include/ -Wall \
  -Wno-long-long -ansi"
LDFLAGS="`$SOQT_PATH/bin/soqt-config --ldflags` \
 `$SOQT_PATH/bin/soqt-config --libs` `$SIMAGE_PATH/bin/simage-config --libs`"

# Compilation with debugging informations.
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],
  [compile with debugging informations (default=no)])])
if test "$enable_debug" = "yes"; then
  CXXFLAGS="-g3 -ggdb -DDEBUG $CXXFLAGS"
  LDFLAGS="-g3 -ggdb $LDFLAGS"
else
  CXXFLAGS="-O2 $CXXFLAGS"
  LDFLAGS="-O2 $LDFLAGS"
fi

# Compilation with profiling informations.
AC_ARG_ENABLE(profile, [AS_HELP_STRING([--enable-profile],
  [compile with profiling informations (default=no)])])
if test "$enable_debug" = "yes"; then
  CXXFLAGS="-pg $CXXFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Compilation with own profiler.
AC_ARG_ENABLE(prprofiler, [AS_HELP_STRING([--enable-prprofiler],
 [compile with PrProfiler library (default=no)])])
if test "$enable_prprofiler" = "yes"; then
  CXXFLAGS="-DPROFILE $CXXFLAGS"
fi


# Checks for libraries.
AC_CHECK_LIB([simage], [printf], [],
  [AC_MSG_FAILURE([Can't link against simage library. Try to specify correct path to simage libary using --with-simage=<simage_path> option.])])
AC_CHECK_LIB([Coin], [printf], [],
  [AC_MSG_FAILURE([Can't link against Coin library. Try to specify correct path to Coin libary using --with-coin=<coin_path> option.])])
AC_CHECK_LIB([SoQt], [printf], [],
  [AC_MSG_FAILURE([Can't link against SoQt library. Try to specify correct path to SoQt libary using --with-soqt=<soqt_path> option.])])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
