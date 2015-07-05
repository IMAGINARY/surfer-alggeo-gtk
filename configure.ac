dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
##LT_PREREQ([2.4.2])
AC_INIT([surfer],[0.2.0~12],
	[info@imaginary.org],[surfer-alggeo-gtk],
	[http://imaginary.org/])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/surfer.cc])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([gnu dist-bzip2 dist-xz dist-zip std-options])
AM_SILENT_RULES([no])
AM_MAINTAINER_MODE([enable])
AM_GNU_GETTEXT_VERSION([0.19.3])
AM_GNU_GETTEXT([external])

AC_CANONICAL_HOST

surfer_AC_SURFER_GALLERY_SETUP

AC_PROG_SED
AC_PROG_LN_S
AC_PROG_INSTALL

##AC_PROG_CC_STDC
AC_PROG_CC_C99
AC_LANG(C++)
AC_PROG_CXX

AC_PATH_PROG([HELP2MAN],[help2man])

##LT_INIT

AC_CHECK_PROGS([SURF_CMD],[surf-alggeo surf],[surf])

PKG_PROG_PKG_CONFIG
LT_LIB_M
PKG_CHECK_MODULES([LIBGTKMM],[gtkmm-2.4 >= 2.8])

AC_HEADER_DIRENT
AC_CHECK_HEADERS([float.h stddef.h stdlib.h string.h unistd.h])
AC_CHECK_FUNCS([setenv])
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([pow sqrt])

AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	po/Makefile.in
	gallery/gallery-cfg.mk
	gallery/Makefile
	gallery/pool/Makefile
	gallery/pool/pic/Makefile
	gallery/pool/ppm/Makefile
	doc/Makefile
	])
AC_OUTPUT
dnl eof
