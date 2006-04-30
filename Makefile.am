EXTRA_DIST = \
        ogl.xpm \
        ogl.ico \
        autogen.sh

SUBDIRS	= src

dist-hook:
	(cd $(top_srcdir))

#
# Rule to build RPM distribution package
#
rpm: dist
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz

