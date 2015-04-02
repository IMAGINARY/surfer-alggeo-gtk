
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src po gallery

uninstall-hook:
	@if test -d $(pkgdatadir) ; then rmdir --i $(pkgdatadir) ; fi

EXTRA_DIST = \
	INSTALL.linux \
	INSTALL.macosx

