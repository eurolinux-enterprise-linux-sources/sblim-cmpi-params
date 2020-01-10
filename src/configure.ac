#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(SBLIM Param Providers, 1.3.0, sblim-devel@lists.sourceforge.net,sblim-cmpi-params)
AC_CONFIG_SRCDIR([Linux_ABIParameter.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# Configuration Arguments
AC_ARG_VAR([TESTSUITEDIR],[the directory where the SBLIM testsuite resides.])
AC_ARG_VAR([PROVIDERDIR],[the directory where the CMPI providers will be installed.])
AC_ARG_VAR([CIMSERVER],[the target CIM server (pegasus|sfcb|openwbem|sniacimom).])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
                                                                                
# Checks for typedefs, structures, and compiler characteristics.
                                                                                
# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([strchr])

# Check for required CMPI header files (defined in acinclude.m4)
CHECK_CMPI

# Check for required libraries (defined in acinclude.m4)
CHECK_PROVIDERDIR

# Check for CIM Server (defined in acinclude.m4)
CHECK_CIMSERVER

# Check for SBLIM Base
CHECK_SBLIM_BASE

# Check for SBLIM Testsuite
CHECK_TESTSUITE

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile sblim-cmpi-params.spec])

if test x"$TESTSUITEDIR" == x; then
   TESTSUITE_SUPPORT=Disabled
else	
   TESTSUITE_SUPPORT=Enabled
fi

# Extend CPPFLAGS
if echo $CPPFLAGS | grep -v CMPI_VERSION
then
   CPPFLAGS="$CPPFLAGS -DCMPI_VERSION=90"
fi

# Display configuration options
echo "-------------------------------------------------------"
echo "Configuration for $PACKAGE complete."
echo ""
echo "The following configuration options have been selected:"
echo "CIMSERVER: " $CIMSERVER
echo "PROVIDERDIR: " $PROVIDERDIR
echo "CPPFLAGS:" $CPPFLAGS
echo "Test Suite Support" $TESTSUITE_SUPPORT
if test x"$TESTSUITEDIR" != x; then
echo "TESTSUITEDIR " $TESTSUITEDIR
fi
echo "-------------------------------------------------------"

AC_OUTPUT

echo "You may now run make"
