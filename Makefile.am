
SUBDIRS = src po

EXTRA_DIST = config.rpath m4/ChangeLog  AUTHORS TODO README configure INSTALL.linux INSTALL.macosx gallery.tar surf-for-surfer.tar.gz

pg_DATA = gallery.tar

gallery-install:
	tar xvf $(DESTDIR)$(pkgdatadir)/gallery.tar -C $(DESTDIR)$(pkgdatadir)
	rm $(DESTDIR)$(pkgdatadir)/gallery.tar

gallery-uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/gallery



install-data-hook: gallery-install
	
uninstall-hook: gallery-uninstall

ACLOCAL_AMFLAGS = -I m4
