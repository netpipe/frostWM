"""
Stores the Preferences per-applet or shared across applets
"""
import gconf
import colorblind

class ColorblindAppletPreferences:
	def __init__(self, applet):
		# Default values
		self.GCONF_APPLET_DIR = colorblind.GCONF_DIR
		
		# These preferences are shared across all applet instances
		# unlike width, which is per-instance.
		self.GCONF_KEYBINDING_ONOFF = colorblind.GCONF_KEYBINDING_ONOFF
		self.GCONF_KEYBINDING_SWITCH = colorblind.GCONF_KEYBINDING_SWITCH

		
