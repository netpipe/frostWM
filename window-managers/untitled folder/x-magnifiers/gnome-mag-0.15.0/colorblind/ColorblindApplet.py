import os, time
from os.path import *
import gnomeapplet, gtk, gtk.gdk, gconf, gnomevfs, gobject, bonobo, ORBit
from gettext import gettext as _

import colorblind, colorblind.ui
from colorblind.ui.About import show_about
from colorblind.ui.ColorblindPreferencesUI import show_preferences
from colorblind.ColorblindAppletPreferences import ColorblindAppletPreferences
from colorblind.Keybinder import get_colorblind_keybinder_on_off, get_colorblind_keybinder_switch
from colorblind.ui.WindowUI import WindowUI

_magnifierAvailable = False

try:
	ORBit.load_typelib ('Accessibility')
	ORBit.load_typelib ('GNOME_Magnifier')
	import GNOME.Magnifier
	_magnifierAvailable = True
except:
	pass

class ColorblindApplet(object):

	__instance = None

	def __init__(self, applet):
		if ColorblindApplet.__instance == None:
			ColorblindApplet.__instance = self
		
		self.enabled = False
		self.shutdown = False
		self.magnifier = None
		self.zoomer = None
		self.zoomerPBag = None
		self.pref_ui_mapped = False

		self.applet = applet
		
		self.start_query_id = 0
			
		self.prefs = ColorblindAppletPreferences(applet)
		
		gtk.window_set_default_icon_name("colorblind-applet")

		self.ui = WindowUI(applet, self.prefs)
		self.ui.connect('show-preferences', self.on_preferences, None)
		self.ui.connect('show-about', self.on_about, None)
			
		self.set_up_ui_signals ()
				
		# Monitor global shortcut binding
		get_colorblind_keybinder_on_off().connect('activated', self.on_keybinding_button_press, "on_off")
		get_colorblind_keybinder_switch().connect('activated', self.on_keybinding_button_press, "switch")
		self.applet.connect('change-background', self.on_change_background)

		self.image = gtk.Image()
		self.applet.add (self.image)
		self.applet.setup_menu_from_file (
			colorblind.SHARED_DATA_DIR, "Colorblind_Applet.xml",
			None, [
			("About", self.on_about),
			("Prefs", self.on_preferences),
			])
		self.on_change_size (self.applet)
		self.applet.show_all()
		
	def get_instance():
		return ColorblindApplet.__instance
	get_instance = staticmethod(get_instance)
		
	def on_keyboard_shortcut (self, sender, qstring, shortcut):
		for modctx in self.module_list:
			if not modctx.enabled:
				continue
				
			match = modctx.module.on_key_press(qstring, shortcut)
			if match != None:
				self.on_match_selected(sender, qstring, match)
				break
					
	def on_about (self, component, verb):
		show_about(self.applet)
	
	def on_preferences (self, component, verb):
		show_preferences(self)
		self.pref_ui_mapped = True

	def active_actual_filter (self):
		if self.enabled == True:
			filter = colorblind.GCONF_CLIENT.get_int(colorblind.GCONF_COLORBLIND_FILTER)
			bonobo.pbclient_set_short (self.zoomerPBag, "color-blind-filter", filter)

	def activate_magnifier (self):
		self.magnifier = bonobo.get_object ("OAFIID:GNOME_Magnifier_Magnifier:0.9", "GNOME/Magnifier/Magnifier")
		magPBag = self.magnifier.getProperties ()
		tdb = magPBag.getValue ("target-display-bounds").value ()
		if tdb.x1 == 0 and tdb.y1 == 0 and tdb.x2 == 0 and tdb.y2 == 0:
			self.zoomer = self.magnifier.createZoomRegion (1, 1, GNOME.Magnifier.RectBounds (0, 0, gtk.gdk.screen_width (), gtk.gdk.screen_height ()), GNOME.Magnifier.RectBounds (0, 0, gtk.gdk.screen_width (), gtk.gdk.screen_height ()))
			self.magnifier.addZoomRegion (self.zoomer)
			bonobo.pbclient_set_long (magPBag, "crosswire-size", 0)
			self.shutdown = True
		if self.zoomer == None:
			self.zoomer = self.magnifier.getZoomRegions ()
			self.zoomer = self.zoomer[0]
		self.zoomerPBag = self.zoomer.getProperties ()
		if tdb.x1 == 0 and tdb.y1 == 0 and tdb.x2 == 0 and tdb.y2 == 0:
			bonobo.pbclient_set_boolean (self.zoomerPBag, "draw-cursor", False)
		cb_filter = colorblind.GCONF_CLIENT.get_int(colorblind.GCONF_DEFAULT_FILTER)
		bonobo.pbclient_set_short (self.zoomerPBag, "color-blind-filter", cb_filter)
		self.enabled = True
	
	def enable_disable_filters (self):
		global _magnifierAvailable

		if _magnifierAvailable:
			if self.enabled == True:
				try:
					if self.shutdown:
						self.magnifier.clearAllZoomRegions ()
						self.magnifier.dispose ()
						self.magnifier = None
						self.zoomer = None
				except:
					pass
				if self.zoomer:
					try:
						bonobo.pbclient_set_short (self.zoomerPBag, "color-blind-filter", 0)
					except:
						self.activate_magnifier ()
						return
				self.enabled = False
				self.shutdown = False
				save_actual_filter = colorblind.GCONF_CLIENT.get_bool (colorblind.GCONF_CHECK_DEFAULT)
				if save_actual_filter:
					af = colorblind.GCONF_CLIENT.get_int (colorblind.GCONF_COLORBLIND_FILTER)
					colorblind.GCONF_CLIENT.set_int (colorblind.GCONF_DEFAULT_FILTER, af)				
			else:
				self.activate_magnifier ()

	def next_filter (self):
		cb_filter = colorblind.GCONF_CLIENT.get_int (colorblind.GCONF_COLORBLIND_FILTER)
		cb_filter = ((cb_filter + 1) % 9)
		if cb_filter == 0:
			cb_filter += 1
		colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, cb_filter)
		if self.enabled and self.pref_ui_mapped == False:
			bonobo.pbclient_set_short (self.zoomerPBag, "color-blind-filter", cb_filter)

	def on_keybinding_button_press(self, widget, str):
		print "Keybinding activated"
		if str == "on_off":
			self.enable_disable_filters ()
		elif str == "switch":
			self.next_filter ()
		
	def on_keybinding_changed(self, binder, bound):
		# FIXME: provide visual clue when not bound
		# FIXME: should be used in the pref window
		pass

	def set_up_ui_signals (self):
		self.applet.connect('change-size', lambda applet, orient: self.on_change_size(applet))

	def set_image_from_file (self, filename, size):
		# We use an intermediate pixbuf to scale the image
		if self.applet.get_orient in [gnomeapplet.ORIENT_DOWN, gnomeapplet.ORIENT_UP]:
			pixbuf = gtk.gdk.pixbuf_new_from_file_at_size (filename, -1, size)
		else:
			pixbuf = gtk.gdk.pixbuf_new_from_file_at_size (filename, size, -1)
		self.image.set_from_pixbuf (pixbuf)

	def on_change_size (self, applet):
		# FIXME: This is ugly, but i don't know how to get it right
		image_name = "colorblind-applet"

		if applet.get_size() <= 36:
			image_name += ".png"
			s = -1
		else:
			image_name += ".svg"
			s = applet.get_size()-12
	
		self.set_image_from_file (join(colorblind.ART_DATA_DIR, image_name), s)

	def on_change_background (self, widget, background, colour, pixmap):
		if background == gnomeapplet.NO_BACKGROUND or background == gnomeapplet.PIXMAP_BACKGROUND:
			copy = self.applet.get_style().copy()
			copy.bg_pixmap[gtk.STATE_NORMAL] = pixmap
			copy.bg_pixmap[gtk.STATE_INSENSITIVE]  = pixmap
			self.applet.set_style(copy)
		elif background == gnomeapplet.COLOR_BACKGROUND:
			self.applet.modify_bg(gtk.STATE_NORMAL, colour)

def active_actual_filter ():
	ColorblindApplet.get_instance ().active_actual_filter ()

def pref_ui_unmapped ():
	ColorblindApplet.get_instance ().pref_ui_mapped = False
