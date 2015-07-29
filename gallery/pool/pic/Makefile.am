gallerypoolpicdir = $(gallerydir)/pool/pic

gallerypoolpic_DATA = $(SURFER_GALLERY_LISTOF_PICTURE_PIC)

uninstall-hook:
	@if test -d $(gallerypoolpicdir) ; then rmdir --i $(gallerypoolpicdir) ; fi

EXTRA_DIST = $(gallerypoolpic_DATA)
