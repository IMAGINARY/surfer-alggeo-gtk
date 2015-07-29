include $(abs_top_builddir)/gallery/gallery-cfg.mk
include $(abs_top_srcdir)/gallery/gallery-def.mk

gallery_collection_tuple = $(call surfer_gallery_collection_get_tuple)
gct_tllc = $(firstword $(gallery_collection_tuple))
gct_fldr = $(lastword $(gallery_collection_tuple))

gallery_collection_dir = $(gallerydir)/$(call surfer_gallery_collection_get_relfolder,$(gct_tllc),$(gct_fldr))

EXTRA_DIST = GNUmakefile

%:
	@echo $@
	# neutralized

all:

install: install-data

install-data:
	$(call surfer_gallery_collection_populate,$(gct_tllc),$(gct_fldr))

uninstall:
	$(call surfer_gallery_collection_unpopulate,$(gct_tllc),$(gct_fldr))
	$(if $(wildcard $(gallery_collection_dir)),$(shell rmdir --i $(gallery_collection_dir)),)

distdir:
	$(call surfer_gallery_collection_distribute,$(gct_tllc),$(gct_fldr))

.PHONY: install uninstall install-data uninstall-dat distdir

# vim:syntax=make
