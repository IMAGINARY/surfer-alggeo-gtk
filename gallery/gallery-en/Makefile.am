gallery_en_dir = $(gallerydir)/gallery-en

SUBDIRS = $(SURFER_GALLERY_LISTOF_COLLECTION)

gallery_en__DATA = galleries.txt

uninstall-hook:
	@if test -d $(gallery_en_dir) ; then rmdir --i $(gallery_en_dir) ; fi

EXTRA_DIST = $(gallery_en__DATA)
