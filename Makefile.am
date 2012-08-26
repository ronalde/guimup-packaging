## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update \
                 po/.intltool-merge-cache

guimupdocdir = ${prefix}/doc/guimup
guimupdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	FAQ

EXTRA_DIST = $(guimupdoc_DATA) $(INTLTOOL_FILES)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

