AC_DEFUN([surfer_AC_SURFER_GALLERY_SETUP],
[

SURFER_GALLERY_LISTOF_COLLECTION="ADE flaechen flaechen_fantasien ManySings RatFl"
AC_SUBST(SURFER_GALLERY_LISTOF_COLLECTION)


SURFER_GALLERY_LISTOF_PICTURE=""

SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_ADE="\
	A1pm \
	A2pm A2pp \
	A3mm A3pm \
	A4pm A4pp \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_ADE)

SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ADE="\
	A1pm \
	A2pm A2pp \
	A3mm A3pm \
	A4pm A4pp \
	A5mm A5pm \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ADE)
SURFER_GALLERY_LISTOF_PICTURE="${SURFER_GALLERY_LISTOF_PICTURE} ${SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ADE}"

SURFER_GALLERY_COLLECTION_ICON_ADE="A4pm"
AC_SUBST(SURFER_GALLERY_COLLECTION_ICON_ADE)


SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_flaechen=""
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_flaechen)

SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen="\
	Buggle \
	Columpius Crixxi Cube \
	Dattel Dromedar Durchblick \
	Fanfare Flirt \
	Geisha Gupf \
	Harlekin \
	Kolibiri Kreuz \
	Limao \
	Michaelangelo \
	Octdong \
	Pilzchen Pipe Plop Polsterzipf \
	Schneeflocke Sofa Sphaere Spindel Suess \
	Tanz Tobel Torus Trichter Twilight \
	Ufo \
	Wedeln Whitney Wigwam Windkanal \
	Xano \
	Zitrus Zylinder \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen)
SURFER_GALLERY_LISTOF_PICTURE="${SURFER_GALLERY_LISTOF_PICTURE} ${SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen}"

SURFER_GALLERY_COLLECTION_ICON_flaechen="Zitrus"
AC_SUBST(SURFER_GALLERY_COLLECTION_ICON_flaechen)


SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_flaechen_fantasien=""
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_flaechen_fantasien)

SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen_fantasien="\
	Calypso Calyx \
	Daisy Diabolo Dingdong Distel Dullo \
	Eistuete \
	Flirt \
	Helix Herz \
	Miau \
	Nepali \
	Sofa Solitude \
	Tanz Tobel Tuelle \
	Visavis \
	Zeck \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen_fantasien)
SURFER_GALLERY_LISTOF_PICTURE="${SURFER_GALLERY_LISTOF_PICTURE} ${SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_flaechen_fantasien}"

SURFER_GALLERY_COLLECTION_ICON_flaechen_fantasien="Helix"
AC_SUBST(SURFER_GALLERY_COLLECTION_ICON_flaechen_fantasien)


SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_ManySings="\
	barth_sextic barthsextic_30A2 \
	cayley_cubic chmutov_pn_8 \
	dessins_quint_15a2 doppelkegel \
	endrass_octic \
	kub_D4 kummer \
	labs_septic \
	monoid \
	p9surface \
	septic_7eck \
	togliatti \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_ManySings)

SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ManySings="\
	barthquintic_15A2 barth_sextic barthsextic_30A2 \
	cayley cayley_cubic chmutov_pn_8 \
	dessins_quint_15a2 doppelkegel \
	endrass_octic \
	kegel kub_A5A1 kub_D4 kummer \
	labs_septic \
	monoid \
	nonic_27D4_3A1 \
	p9surface \
	septic_7eck \
	togliatti \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ManySings)
SURFER_GALLERY_LISTOF_PICTURE="${SURFER_GALLERY_LISTOF_PICTURE} ${SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_ManySings}"

SURFER_GALLERY_COLLECTION_ICON_ManySings="labs_septic"
AC_SUBST(SURFER_GALLERY_COLLECTION_ICON_ManySings)


SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_RatFl="\
	cayley clebsch \
	ebene einHyp Ell ellPar ellZyl \
	hypPar hypZyl \
	kegel kub_A5A1 kugel \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_DOCUMENT_RatFl)

SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_RatFl="\
	cayley clebsch \
	ebene einHyp Ell ellPar ellZyl \
	hypPar hypZyl \
	kegel kub_A5A1 kub_D4 kugel \
	"
AC_SUBST(SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_RatFl)
SURFER_GALLERY_LISTOF_PICTURE="${SURFER_GALLERY_LISTOF_PICTURE} ${SURFER_GALLERY_COLLECTION_LISTOF_PICTURE_RatFl}"

SURFER_GALLERY_COLLECTION_ICON_RatFl="kub_A5A1"
AC_SUBST(SURFER_GALLERY_COLLECTION_ICON_RatFl)

SURFER_GALLERY_LISTOF_PICTURE="`\
	for _pic in ${SURFER_GALLERY_LISTOF_PICTURE} ; do echo ${_pic}; done | \
	sort -u | \
	tr '\n' ' '\
	`"

SURFER_GALLERY_LISTOF_PICTURE_PIC=`for _pic in ${SURFER_GALLERY_LISTOF_PICTURE} ; do echo ${_pic}.pic ; done | tr '\n' ' '`
SURFER_GALLERY_LISTOF_PICTURE_PPM=`for _pic in ${SURFER_GALLERY_LISTOF_PICTURE} ; do echo ${_pic}.ppm ; done | tr '\n' ' '`

AC_SUBST(SURFER_GALLERY_LISTOF_PICTURE)
AC_SUBST(SURFER_GALLERY_LISTOF_PICTURE_PIC)
AC_SUBST(SURFER_GALLERY_LISTOF_PICTURE_PPM)

AC_SUBST(gallerydir,'$(pkgdatadir)/gallery')

])
