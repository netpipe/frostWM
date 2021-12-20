import os, sys
from os.path import join, exists, isdir, isfile, dirname, abspath, expanduser

import gtk, gtk.gdk, gconf

# Autotools set the actual data_dir in defs.py
from defs import *

try:
	# Allows to load uninstalled .la libs
	import ltihooks
except ImportError:
	pass

# Allow to use uninstalled deskbar --------------------------------------------
UNINSTALLED_COLORBLIND = False
def _check(path):
	return exists(path) and isdir(path) and isfile(path+"/AUTHORS")
	
name = join(dirname(__file__), '..')
if _check(name):
	UNINSTALLED_COLORBLIND = True
	
# Sets SHARED_DATA_DIR to local copy, or the system location
# Shared data dir is most the time /usr/share/colorblind-applet
if UNINSTALLED_COLORBLIND:
	SHARED_DATA_DIR = abspath(join(dirname(__file__), 'data'))
else:
	SHARED_DATA_DIR = join(DATA_DIR, "colorblind")
print "Data Dir: %s" % SHARED_DATA_DIR

# Set the cwd to the home directory so spawned processes behave correctly
# when presenting save/open dialogs
os.chdir(expanduser("~"))

# Path to images, icons
ART_DATA_DIR = join(SHARED_DATA_DIR, "art")
# Default icon size in the entry
ICON_WIDTH = 24
ICON_HEIGHT = 24

#Gconf client
GCONF_CLIENT = gconf.client_get_default()

# GConf directory for colorblind in window mode and shared settings
GCONF_DIR = "/apps/colorblind"

GCONF_CLIENT.add_dir(GCONF_DIR, gconf.CLIENT_PRELOAD_NONE)

# GConf key for global keybinding
GCONF_KEYBINDING_ONOFF = GCONF_DIR + "/keybinding_on_off"
GCONF_KEYBINDING_SWITCH = GCONF_DIR + "/keybinding_switch"

#GConf key for colorblind filter
GCONF_COLORBLIND_FILTER = GCONF_DIR + "/colorblind_filter"

#GConf key for default colorblind filter
GCONF_DEFAULT_FILTER = GCONF_DIR + "/default_filter"

#GConf key for combobox check to change default filter
GCONF_CHECK_DEFAULT = GCONF_DIR + "/check_default"

# Global overrides for command line mode
UI_OVERRIDE = None
