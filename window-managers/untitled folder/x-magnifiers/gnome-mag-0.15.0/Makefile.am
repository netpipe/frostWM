# Process this file with automake to create Makefile.in.

SUBDIRS = idl magnifier test $(CB_DIR) po cursors docs

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= gnome-mag-1.0.pc

EXTRA_DIST=gnome-mag-1.0.pc.in \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

doc_tarball_files = \
	docs/reference/html

DISTCLEANFILES = intltool-extract \
	intltool-merge	\
	intltool-update

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update

gnome-mag-1-0-docs.tar.gz:
	tar cf - $(doc_tarball_files) | gzip -c --best >$@

doc-clean-recursive:
	(cd docs && $(MAKE) $(AM_MAKEFLAGS) doc-clean)

doc-clean: doc-clean-recursive

doc-rebuild:
	(cd docs && $(MAKE) $(AM_MAKEFLAGS) doc-rebuild)

.PHONY: files doc-clean doc-clean-recursive doc-rebuild

files:
	@for subdir in $(SUBDIRS); do\
	  files=`cd $$subdir; $(MAKE) files | grep -v "make\[[1-9]\]"`; \
	  for file in $$files; do \
	    echo $$subdir/$$file; \
	  done; \
	done

