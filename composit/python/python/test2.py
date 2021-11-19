#!/usr/bin/env python

import sys
import cairo
import rsvg
import gtk
import math

BORDER_WIDTH = 1


def delete_cb(win, event):
    gtk.main_quit()


def expose_cairo(win, event, svg):

    x, y, w, h = win.allocation
    win.set_size_request(500, 400)
    cr = win.window.cairo_create()
    cr.set_source_color(win.style.fg[win.state])

    cr.rectangle(10, 200, 120, 80)
    cr.stroke()

    cr.arc(300, 240, 40, 0, 2*math.pi)
    cr.stroke()

#
#  Is there a way to get the image to appear (properly scaled) in the 
#  rectangle and circle defined above?
#
    if svg != None:
        svg.render_cairo(cr)

    return True

def main():
    win = gtk.Window ()
    win.connect("delete-event", delete_cb)


    svg = rsvg.Handle(file="1.svg") #any svg file

    win.connect("expose-event", expose_cairo, svg)

    print svg.props.width, svg.props.height, svg.props.em, svg.props.ex

    win.show_all()
    win.connect("destroy", lambda w: gtk.main_quit())
    gtk.main()

if __name__ == '__main__':
    main()
