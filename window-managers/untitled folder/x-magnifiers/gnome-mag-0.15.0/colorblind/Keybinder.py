import gtk, gobject, gconf
import colorblind, colorblind.keybinder

class Keybinder(gobject.GObject):
	__gsignals__ = {
		"activated" : (gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, []),
		# When the keybinding changes, passes a boolean indicating wether the keybinding is successful
		"changed" : (gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, [gobject.TYPE_BOOLEAN]),
	}

	def __init__(self, keybinding):
		gobject.GObject.__init__(self)
		
		self.bound = False
		self.prevbinding = None
		
		# Set and retreive global keybinding from gconf
		self.keybinding = colorblind.GCONF_CLIENT.get_string(keybinding)
		colorblind.GCONF_CLIENT.notify_add(keybinding, lambda x, y, z, a: self.on_config_keybinding(z.value))
		
		self.bind()
		
	def on_config_keybinding(self, value=None):
		if value != None and value.type == gconf.VALUE_STRING:
			self.prevbinding = self.keybinding
			self.keybinding = value.get_string()
			self.bind()

	def on_keyboard_shortcut(self):
		print "Keyboard shortcut"
		self.emit ("activated")
	
	def bind(self):
		if self.bound:
			self.unbind()
			
		try:
			print 'Binding Global shortcut %s to focus the colorblind' % self.keybinding
			colorblind.keybinder.tomboy_keybinder_bind(self.keybinding, self.on_keyboard_shortcut)
			self.bound = True
		except KeyError:
			# if the requested keybinding conflicts with an existing one, a KeyError will be thrown
			self.bound = False
		
		self.emit('changed', self.bound)
					
	def unbind(self):
		try:
			colorblind.keybinder.tomboy_keybinder_unbind(self.prevbinding)
			self.bound = False
		except KeyError:
			# if the requested keybinding is not bound, a KeyError will be thrown
			pass

if gtk.pygtk_version < (2,8,0):
	gobject.type_register(Keybinder)

keybinder_on_off = Keybinder(colorblind.GCONF_KEYBINDING_ONOFF)
keybinder_switch = Keybinder(colorblind.GCONF_KEYBINDING_SWITCH)
def get_colorblind_keybinder_on_off():
	return keybinder_on_off

def get_colorblind_keybinder_switch():
	return keybinder_switch
