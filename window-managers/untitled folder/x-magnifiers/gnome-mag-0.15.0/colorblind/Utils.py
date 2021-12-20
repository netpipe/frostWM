import os, cgi, re
from os.path import *
from gettext import gettext as _
import colorblind
import gtk, gtk.gdk, gnome.ui, gobject, gnomevfs

ICON_THEME = gtk.icon_theme_get_default()
factory = gnome.ui.ThumbnailFactory(colorblind.ICON_HEIGHT)

def get_xdg_data_dirs():
	dirs = os.getenv("XDG_DATA_HOME")
	if dirs == None:
		dirs = expanduser("~/.local/share")
	
	sysdirs = os.getenv("XDG_DATA_DIRS")
	if sysdirs == None:
		sysdirs = "/usr/local/share:/usr/share"
	
	dirs = "%s:%s" % (dirs, sysdirs)
	return [dir for dir in dirs.split(":") if dir.strip() != "" and exists(dir)]

def load_icon_for_file(f):
	icon_name, flags = gnome.ui.icon_lookup(ICON_THEME, factory,
				f, "",
				gnome.ui.ICON_LOOKUP_FLAGS_SHOW_SMALL_IMAGES_AS_THEMSELVES)
		
	return load_icon(icon_name)

def load_icon_for_desktop_icon(icon):
	if icon != None:
		icon = colorblind.gnomedesktop.find_icon(ICON_THEME, icon, colorblind.ICON_HEIGHT, 0)
		if icon != None:
			return load_icon(icon)
		
# We load the icon file, and if it fails load an empty one
# If the iconfile is a path starting with /, load the file
# else try to load a stock or named icon name
def load_icon(icon, width=colorblind.ICON_HEIGHT, height=colorblind.ICON_HEIGHT):
	pixbuf = None
	if icon != None and icon != "":
		try:
			our_icon = join(colorblind.ART_DATA_DIR, icon)
			if exists(our_icon):
				pixbuf = gtk.gdk.pixbuf_new_from_file_at_size(our_icon, width, height)
			elif icon.startswith("/"):
				pixbuf = gtk.gdk.pixbuf_new_from_file_at_size(icon, width, height)
			else:
				pixbuf = ICON_THEME.load_icon(splitext(icon)[0], width, gtk.ICON_LOOKUP_USE_BUILTIN)
		except Exception, msg1:
			try:
				pixbuf = ICON_THEME.load_icon(icon, width, gtk.ICON_LOOKUP_USE_BUILTIN)
			except Exception, msg2:
				print 'Error:load_icon:Icon Load Error:%s (or %s)' % (msg1, msg2)
				
	# an icon that is too tall will make the EntryCompletion look funny
	if pixbuf != None and pixbuf.get_height() > height:
		pixbuf = pixbuf.scale_simple(width, height, gtk.gdk.INTERP_BILINEAR)
	return pixbuf

PATH = [path for path in os.getenv("PATH").split(os.path.pathsep) if path.strip() != "" and exists(path) and isdir(path)]
def is_program_in_path(program):
	for path in PATH:
		prog_path = join(path, program)
		if exists(prog_path) and isfile(prog_path) and os.access(prog_path, os.F_OK | os.R_OK | os.X_OK):
			return True
