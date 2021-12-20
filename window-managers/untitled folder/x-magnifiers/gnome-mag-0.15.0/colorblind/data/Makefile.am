SUBDIRS = art

uidir = $(datadir)/colorblind
ui_DATA = \
	Colorblind_Applet.xml \
	colorblind-prefs.glade

# *****************************************************************************
# GConf Schemas
# *****************************************************************************
# GConf Preference Schemas
schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_in_files = colorblind-applet.schemas.in
schema_DATA = $(schema_in_files:.schemas.in=.schemas)

@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		$(GCONFTOOL) --makefile-install-rule $(schema_DATA)
endif

DISTCLEANFILES = \
	$(schema_DATA)

EXTRA_DIST = \
	$(ui_DATA) \
	$(schema_in_files)
