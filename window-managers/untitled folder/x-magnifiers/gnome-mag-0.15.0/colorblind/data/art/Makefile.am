# *****************************************************************************
# Images and icon
# *****************************************************************************
artdir = $(datadir)/colorblind/art
art_DATA = \
	colorblind-applet.svg \
	colorblind-applet.png

16icondir = $(datadir)/icons/hicolor/16x16/apps
16icon_DATA = 16x16/colorblind-applet.png

24icondir = $(datadir)/icons/hicolor/24x24/apps
24icon_DATA = 24x24/colorblind-applet.png

48icondir = $(datadir)/icons/hicolor/48x48/apps
48icon_DATA = 48x48/colorblind-applet.png

96icondir = $(datadir)/icons/hicolor/96x96/apps
96icon_DATA = 96x96/colorblind-applet.png

svgicondir = $(datadir)/icons/hicolor/scalable/apps
svgicon_DATA = scalable/colorblind-applet.svg

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
	$(art_DATA) \
	$(16icon_DATA) \
	$(24icon_DATA) \
	$(48icon_DATA) \
	$(96icon_DATA) \
	$(svgicon_DATA)
