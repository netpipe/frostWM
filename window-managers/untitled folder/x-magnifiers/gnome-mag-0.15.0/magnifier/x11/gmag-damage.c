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

#include "magnifier.h"
#include "magnifier-private.h"
#include "gmag-graphical-server.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>

#include <gdk/gdkx.h>

#define DAMAGE_DEBUG
#undef  DAMAGE_DEBUG

static Magnifier         *mag_ref;
static Display           *dpy;
static Window             root_window;
static Damage             root_damage;
static XserverRegion      tmp_region;
static DamageHandlerFunc  gmag_damage_notify_handler;

static void
gmag_damage_notify_handler (XEvent *ev)
{
	XDamageNotifyEvent *dev = (XDamageNotifyEvent *) ev;
	XRectangle         *rectlist;

#ifdef DAMAGE_DEBUG
	g_message ("Damage area %3d, %3d x %3d, %3d",
		   (int) dev->area.x, (int) dev->area.x + dev->area.width,
		   (int) dev->area.y, (int) dev->area.y + dev->area.height);
	g_message ("Damage geometry %3d, %3d x %3d, %3d",
		   (int) dev->geometry.x,
		   (int) dev->geometry.x + dev->geometry.width,
		   (int) dev->geometry.y,
		   (int) dev->geometry.y + dev->geometry.height);
#endif /* DAMAGE_DEBUG */

	XDamageSubtract (dpy, dev->damage, None, tmp_region);
	
	if (mag_ref) {
		int i, howmany;
		
		rectlist = XFixesFetchRegion (dpy, tmp_region, &howmany);

		if (rectlist == NULL) /* no reply from fetch */
			return;
		for (i = 0; i < howmany; ++i) {
			gmag_gs_notify_damage (mag_ref, &rectlist[i]);
		}
		XFree (rectlist);
	}
}

void
gmag_damage_init (Magnifier *magnifier, Display *dpy_conn)
{
	gint event, error;

	if (!XDamageQueryExtension (dpy_conn, &event, &error)) {
		g_warning ("Damage extension not currently active.\n");
	} else if (g_getenv ("MAGNIFIER_IGNORE_DAMAGE")) {
		g_warning ("Damage extension being ignored at user request.");
	} else {
		dpy = dpy_conn;
		mag_ref = magnifier;
		tmp_region = XFixesCreateRegion (dpy, 0, 0);
		root_window = GDK_WINDOW_XWINDOW (mag_ref->priv->root);
		root_damage = XDamageCreate (dpy, root_window,
					     XDamageReportDeltaRectangles);
		/* I don't know why, but without this XDamageSubtract
		 * call below the damage events aren't hanled normally.
		 * They start to be handled normally, without the call
		 * below, only after you move your mouse.
		 */
		XDamageSubtract (dpy, root_damage, None, None);
		gmag_gs_set_damage_handler_func (gmag_damage_notify_handler);

		g_message ("added event source to damage connection");
	}
}
