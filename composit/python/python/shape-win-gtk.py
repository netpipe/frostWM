#!/usr/bin/env python

#PyCairoShape clock
#(c) 2007 Nicolas Trangez - ikke at nicolast dot be
#Released under GPL v2

#All imports we need
import sys
import gobject
import pango
import pygtk
pygtk.require('2.0')
import gtk
from gtk import gdk
import cairo
from datetime import datetime
import math

#Yeah, globals. Warning: this code is messy
supports_alpha = False
# dreblen - At this point, win doesn't need to be global
#win = None
# /dreblen
shift_pressed = False
img = None

# dreblen - You can use the widget argument (renamed to win) instead of global win
def expose(win, event):   # -widget, +win
# /dreblen
    global supports_alpha, img

    pixbuf = gdk.pixbuf_new_from_file('1.svg')
    pix, mask = pixbuf.render_pixmap_and_mask()
    win.shape_combine_mask(mask, 0, 0)

    return False

#If screen changes, it could be possible we no longer got rgba colors
def screen_changed(widget, old_screen=None):
   
    global supports_alpha
   
    screen = widget.get_screen()
    colormap = screen.get_rgba_colormap()
    supports_alpha = True   
    widget.set_colormap(colormap)
   
    return True

#Queue a redraw of our window every second
def update_clock():
#    global win
#    win.queue_draw()
    return True

#Our widget (window) is clicked
# dreblen - You can use the widget argument (renamed to win) instead of global win
def clicked(win, event):   # -widget, +win
# No need for global
#    global win
# /dreblen
    win.begin_move_drag(event.button, int(event.x_root), int(event.y_root), event.time)

#This should be obvious
def main(args):
# dreblen - no need for global
#    global win
# /dreblen
   
    win = gtk.Window()

    #win.set_property("skip-taskbar-hint", True)
   
    win.set_title('PyCairoShape clock')
    win.connect('delete-event', gtk.main_quit)

    win.set_app_paintable(True)
   
    win.connect('expose-event', expose)
    win.connect('screen-changed', screen_changed)

    win.set_decorated(True)
    win.add_events(gdk.BUTTON_PRESS_MASK)
    win.connect('button-press-event', clicked)
   
    screen_changed(win)

    gobject.timeout_add(1000, update_clock)

    win.show_all()
    gtk.main()
   
    return True

if __name__ == '__main__':
    sys.exit(main(sys.argv)) 
