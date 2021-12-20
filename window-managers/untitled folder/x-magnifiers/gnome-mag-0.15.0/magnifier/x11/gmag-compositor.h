/*
 * GNOME-MAG Magnification service for GNOME
 *
 * Copyright 2006 Carlos Eduardo Rodrigues Diógenes
 * Copyright 2004 Sun Microsystems Inc. (damage-client.c)
 * Copyright 2001 Sun Microsystems Inc. (magnifier.c)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GMAG_COMPOSITOR_H_
#define GMAG_COMPOSITOR_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#ifdef HAVE_COMPOSITE
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xcomposite.h>
#endif /* HAVE_COMPOSITE */

#include <glib.h>

typedef struct _GmagWin {
	Window            xwin;
	Damage            damage;
#ifdef HAVE_COMPOSITE
	Picture           pic;
#endif /* HAVE_COMPOSITE */
	XWindowAttributes attr;
	XserverRegion     clip;
	XserverRegion     win_region;
} GmagWin, *GmagWinPtr;

void     gmag_compositor_init (Magnifier *magnifier, Display *dpy_conn);
gboolean gmag_compositor_events_handler (Magnifier *magnifier, XEvent *ev);
void     gmag_compositor_paint_screen ();

#endif /* GMAG_COMPOSITOR_H_ */
