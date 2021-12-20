from gettext import gettext as _
from os.path import join, isdir
import os
import struct
import gtk, gtk.gdk, gtk.glade, gobject, gconf
import colorblind, colorblind.Utils, colorblind.ColorblindApplet

MAXINT = 2 ** ((8 * struct.calcsize('i')) - 1) - 1

class AccelEntry( gobject.GObject ):

	__gsignals__ = {
		'accel-edited': (gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE,
						 [gobject.TYPE_STRING, gobject.TYPE_UINT, gobject.TYPE_UINT, gobject.TYPE_UINT]),
		'keyboard-shortcut' : (gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, [gobject.TYPE_STRING, gobject.TYPE_UINT]),
	}
	__gproperties__ = {
		'accel_key': ( gobject.TYPE_UINT, "Accelerator key", "Accelerator key", 0, MAXINT, 0, gobject.PARAM_READWRITE ),
		'accel_mods': ( gobject.TYPE_FLAGS, "Accelerator modifiers", "Accelerator modifiers", 0, gobject.PARAM_READWRITE ),
		'keycode': ( gobject.TYPE_UINT, "Accelerator keycode", "Accelerator keycode", 0, MAXINT, 0, gobject.PARAM_READWRITE ),
	}
	
	FORBIDDEN_KEYS = (gtk.keysyms.BackSpace, gtk.keysyms.Begin, gtk.keysyms.Delete, gtk.keysyms.End, gtk.keysyms.Down,
					  gtk.keysyms.Home, gtk.keysyms.Insert, gtk.keysyms.Left, gtk.keysyms.Page_Down, gtk.keysyms.Page_Up,
					  gtk.keysyms.Right, gtk.keysyms.Tab, gtk.keysyms.Up, gtk.keysyms.bar, gtk.keysyms.braceleft,
					  gtk.keysyms.braceright, gtk.keysyms.colon, gtk.keysyms.comma, gtk.keysyms.dollar, gtk.keysyms.equal,
					  gtk.keysyms.exclam, gtk.keysyms.hyphen, gtk.keysyms.period, gtk.keysyms.percent, gtk.keysyms.question,
					  gtk.keysyms.quotedbl, gtk.keysyms.semicolon, gtk.keysyms.slash, gtk.keysyms.space, gtk.keysyms.underscore)

	def __init__(self, accel_name='', cb_pref_ui = None):
		self.__old_value = None
		self._attributes = {'accel_key': 0, 'accel_mods': 0, 'keycode': 0}
		self._cb_pref_ui = cb_pref_ui
		gobject.GObject.__init__(self)
		
		self.entry = gtk.Entry()
		self.entry.set_property('editable', False)
		self.entry.connect('button-press-event', self.__on_button_press_event)
		self.entry.connect('key-press-event', self.__on_key_press_event)
		self.entry.connect('focus-out-event', self.__on_focus_out_event)

		self.set_accelerator_name(accel_name)

	def do_get_property(self, pspec):
		if pspec.name in ('accel_key', 'accel_mods', 'keycode'):
			return self._attributes[pspec.name]
	
	def do_set_property(self, pspec, value):
		if pspec.name == 'accel_key':
			self.set_accelerator(int(value), self.get_property('keycode'), self.get_property('accel_mask'))
		elif pspec.name == 'accel_mods':
			self.set_accelerator(self.get_property('accel_key'), self.get_property('keycode'), int(value))
		elif pspec.name == 'keycode':
			self.set_accelerator(self.get_property('accel_key'), int(value), self.get_property('accel_mask'))

	def get_accelerator_name(self):
		return self.entry.get_text()

	def set_accelerator_name(self, value):
		if value == None:
			value = ""
			
		(keyval, mods) = gtk.accelerator_parse(value)
		if gtk.accelerator_valid(keyval, mods):
			self.entry.set_text(value)
		return

	def get_accelerator(self):
		return ( self.get_property('accel_key'), self.get_property('keycode'), self.get_property('accel_mods') )

	def set_accelerator(self, keyval, mods, keycode):
		changed = False
		self.freeze_notify()
		if keyval != self._attributes['accel_key']:
			self._attributes['accel_key'] = keyval
			self.notify('accel_key')
			changed = True
			
		if mods != self._attributes['accel_mods']:
			self._attributes['accel_mods'] = mods
			self.notify('accel_mods')
			changed = True
			
		if keycode != self._attributes['keycode']:
			self._attributes['keycode'] = keycode
			self.notify('keycode')
			changed = True
			
		self.thaw_notify()
		if changed:
			text = self.__convert_keysym_state_to_string (keyval, keycode, mods)
			self.entry.set_text(text)
			
	def __convert_keysym_state_to_string(self, keysym, keycode, mask):		
		name = gtk.accelerator_name(keysym, mask)
		if keysym == 0:
			name = "%s0x%02x" % (name, keycode)
		return name

	def get_widget(self):
		return self.entry

	def __on_button_press_event(self, entry, event):
		self.__old_value = self.entry.get_text()
		entry.set_text( _('New accelerator...') )
		entry.grab_focus()
		return True

	def __on_key_press_event(self, entry, event):
		accel_mods = 0
		edited = False
		
		keymap = gtk.gdk.keymap_get_default()
		translation = keymap.translate_keyboard_state(event.hardware_keycode, event.state, event.group)
		if translation == None:
			consumed_modifiers = 0
		else:
			(keyval, egroup, level, consumed_modifiers) = translation
		
		accel_keyval = gtk.gdk.keyval_to_lower(event.keyval)
		if (accel_keyval == gtk.keysyms.ISO_Left_Tab):
		    accel_keyval = gtk.keysyms.Tab
		
		accel_mods = event.state & gtk.accelerator_get_default_mod_mask()
		
		# Filter consumed modifiers		
		accel_mods &= ~consumed_modifiers
		
  		# Put shift back if it changed the case of the key, not otherwise.
		if (accel_keyval != event.keyval):
			accel_mods |= gtk.gdk.SHIFT_MASK
			
		if accel_mods == 0:
			if accel_keyval == gtk.keysyms.Escape:
				self.__revert()
				return
						
		# Do not make keyboard unusable
		if ( ((accel_mods == 0 or accel_mods == gtk.gdk.SHIFT_MASK) and accel_keyval >= gtk.keysyms.a and accel_keyval <= gtk.keysyms.z) # alphabet
			or (accel_mods == 0 and (
				   (accel_keyval >= 48 and accel_keyval <= 57) # number keys
				or (accel_keyval >= gtk.keysyms.kana_fullstop and accel_keyval <= gtk.keysyms.semivoicedsound)
	   			or (accel_keyval >= gtk.keysyms.Arabic_comma and accel_keyval <= gtk.keysyms.Arabic_sukun)
	   	 		or (accel_keyval >= gtk.keysyms.Serbian_dje and accel_keyval <= gtk.keysyms.Cyrillic_HARDSIGN)
	   	  		or (accel_keyval >= gtk.keysyms.Greek_ALPHAaccent and accel_keyval <= gtk.keysyms.Greek_omega)
	   	   		or (accel_keyval >= gtk.keysyms.hebrew_doublelowline and accel_keyval <= gtk.keysyms.hebrew_taf)
   	   			or (accel_keyval >= gtk.keysyms.Thai_kokai and accel_keyval <= gtk.keysyms.Thai_lekkao)
   	   			or (accel_keyval >= gtk.keysyms.Hangul and accel_keyval <= gtk.keysyms.Hangul_Special)
   	   			or (accel_keyval >= gtk.keysyms.Hangul_Kiyeog and accel_keyval <= gtk.keysyms.Hangul_J_YeorinHieuh)
   	   		))
			or (accel_keyval in self.FORBIDDEN_KEYS)
			or (accel_keyval == 0 and accel_mods != 0) ):
			dialog = gtk.MessageDialog (self.entry.get_toplevel(),
				gtk.DIALOG_DESTROY_WITH_PARENT | gtk.DIALOG_MODAL,
				gtk.MESSAGE_WARNING,
				gtk.BUTTONS_CANCEL,
				_("The shortcut \"%s\" cannot be used because it will become impossible to type using this key.\nPlease try with a key such as Control, Alt or Shift at the same time.\n")
				% gtk.accelerator_name(accel_keyval, accel_mods)
			)
			dialog.run()
			dialog.destroy()
			self.__revert()
			return
						
		if not gtk.accelerator_valid(accel_keyval, accel_mods):
			self.__revert()
			return
		
		accel_name = self.__convert_keysym_state_to_string(accel_keyval, event.hardware_keycode, accel_mods)

		# Yeah, I know, this is very ugly code, but I don't know how do
		# it better for now... patches are welcome :-)
		if entry == self._cb_pref_ui.get_on_off_entry().entry:
			keybinding_switch_string = colorblind.GCONF_CLIENT.get_string(colorblind.GCONF_KEYBINDING_SWITCH)
			if accel_name == keybinding_switch_string:
				return
		elif entry == self._cb_pref_ui.get_switch_entry().entry:
			keybinding_on_off_string = colorblind.GCONF_CLIENT.get_string(colorblind.GCONF_KEYBINDING_ONOFF)
			if accel_name == keybinding_on_off_string:
				return

		self.set_accelerator(accel_keyval, accel_mods, event.hardware_keycode)
		self.__old_value = None
		self.emit('accel-edited', accel_name, accel_keyval, accel_mods, event.hardware_keycode)
		return True

	def __on_focus_out_event(self, entry, event):
		if self.__old_value != None:
			self.__revert()
	
	def __revert(self):
		self.set_accelerator_name(self.__old_value)

class ColorblindPreferencesUI:
	CB_FILTER_NO_FILTER, CB_FILTER_SELECTIVE_SATURATE_RED, CB_FILTER_SELECTIVE_SATURATE_GREEN, CB_FILTER_SELECTIVE_SATURATE_BLUE, CB_FILTER_SELECTIVE_DESATURATE_RED, CB_FILTER_SELECTIVE_DESATURATE_GREEN, CB_FILTER_SELECTIVE_DESATURATE_BLUE, CB_FILTER_POSITIVE_HUE_SHIFT, CB_FILTER_NEGATIVE_HUE_SHIFT = range (9)

	def __init__(self, applet):
		self.glade = gtk.glade.XML(join(colorblind.SHARED_DATA_DIR, "colorblind-prefs.glade"))

		self.dialog = self.glade.get_widget("window_preferences")
		self.radio1 = self.glade.get_widget("radio_filter_red_saturate")
		self.radio2 = self.glade.get_widget("radio_filter_green_saturate")
		self.radio3 = self.glade.get_widget("radio_filter_blue_saturate")
		self.radio4 = self.glade.get_widget("radio_filter_red_desaturate")
		self.radio5 = self.glade.get_widget("radio_filter_green_desaturate")
		self.radio6 = self.glade.get_widget("radio_filter_blue_desaturate")
		self.radio7 = self.glade.get_widget("radio_filter_positive_hue_shift")
		self.radio8 = self.glade.get_widget("radio_filter_negative_hue_shift")
		self.hbox_default = self.glade.get_widget("hbox_default")
		self.combofilters = gtk.combo_box_new_text ()
		self.hbox_default.add (self.combofilters)
		self.combofilters.append_text (_("Selective Red Saturation"))
		self.combofilters.append_text (_("Selective Green Saturation"))
		self.combofilters.append_text (_("Selective Blue Saturation"))
		self.combofilters.append_text (_("Selective Red Desaturation"))
		self.combofilters.append_text (_("Selective Green Desaturation"))
		self.combofilters.append_text (_("Selective Blue Desaturation"))
		self.combofilters.append_text (_("Positive Hue Shift"))
		self.combofilters.append_text (_("Negative Hue Shift"))
		self.check_default_filter = self.glade.get_widget("check_default_filter")
		self.button_close = self.glade.get_widget("button_close")
		
		# Retreive current values
		self.keybinding_on_off = colorblind.GCONF_CLIENT.get_string(applet.prefs.GCONF_KEYBINDING_ONOFF)
		self.keyboard_shortcut_entry_on_off = AccelEntry("<Shift><Control>c", self)
		self.keyboard_shortcut_entry_on_off.connect('accel-edited', self.on_keyboard_shortcut_entry_changed, colorblind.GCONF_KEYBINDING_ONOFF)
		self.glade.get_widget("table_shortcut").attach(self.keyboard_shortcut_entry_on_off.get_widget(), 1, 2, 0, 1)

		self.keybinding_switch = colorblind.GCONF_CLIENT.get_string(applet.prefs.GCONF_KEYBINDING_SWITCH)
		self.keyboard_shortcut_entry_switch = AccelEntry("<Shift><Control>n", self)
		self.keyboard_shortcut_entry_switch.connect('accel-edited', self.on_keyboard_shortcut_entry_changed, colorblind.GCONF_KEYBINDING_SWITCH)
		self.glade.get_widget("table_shortcut").attach(self.keyboard_shortcut_entry_switch.get_widget(), 1, 2, 1, 2)

		colorblind.GCONF_CLIENT.notify_add (colorblind.GCONF_COLORBLIND_FILTER, self.sync_ui)
		self.colorblind_filter = colorblind.GCONF_CLIENT.get_int(colorblind.GCONF_COLORBLIND_FILTER)
		if self.colorblind_filter == None:
			self.colorblind_filter = self.CB_FILTER_POSITIVE_HUE_SHIFT
		colorblind.GCONF_CLIENT.notify_add (colorblind.GCONF_DEFAULT_FILTER, self.sync_ui)
		self.default_filter = colorblind.GCONF_CLIENT.get_int(colorblind.GCONF_DEFAULT_FILTER)
		if self.colorblind_filter == None:
			self.colorblind_filter = self.CB_FILTER_POSITIVE_HUE_SHIFT
		self.check_default = colorblind.GCONF_CLIENT.get_bool(colorblind.GCONF_CHECK_DEFAULT)
		if self.check_default == None:
			self.check_default = False

		self.sync_ui ()

		self.set_ui_signals ()

	def __on_radio_toggled_event (self, entry):
		if entry == self.radio1:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_RED)
		elif entry == self.radio2:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_GREEN)
		elif entry == self.radio3:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_BLUE)
		elif entry == self.radio4:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_RED)
		elif entry == self.radio5:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_GREEN)
		elif entry == self.radio6:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_BLUE)
		elif entry == self.radio7:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_POSITIVE_HUE_SHIFT)
		elif entry == self.radio8:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_COLORBLIND_FILTER, self.CB_FILTER_NEGATIVE_HUE_SHIFT)
		colorblind.ColorblindApplet.active_actual_filter ()

	def __on_changed_event (self, entry):
		i = entry.get_active () + 1
		if i == self.CB_FILTER_SELECTIVE_SATURATE_RED:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_RED)
		elif i == self.CB_FILTER_SELECTIVE_SATURATE_GREEN:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_GREEN)
		elif i == self.CB_FILTER_SELECTIVE_SATURATE_BLUE:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_SATURATE_BLUE)
		elif i == self.CB_FILTER_SELECTIVE_DESATURATE_RED:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_RED)
		elif i == self.CB_FILTER_SELECTIVE_DESATURATE_GREEN:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_GREEN)
		elif i == self.CB_FILTER_SELECTIVE_DESATURATE_BLUE:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_SELECTIVE_DESATURATE_BLUE)
		elif i == self.CB_FILTER_POSITIVE_HUE_SHIFT:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_POSITIVE_HUE_SHIFT)
		elif i == self.CB_FILTER_NEGATIVE_HUE_SHIFT:
			colorblind.GCONF_CLIENT.set_int(colorblind.GCONF_DEFAULT_FILTER, self.CB_FILTER_NEGATIVE_HUE_SHIFT)

	def __on_check_toggled_event (self, entry):
		state = entry.get_active ()
		colorblind.GCONF_CLIENT.set_bool(colorblind.GCONF_CHECK_DEFAULT, state)

	def __on_click_button_close_event (self, entry):
		self.dialog.emit ('delete-event', gtk.gdk.Event (gtk.gdk.DELETE))

	def __on_dialog_delete_event (self, entry, *extra):
		self.dialog.destroy ()
		colorblind.ColorblindApplet.pref_ui_unmapped ()

	def set_ui_signals (self):
		self.dialog.connect('delete-event', self.__on_dialog_delete_event)
		self.radio1.connect('toggled', self.__on_radio_toggled_event)
		self.radio2.connect('toggled', self.__on_radio_toggled_event)
		self.radio3.connect('toggled', self.__on_radio_toggled_event)
		self.radio4.connect('toggled', self.__on_radio_toggled_event)
		self.radio5.connect('toggled', self.__on_radio_toggled_event)
		self.radio6.connect('toggled', self.__on_radio_toggled_event)
		self.radio7.connect('toggled', self.__on_radio_toggled_event)
		self.radio8.connect('toggled', self.__on_radio_toggled_event)

		self.combofilters.connect('changed', self.__on_changed_event)

		self.check_default_filter.connect('toggled', self.__on_check_toggled_event)
		self.button_close.connect ('clicked', self.__on_click_button_close_event)
		
	def show_run_hide(self, parent):
		self.sync_ui ()
		self.dialog.set_screen(parent.get_screen())
		self.dialog.show_all ()
		self.mapped = True

	def get_on_off_entry (self):
		return self.keyboard_shortcut_entry_on_off

	def get_switch_entry (self):
		return self.keyboard_shortcut_entry_switch
	
	def sync_ui(self, *extra):
		if self.keybinding_on_off != None:
			self.keyboard_shortcut_entry_on_off.set_accelerator_name(self.keybinding_on_off)
		else:
			self.keyboard_shortcut_entry_on_off.set_accelerator_name("<Shift><Control>c")
		if self.keybinding_switch != None:
			self.keyboard_shortcut_entry_switch.set_accelerator_name(self.keybinding_switch)
		else:
			self.keyboard_shortcut_entry_switch.set_accelerator_name("<Shift><Control>n")

		self.colorblind_filter = colorblind.GCONF_CLIENT.get_int (colorblind.GCONF_COLORBLIND_FILTER)
		self.default_filter = colorblind.GCONF_CLIENT.get_int (colorblind.GCONF_DEFAULT_FILTER)
		self.check_default = colorblind.GCONF_CLIENT.get_bool (colorblind.GCONF_CHECK_DEFAULT)

		if self.colorblind_filter == 1:
			self.radio1.set_active (True)
		elif self.colorblind_filter == 2:
			self.radio2.set_active (True)
		elif self.colorblind_filter == 3:
			self.radio3.set_active (True)
		elif self.colorblind_filter == 4:
			self.radio4.set_active (True)
		elif self.colorblind_filter == 5:
			self.radio5.set_active (True)
		elif self.colorblind_filter == 6:
			self.radio6.set_active (True)
		elif self.colorblind_filter == 7:
			self.radio7.set_active (True)
		elif self.colorblind_filter == 8:
			self.radio8.set_active (True)
		else:
			self.radio7.set_active (True)

		if self.default_filter == 1:
			self.combofilters.set_active (0)
		elif self.default_filter == 2:
			self.combofilters.set_active (1)
		elif self.default_filter == 3:
			self.combofilters.set_active (2)
		elif self.default_filter == 4:
			self.combofilters.set_active (3)
		elif self.default_filter == 5:
			self.combofilters.set_active (4)
		elif self.default_filter == 6:
			self.combofilters.set_active (5)
		elif self.default_filter == 7:
			self.combofilters.set_active (6)
		elif self.default_filter == 8:
			self.combofilters.set_active (7)
		else:
			self.combofilters.set_active (6)

		if self.check_default == True:
			self.check_default_filter.set_active (True)
		else:
			self.check_default_filter.set_active (False)
			
	def on_keyboard_shortcut_entry_changed(self, entry, accel_name, keyval, mods, keycode, keybinding):		
		if accel_name != "":
			colorblind.GCONF_CLIENT.set_string(keybinding, accel_name)
		return False

def show_preferences(applet):
	ColorblindPreferencesUI (applet).show_run_hide(applet.applet)
