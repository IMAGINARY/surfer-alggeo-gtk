bin_PROGRAMS = surfer

surfer_SOURCES = bwindow_gdk.cc fix_input.cc gallery.cc gallerycontrol.cc \
	options.cc parallel.cc parsepic.cc rotate.cc select.cc surfer.cc win32.cc

noinst_HEADERS = surfer.hh bwindow_gdk.hh matrix.hh

AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@ 

surfer_LDADD = @LIBS@ @GTKMM_LIBS@ 
