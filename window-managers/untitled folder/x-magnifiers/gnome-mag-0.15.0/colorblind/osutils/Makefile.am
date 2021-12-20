INCLUDES = $(PYTHON_INCLUDES) $(CBAPPLET_CFLAGS)

_osutilsdir = $(pythondir)/colorblind/osutils
_osutils_LTLIBRARIES = _osutils.la
_osutils_la_LDFLAGS = -module -avoid-version init_osutils
_osutils_la_LIBADD = $(CBAPPLET_LIBS)
_osutils_la_SOURCES = \
	_osutilsmodule.c

_osutils_PYTHON = \
	__init__.py
