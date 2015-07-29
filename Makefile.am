
ACLOCAL_AMFLAGS = -I m4

AM_DISTCHECK_CONFIGURE_FLAGS ?= \
	--disable-maintainer-mode

SUBDIRS = src po gallery doc

uninstall-hook:
	@if test -d $(pkgdatadir) ; then rmdir --i $(pkgdatadir) ; fi

EXTRA_DIST = \
	INSTALL.linux \
	INSTALL.macosx

