export abs_top_builddir abs_top_srcdir

SUBDIRS = pool

GMKM_SUBDIRS = gallery-en gallery-de gallery-es #gallery-ca gallery-ga

install-data-local:
	$(foreach _gal,$(GMKM_SUBDIRS), $(MAKE) -C $(srcdir)/$(_gal) $(AM_MAKEFLAGS) install-data ;)

uninstall-local:
	$(foreach _gal,$(GMKM_SUBDIRS), $(MAKE) -C $(srcdir)/$(_gal) $(AM_MAKEFLAGS) uninstall ;)

dist-hook:
	$(foreach _gal,$(GMKM_SUBDIRS), $(MAKE) -C $(srcdir)/$(_gal) $(AM_MAKEFLAGS) distdir ;)

uninstall-hook:
	@if test -d $(gallerydir) ; then rmdir --i $(gallerydir) ; fi

EXTRA_DIST = gallery-def.mk gallerymakefile.mk gallerycollectionmakefile.mk
