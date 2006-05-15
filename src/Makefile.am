EXTRA_DIST = \
        commander.rc \
        commander.res

SUBDIRS = simgear input_output math wx gui bitmaps

AM_CPPFLAGS = -I$(top_srcdir)src \
              -I$(top_srcdir)/src/input_output -I$(top_srcdir)/src/math \
	      -I$(top_srcdir)/src/simgear/xml \
	      -DwxUSE_DEPRECATED=1 -DwxUSE_PROLOGIO=1 \
              -I$(top_srcdir)/src/gui ${WX_CXXFLAGS}

bin_PROGRAMS = Commander

commander.res: commander.rc
	$(RC) $< -O coff -o $@ $(WX_INCLUDES) -I$(top_srcdir)/src -I$(top_srcdir)/src/bitmaps

Commander_SOURCES = Commander.cpp

Commander_LDADD = $(top_builddir)/src/gui/libGUI.a \
	$(top_builddir)/src/input_output/libInputOutput.a \
	$(top_builddir)/src/math/libMath.a \
	$(top_builddir)/src/libJSBSim.a \
	$(top_builddir)/src/simgear/props/libProperties.a \
	$(top_builddir)/src/simgear/xml/libExpat.a \
	$(top_builddir)/src/wx/plot/libPlot.a \
	$(top_builddir)/src/wx/ogl/libOGL.a \
	$(top_builddir)/src/wx/deprecated/libDeprecated.a \
	${WX_LIBS} -lm $(WIN32_LDADD) $(COMMANDER_RES)

Commander_DEPENDENCIES = \
        $(top_builddir)/src/gui/libGUI.a \
	$(top_builddir)/src/input_output/libInputOutput.a \
	$(top_builddir)/src/math/libMath.a \
	$(top_builddir)/src/libJSBSim.a \
	$(top_builddir)/src/simgear/props/libProperties.a \
	$(top_builddir)/src/simgear/xml/libExpat.a \
	$(top_builddir)/src/wx/plot/libPlot.a \
	$(top_builddir)/src/wx/ogl/libOGL.a \
	$(top_builddir)/src/wx/deprecated/libDeprecated.a \
	$(COMMANDER_RES)

noinst_LIBRARIES = libJSBSim.a

libJSBSim_a_SOURCES =  FGJSBBase.cpp

noinst_HEADERS = initprops.h Commander.h FGJSBBase.h 
