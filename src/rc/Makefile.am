
EXTRA_DIST = trans.cpp \
  frame.xrc            menu.xrc           toolbar.xrc \
  sum_dlg.xrc          propdlg.xrc \
  acft_mgr.bmp         mgr.bmp             rslt_mgr.bmp \
  appicon.xpm          appicon.ico

noinst_LIBRARIES = libRC.a

libRC_a_SOURCES = resource.cpp
libRC_a_CPPFLAGS =  ${WX_CXXFLAGS} 

XRC_FILES = \
  $(srcdir)/frame.xrc    $(srcdir)/menu.xrc  $(srcdir)/toolbar.xrc \
  $(srcdir)/sum_dlg.xrc  $(srcdir)/propdlg.xrc

resource.cpp : $(XRC_FILES)
	$(WXRC) -c -o $@ $(XRC_FILES)

.xrc.h:
	$(WXRC) -e -c -o $*.cpp $<

trans.cpp : $(XRC_FILES)
	$(WXRC) -g $(XRC_FILES) | $(AWK) 'BEGIN {print "#include <string>\nusing std::string;\n#define _(ABC) ABC\n\nint main() {\n  string str;"} END {print "  return 0;\n}\n"} {print "  str = " $$0 }' > $@
	cp -f $@ $(srcdir)/$@

