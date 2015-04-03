
## parameters: TwoLetterLanguageCode Collection
surfer_gallery_get_tuple = \
	$(eval _realfolder:=$(realpath .)) \
	$(eval _realfolder:=$(subst /, ,$(_realfolder))) \
	$(eval _length:=$(words $(_realfolder))) \
	$(eval _lengthminusone:=$(words $(wordlist 2,$(_length),$(_realfolder)))) \
	$(eval _folder:=$(lastword $(_realfolder))) \
	$(eval _tllc:=$(word $(_lengthminusone),$(_realfolder))) \
	$(eval _tllc:=$(subst gallery-,,$(_tllc))) \
	$(_tllc) $(_folder)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_get_relfolder = gallery-$(1)/$(2)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_get_data = \
	$(2).txt \
	$(if $(wildcard $(2).png),$(2).png,) \
	$(addsuffix .png,$(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_$(2)))

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_populate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_get_relfolder,$(1),$(2))) \
	$(shell $(MKDIR_P) $(_folder)) \
	$(foreach _pic,$(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_$(2)), \
		$(shell \
			cd $(_folder) ; \
			rm -f $(_pic).pic ; $(LN_S) ../../pool/pic/$(_pic).pic ; \
			rm -f $(_pic).ppm ; $(LN_S) ../../pool/ppm/$(_pic).ppm ; \
			) \
		) \
	$(if $(SURFER_GALLERY_COLLECTION_ICON_$(2)), \
		$(shell \
			cd $(_folder) ; \
			rm -f $(2).ppm ; $(LN_S) $(SURFER_GALLERY_COLLECTION_ICON_$(2)).ppm $(2).ppm ; \
			), \
		)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_unpopulate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_get_relfolder,$(1),$(2))) \
	$(foreach _pic,$(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_$(2)), \
		$(shell \
			rm -f $(_folder)/$(_pic).pic $(_folder)/$(_pic).ppm ; \
			) \
		) \
	$(if $(SURFER_GALLERY_COLLECTION_ICON_$(2)), \
		$(shell \
			rm -f $(_folder)/$(2).ppm ; \
			), \
		)

# vim:syntax=make
