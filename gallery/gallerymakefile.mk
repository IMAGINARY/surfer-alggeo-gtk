include $(abs_top_builddir)/gallery/gallery-cfg.mk
include $(abs_top_srcdir)/gallery/gallery-def.mk

gallery_tuple = $(call surfer_gallery_get_tuple)
gt_tllc = $(firstword $(gallery_tuple))

gallery_reldir = $(call surfer_gallery_get_relfolder,$(gt_tllc))
gallery_dir = $(gallerydir)/$(gallery_reldir)
abs_srcdir=$(abs_top_srcdir)/gallery/$(gallery_reldir)

EXTRA_DIST = GNUmakefile

GMKM_SUBDIRS = $(SURFER_GALLERY_LISTOF_COLLECTION)

%:
	@echo $@
	# neutralized

all:

install: install-data

install-data:
	$(call surfer_gallery_populate,$(gt_tllc))
	$(foreach _col,$(GMKM_SUBDIRS), $(MAKE) -C $(abs_srcdir)/$(_col) $(AM_MAKEFLAGS) install-data ;)

uninstall-data:
	$(foreach _col,$(GMKM_SUBDIRS), $(MAKE) -C $(abs_srcdir)/$(_col) $(AM_MAKEFLAGS) uninstall ;)
	$(call surfer_gallery_unpopulate,$(gt_tllc))

uninstall: uninstall-data
	$(if $(wildcard $(gallery_dir)),$(shell rmdir --i $(gallery_dir)),)

distdir:
	$(call surfer_gallery_distribute,$(gt_tllc))
	$(foreach _col,$(GMKM_SUBDIRS), $(MAKE) -C $(abs_srcdir)/$(_col) $(AM_MAKEFLAGS) distdir ;)

.PHONY: install uninstall install-data distdir

# vim:syntax=make
