bin_PROGRAMS = surfer

surfer_SOURCES = about.cc bwindow_gdk.cc fix_input.cc gallery.cc \
	gallerycontrol.cc options.cc parallel.cc parsepic.cc rotate.cc select.cc surfer.cc \
	win32.cc xpm_data.cc

surfer_CPPFLAGS = -I $(srcdir)/vecmath

noinst_HEADERS = surfer.hh bwindow_gdk.hh matrix.hh \
	vecmath/AxisAngle4_.h  vecmath/Point2.h     vecmath/Tuple4.h \
	vecmath/AxisAngle4.h   vecmath/Point3.h     vecmath/vecmath.h \
	vecmath/Color3.h       vecmath/Point4.h     vecmath/Vector2.h \
	vecmath/Color4.h       vecmath/Quat4_.h     vecmath/Vector3.h \
	vecmath/Matrix3_.h     vecmath/Quat4.h      vecmath/Vector4.h \
	vecmath/Matrix3.h      vecmath/TexCoord2.h  vecmath/vm_conf.h \
	vecmath/Matrix4_.h     vecmath/Tuple2.h     vecmath/VmUtil.h \
	vecmath/Matrix4.h      vecmath/Tuple3.h


AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@ 

surfer_LDADD = @LIBS@ @GTKMM_LIBS@ 

DEFS = -DLOCALEDIR=\"$(localedir)\" -DGALLERY_PATH=\"$(pkgdatadir)/gallery\" @DEFS@

