AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math -I$(top_srcdir)/src/initialization \
              -I$(top_srcdir)/src/input_output -I$(top_srcdir)/src/simgear/xml \
              -I$(top_srcdir)/src/prop \
              -I$(top_srcdir)/src/simgear \
              -DNOSIMGEAR -DHAVE_EXPAT_CONFIG_H -DHAVE_MEMMOVE

noinst_LIBRARIES = libExpat.a

libExpat_a_SOURCES = xmlparse.c xmltok.c xmlrole.c easyxml.cxx

#Matt's change here
noinst_HEADERS = ascii.h asciitab.h easyxml.hxx expat.h expat_external.h \
                 iasciitab.h internal.h latin1tab.h macconfig.h nametab.h \
                 utf8tab.h winconfig.h xmlrole.h xmltok.h xmltok_impl.h \
                 xmltok_impl.c xmltok_ns.c
