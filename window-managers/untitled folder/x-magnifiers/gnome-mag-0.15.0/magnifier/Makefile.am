Cursorsdir = $(datadir)/gnome-mag

lib_LTLIBRARIES = libgnome-mag.la

bin_PROGRAMS = magnifier

libgnome_mag_la_SOURCES = \
	GNOME_Magnifier-common.c \
	GNOME_Magnifier-stubs.c

libgnome_mag_la_DEPENDENCIES = \
	GNOME_Magnifier.h

libgnome_mag_la_LDFLAGS = \
	$(LT_VERSION_INFO) -no-undefined

libgnome_mag_la_LIBADD = $(LIBDEPS_LIBS)

libgnomemagincludedir=$(includedir)/gnome-mag-1.0/magnifier
libgnomemaginclude_HEADERS=GNOME_Magnifier.h

AM_CFLAGS = $(CFLAGS) $(DEPS_CFLAGS) $(X_FLAGS)

magnifier_SOURCES = magnifier-main.c \
	magnifier.c \
	magnifier-private.h \
	zoom-region.c \
	zoom-region.h \
	zoom-region-private.h \
	magnifier.h \
	magnifier-private.h \
	GNOME_Magnifier.h \
	GNOME_Magnifier-skels.c \
	GNOME_Magnifier-common.c \
	$(targetdir)/gmag-graphical-server.c \
	$(targetdir)/gmag-graphical-server.h \
	$(targetdir)/gmag-compositor.c \
	$(targetdir)/gmag-compositor.h \
	$(targetdir)/gmag-damage.c \
	$(targetdir)/gmag-damage.h \
	$(targetdir)/gmag-cursor.c \
	$(targetdir)/gmag-cursor.h
magnifier_CFLAGS = $(AM_CFLAGS)

INCLUDES = \
	$(DEPS_CFLAGS)		               \
	-I$(top_srcdir)		               \
	-I$(top_builddir)	               \
	-I$(top_srcdir)/idl	               \
	-I$(top_srcdir)/magnifier/$(targetdir) \
	$(GNOME_MAG_DEBUG_CFLAGS)	       \
	-DCURSORSDIR=\""$(Cursorsdir)"\"

IDL_OUT = GNOME_Magnifier.h GNOME_Magnifier-skels.c GNOME_Magnifier-stubs.c GNOME_Magnifier-common.c GNOME_Magnifier-imodule.c

IDL = $(top_srcdir)/idl/GNOME_Magnifier.idl

BUILT_SOURCES = $(IDL_OUT)
CLEANFILES = $(IDL_OUT)
DISTCLEANFILES = $(IDL_OUT)

magnifier.h : GNOME_Magnifier.h

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) 	\
	-I$(LIBBONOBO_IDL_DIR)			\
	-I$(top_srcdir)/idl			\
	--add-imodule

orbittypelibdir = $(libdir)/orbit-2.0
orbittypelib_LTLIBRARIES = GNOME_Magnifier_module.la
GNOME_Magnifier_module_la_LDFLAGS = \
	-export-dynamic -module -avoid-version -no-undefined

GNOME_Magnifier_module_la_LIBADD = $(ORBIT_LIBS)

GNOME_Magnifier_module_la_SOURCES = \
	GNOME_Magnifier-imodule.c

# The X_LIBS must NOT be searched first or it breaks the build
LDADD = libgnome-mag.la $(DEPS_LIBS) $(X_LIBS) $(COLORBLIND_LIBS)

serverinfodir= $(libdir)/bonobo/servers
$(server_in_files): $(server_in_files).in
	sed -e "s|\@BINDIR\@|$(bindir)|" $< > $@
server_in_files = GNOME_Magnifier.server.in
serverinfo_DATA = $(server_in_files:.server.in=.server)

@INTLTOOL_SERVER_RULE@

EXTRA_DIST = $(server_in_files).in 

$(IDL_OUT) : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $(IDL)

DONT_DIST_SOURCE = $(IDL_OUT)

CLEANFILES += $(server_in_files) $(serverinfo_DATA)

dist-hook:
	for file in $(DONT_DIST_SOURCE) ; do \
	    rm -f $(distdir)/$$file ; \
	done
