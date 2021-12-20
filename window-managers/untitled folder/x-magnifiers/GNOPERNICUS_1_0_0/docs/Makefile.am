## Process this file with automake to produce Makefile.in

# The name of the module.
DOC_MODULE=srlow

# The top-level SGML file.
DOC_MAIN_SGML_FILE=srlow-docs.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR=../srlow/libsrlow

# Used for dependencies
HFILE_GLOB =
CFILE_GLOB =

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
INCLUDES =                      \
	-I$(top_srcdir)         \
	-I$(top_builddir)       \
	$(DEP_CFLAGS)

# gtkdoc-mkdb related varaibles
MKDB_OPTIONS =
content_files =

# Images to copy into HTML directory
HTML_IMAGES =

include $(top-srcdir)/gtk-doc.make
