EXTRA_DIST = config.rpath m4/ChangeLog  \
        autogen.sh

SUBDIRS	= po src

dist-hook:
	(cd $(top_srcdir))

#
# Rule to build RPM distribution package
#
rpm: dist
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz


ACLOCAL_AMFLAGS = -I m4
