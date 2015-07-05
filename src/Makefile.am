bin_PROGRAMS = surfer

man_MANS = surfer.1

noinst_HEADERS = \
	surfer.hh \
	bwindow_gdk.hh \
	matrix.hh \
	vecmath/AxisAngle4_.h \
	vecmath/Point2.h \
	vecmath/Tuple4.h \
	vecmath/AxisAngle4.h \
	vecmath/Point3.h \
	vecmath/vecmath.h \
	vecmath/Color3.h \
	vecmath/Point4.h \
	vecmath/Vector2.h \
	vecmath/Color4.h \
	vecmath/Quat4_.h \
	vecmath/Vector3.h \
	vecmath/Matrix3_.h \
	vecmath/Quat4.h \
	vecmath/Vector4.h \
	vecmath/Matrix3.h \
	vecmath/TexCoord2.h \
	vecmath/vm_conf.h \
	vecmath/Matrix4_.h \
	vecmath/Tuple2.h \
	vecmath/VmUtil.h \
	vecmath/Matrix4.h \
	vecmath/Tuple3.h

surfer_SOURCES = \
	about.cc \
	bwindow_gdk.cc \
	fix_input.cc \
	gallery.cc \
	gallerycontrol.cc \
	options.cc \
	parsepic.cc \
	rotate.cc \
	select.cc \
	surfer.cc \
	win32.cc \
	xpm_data.cc

AM_H2MFLAGS = \
	--manual="IMAGINARY" \
	--help-option="--help" \
	--version-option="--version" \
	--no-info

surfer_CPPFLAGS = \
	-I $(top_srcdir)/src/vecmath \
	-DSURF_CMD=\"$(SURF_CMD)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DGALLERY_PATH=\"$(gallerydir)\"

surfer_CXXFLAGS = \
	$(LIBGTKMM_CFLAGS)

surfer_LDADD = \
	$(LIBGTKMM_LIBS)

## short descriptions for manpages
surfer_DESCRIPTION =  "Interactive Visualization of ALGebraic GEOmetric SURFaces"

%.1: %
	$(HELP2MAN) \
			-s 1 \
			$(AM_H2MFLAGS) \
			-I $(top_srcdir)/man/$*.h2m \
			-n $(if $($(subst -,_,$*)_DESCRIPTION), $($(subst -,_,$*)_DESCRIPTION), "manual page for $*") \
			-o $@ \
		$(builddir)/$<

EXTRA_DIST = \
	$(man_MANS)

DISTCLEANFILES = \
	$(man_MANS)

