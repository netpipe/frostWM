#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# (C) 2007 Carlos Eduardo Rodrigues Diógenes.
# (C) 2005 Nigel Tao.
# Licensed under the GNU GPL.

import gtk, gnomeapplet
import getopt, sys
from os.path import *

# Allow to use uninstalled
def _check(path):
	return exists(path) and isdir(path) and isfile(path+"/AUTHORS")

name = join(dirname(__file__), '..')
if _check(name):
	print 'Running uninstalled colorblind, modifying PYTHONPATH'
	sys.path.insert(0, abspath(name))
else:
	sys.path.insert(0, abspath("@PYTHONDIR@"))
	print "Running installed colorblind, using [@PYTHONDIR@:$PYTHONPATH]"

# Now the path is set, import our applet
import colorblind, colorblind.ColorblindApplet, colorblind.defs

try:
	# attempt to set a name for killall
	import colorblind.osutils
	colorblind.osutils.set_process_name ("colorblind-applet")
except:
	print "Unable to set processName"

import gettext, locale
gettext.bindtextdomain('colorblind-applet', abspath(join(colorblind.defs.DATA_DIR, "locale")))
if hasattr(gettext, 'bind_textdomain_codeset'):
	gettext.bind_textdomain_codeset('colorblind-applet','UTF-8')
gettext.textdomain('colorblind-applet')

locale.bindtextdomain('colorblind-applet', abspath(join(colorblind.defs.DATA_DIR, "locale")))
if hasattr(locale, 'bind_textdomain_codeset'):
	locale.bind_textdomain_codeset('colorblind-applet','UTF-8')
locale.textdomain('colorblind-applet')

def applet_factory(applet, iid):
	print 'Starting Colorblind instance:', applet, iid
	colorblind.ColorblindApplet.ColorblindApplet(applet)
	return True

# Return a standalone window that holds the applet
def build_window(popup_mode=False):
	app = gtk.Window(gtk.WINDOW_TOPLEVEL)
	app.set_title("Colorblind Applet")
	app.connect("destroy", gtk.main_quit)
	if not popup_mode:
		app.set_property('resizable', False)
	
	applet = gnomeapplet.Applet()
	applet.get_orient = lambda: gnomeapplet.ORIENT_DOWN
	applet_factory(applet, None)
	applet.reparent(app)
		
	app.show_all()
	
	return app
		
def usage():
	print """=== Colorblind applet: Usage
$ colorblind-applet [OPTIONS]

OPTIONS:
	-h, --help		Print this help notice.
	-w, --window		Launch the applet in a standalone window for test purposes (default=no).
	-t, --trace		Use tracing (default=no).
	"""
	sys.exit()
	
if __name__ == "__main__":	
	standalone = False
	do_trace = False
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hwcpt", ["help", "window", "cuemiac","popup","trace"])
	except getopt.GetoptError:
		# Unknown args were passed, we fallback to bahave as if
		# no options were passed
		print "WARNING: Unknown arguments passed, using defaults."
		opts = []
		args = sys.argv[1:]
	
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
		elif o in ("-w", "--window"):
			standalone = True
		elif o in ("-t", "--trace"):
			do_trace = True

	print 'Running with options:', {
		'standalone': standalone,
	}
	
	if standalone:
		import gnome
		gnome.init(colorblind.defs.PACKAGE, colorblind.defs.VERSION)
		build_window(standalone)
	
		# run the new command using the given trace
		if do_trace:
			import trace
			trace = trace.Trace(
				ignoredirs=[sys.prefix],
				ignoremods=['sys', 'os', 'getopt', 'libxml2', 'ltihooks'],
				trace=True,
				count=False)
			trace.run('gtk.main()')
		else:
			gtk.main()
		
	else:
		gnomeapplet.bonobo_factory(
			"OAFIID:GNOME_Magnifier_ColorblindApplet_Factory",
			gnomeapplet.Applet.__gtype__,
			colorblind.defs.PACKAGE,
			colorblind.defs.VERSION,
			applet_factory)
