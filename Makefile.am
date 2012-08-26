## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

guimupdocdir = ${prefix}/doc/guimup
guimupdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(guimupdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(guimupdocdir)
