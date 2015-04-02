gallerypoolppmdir = $(gallerydir)/pool/ppm

gallerypoolppm_DATA = $(SURFER_GALLERY_LISTOF_PICTURE_PPM)

uninstall-hook:
	@if test -d $(gallerypoolppmdir) ; then rmdir --i $(gallerypoolppmdir) ; fi

EXTRA_DIST = $(gallerypoolppm_DATA)
