from gettext import gettext as _
import gtk
import gobject
import gnomeapplet
import colorblind
from os.path import *

class WindowUI (gobject.GObject):
    
    __gsignals__ = {
        "show-preferences": ( gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, [] ),
        "show-about": ( gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, [] ),
    }    
    
    ui = '''<ui>
        <menubar>
            <menu name="FileMenu" action="FileMenuAction">
            <separator />
            <menuitem name="Quit" action="QuitAction" />
            </menu>
            <menu name="EditMenu" action="EditMenuAction">
                <menuitem name="Preferences" action="PreferencesAction" />
            </menu>
            <menu name="HelpMenu" action="HelpMenuAction">
                <menuitem name="About" action="AboutAction" />
            </menu>
        </menubar>
    </ui>'''
    
    def __init__(self, applet, prefs):
	gobject.GObject.__init__ (self)
	self.applet = applet
	self.prefs = prefs
        
        actiongroup = gtk.ActionGroup('colorblind-window')
        actiongroup.add_actions([('FileMenuAction', None, _('_File')),
                                  ('QuitAction', gtk.STOCK_QUIT, _('_Quit'), '<Ctrl>Q', None, self.__on_quit_activate),
                                  ('EditMenuAction', None, _('_Edit')),
                                  ('PreferencesAction', gtk.STOCK_PREFERENCES, _('_Preferences'), None, None, self.__on_preferences_activate),
                                  ('AboutAction', gtk.STOCK_ABOUT, _('_About'), None, None, self.__on_about_activate),
                                  ])
        self.__accels_connected = False # We use this boolean to check when we should connect the accels - we do this on first receive_focus()
        
    def __has_toplevel_window (self):
    	"""
    	Return True if the view is in a gtk.Window.
    	"""
    	# We have to check this frequently because get_view().get_toplevel()
    	# will return the view widget if there's no toplevel window.
    	return (self.get_view().get_toplevel().flags() & gtk.TOPLEVEL)
    
    def __connect_accels (self):
    	"""
    	Connect accelerators to the toplevel window if it is present.
    	"""
    	if self.__accels_connected:
    		# We have already done this
    		return
        self.get_view().get_toplevel().add_accel_group(accelgroup)
        self.__accels_connected = True
    
    def __create_frame_with_alignment(self, label_markup):
        label = gtk.Label()
        label.set_markup( label_markup )
        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_NONE)
        frame.set_label_widget(label)
        alignment = gtk.Alignment(xscale=1.0, yscale=1.0)
        alignment.set_padding(0, 0, 12, 0)
        frame.add(alignment)
        
        label.show()
        alignment.show()
        
        return frame
    
    def __on_quit_activate(self, widget):
        self.close_view()
        gtk.main_quit ()
        
    def __on_preferences_activate(self, widget):
        self.emit('show-preferences')
    
    def __on_about_activate(self, widget):
        self.emit('show-about')
        return
