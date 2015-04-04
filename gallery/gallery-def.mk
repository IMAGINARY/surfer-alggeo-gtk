
#### Utility tools for Galleries

## parameters: TwoLetterLanguageCode
surfer_gallery_get_tuple = \
	$(eval _realfolder:=$(realpath .)) \
	$(eval _realfolder:=$(subst /, ,$(_realfolder))) \
	$(eval _tllc:=$(lastword $(_realfolder))) \
	$(eval _tllc:=$(subst gallery-,,$(_tllc))) \
	$(_tllc)

## parameters: TwoLetterLanguageCode
surfer_gallery_get_relfolder = gallery-$(1)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_get_data = galleries.txt

## parameters: TwoLetterLanguageCode
surfer_gallery_populate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_get_relfolder,$(1))) \
	$(eval _data:=$(call surfer_gallery_get_data,$(1))) \
	$(call surfer__populate,$(_folder),$(_data))

## parameters: TwoLetterLanguageCode
surfer_gallery_unpopulate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_get_relfolder,$(1))) \
	$(eval _data:=$(call surfer_gallery_get_data,$(1))) \
	$(call surfer__unpopulate,$(_folder),$(_data))

## parameters: TwoLetterLanguageCode
surfer_gallery_distribute = \
	$(eval _relfolder:=gallery/$(call surfer_gallery_get_relfolder,$(1))) \
	$(eval _data:=$(call surfer_gallery_get_data,$(1))) \
	$(call surfer__distribute,$(_relfolder),$(_data))


#### Utility tools for Gallery Collections

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_get_tuple = \
	$(eval _realfolder:=$(realpath .)) \
	$(eval _realfolder:=$(subst /, ,$(_realfolder))) \
	$(eval _length:=$(words $(_realfolder))) \
	$(eval _lengthminusone:=$(words $(wordlist 2,$(_length),$(_realfolder)))) \
	$(eval _folder:=$(lastword $(_realfolder))) \
	$(eval _tllc:=$(word $(_lengthminusone),$(_realfolder))) \
	$(eval _tllc:=$(subst gallery-,,$(_tllc))) \
	$(_tllc) $(_folder)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_get_relfolder = gallery-$(1)/$(2)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_get_data = \
	$(2).txt \
	$(if $(wildcard $(2).png),$(2).png,) \
	$(addsuffix .png,$(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_$(2)))

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_populate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_collection_get_relfolder,$(1),$(2))) \
	$(eval _data:=$(call surfer_gallery_collection_get_data,$(1),$(2))) \
	$(call surfer__populate,$(_folder),$(_data)) \
	$(foreach _pic,$(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_$(2)), \
		$(shell \
			cd $(_folder) ; \
			test -f $(_pic).pic || $(LN_S) ../../pool/pic/$(_pic).pic || exit 1 ; \
			test -f $(_pic).ppm || $(LN_S) ../../pool/ppm/$(_pic).ppm || exit 1 ; \
			) \
		) \
	$(if $(SURFER_GALLERY_COLLECTION_ICON_$(2)), \
		$(shell \
			cd $(_folder) ; \
			test -f $(2).ppm || $(LN_S) $(SURFER_GALLERY_COLLECTION_ICON_$(2)).ppm $(2).ppm || exit 1 ; \
			), \
		)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_unpopulate = \
	$(eval _folder:=$(DESTDIR)/$(gallerydir)/$(call surfer_gallery_collection_get_relfolder,$(1),$(2))) \
	$(eval _data:=$(call surfer_gallery_collection_get_data,$(1),$(2))) \
	$(call surfer__unpopulate,$(_folder), \
		$(if $(SURFER_GALLERY_COLLECTION_ICON_$(2)),$(2).ppm,) \
		$(foreach _ext, ppm pic ,$(addsuffix .$(_ext),$(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_$(2)))) \
		$(_data) \
		)

## parameters: TwoLetterLanguageCode Collection
surfer_gallery_collection_distribute = \
	$(eval _relfolder:=gallery/$(call surfer_gallery_collection_get_relfolder,$(1),$(2))) \
	$(eval _data:=$(call surfer_gallery_collection_get_data,$(1),$(2))) \
	$(call surfer__distribute,$(_relfolder),$(_data))


#### Utility metatools

## parameters: Folder DataSet
surfer__populate = \
	$(shell $(MKDIR_P) $(1)) \
	$(foreach _datum,$(2), \
		$(shell \
			test -f $(1)/$(_datum) || cp -p $(_datum) $(1)/$(_datum) || exit 1 ; \
			) \
		)

## parameters: Folder DataSet
surfer__unpopulate = \
	$(shell cd $(1) && rm -f $(2) ;)

## parameters: RelativeFolder DataSet
surfer__distribute = \
	$(if $(top_distdir),,$(error preset automake variable top_distdir not set, consider to run make from the top directory)) \
	$(eval _distfolder:=$(lastword $(subst /, ,$(top_distdir)))) \
	$(eval _abs_distfolder:=$(abs_top_builddir)/$(_distfolder)/$(1)) \
	$(call surfer__populate,$(_abs_distfolder), $(2) $(EXTRA_DIST) )


# vim:syntax=make
