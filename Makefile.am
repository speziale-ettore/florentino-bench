
## Makefile.am: project main makefile.

SUBDIRS = include src

MAINTAINERCLEANFILES = aclocal.m4 \
                       configure \
                       Makefile.in \
                       m4/libtool.m4  \
                       m4/ltoptions.m4 \
                       m4/ltsugar.m4 \
                       m4/ltversion.m4 \
                       m4/lt~obsolete.m4

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:
	rm -rf build-aux
