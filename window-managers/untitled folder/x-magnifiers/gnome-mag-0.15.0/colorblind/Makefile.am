SUBDIRS = ui osutils data keybinder

colorblind-applet: colorblind-applet.py
	sed -e "s|\@PYTHONDIR\@|$(pythondir)/colorblind|" $< > $@

colorblindbindir = $(libdir)/colorblind-applet
colorblindbin_SCRIPTS = colorblind-applet

colorblinddir = $(pythondir)/colorblind

colorblind_PYTHON = \
	__init__.py \
	ColorblindApplet.py \
	ColorblindAppletPreferences.py \
	Keybinder.py \
	Utils.py \
	defs.py

serverdir = $(libdir)/bonobo/servers
$(server_in_files): $(server_in_files).in
	sed -e "s|\@LIBDIR\@|$(libdir)|" $< > $@
server_in_files = GNOME_Magnifier_ColorblindApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)

@INTLTOOL_SERVER_RULE@

CLEANFILES = 					   \
	colorblind-applet			   \
	GNOME_Magnifier_ColorblindApplet.server.in \
	$(colorblind_DATA)			   \
	$(server_DATA)

DISTCLEANFILES =		\
	defs.py			\
	$(CLEANFILES)

EXTRA_DIST =			\
	colorblind-applet.py	\
	defs.py.in		\
	$(server_in_files).in
