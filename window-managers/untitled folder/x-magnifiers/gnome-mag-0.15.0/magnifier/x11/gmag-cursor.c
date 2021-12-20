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

#include "gmag-cursor.h"
#include "magnifier.h"
#include "magnifier-private.h"

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#include <gdk/gdkx.h>

#define CURSOR_DEBUG
#undef  CURSOR_DEBUG

gint                         fixes_event_base = 0;
static Display              *dpy;
static Window                root_window;
static gint                  fixes_error_base;


static void
gmag_cursor_cursor_convert_to_rgba (Magnifier *magnifier,
				    XFixesCursorImage *cursor_image)
{
	int i, count = cursor_image->width * cursor_image->height;
	for (i = 0; i < count; ++i) {
		guint32 pixval = GUINT_TO_LE (cursor_image->pixels[i]);
		cursor_image->pixels[i] = pixval;
	}
}

static void
gmag_cursor_free_cursor_pixels (guchar *pixels, gpointer data)
{
    XFree (data);
}

GdkPixbuf *
gmag_cursor_get_source_pixbuf (Magnifier *magnifier)
{
	XFixesCursorImage *cursor_image = XFixesGetCursorImage (dpy);
        GdkPixbuf *cursor_pixbuf = NULL;
	gchar *s1, *s2;

	if (cursor_image) {
		s1 = (char *) malloc (sizeof (char) * 6);
		s2 = (char *) malloc (sizeof (char) * 6);
	        gmag_cursor_cursor_convert_to_rgba (magnifier, cursor_image);
		cursor_pixbuf = gdk_pixbuf_new_from_data (
			(guchar *) cursor_image->pixels, GDK_COLORSPACE_RGB,
			TRUE, 8, cursor_image->width, cursor_image->height,
			cursor_image->width * 4,
			gmag_cursor_free_cursor_pixels, cursor_image);
		g_object_set_data (G_OBJECT (cursor_pixbuf), "x_hot", 
				   g_ascii_dtostr (
					   s1, 6,
					   (gdouble) cursor_image->xhot));
		g_object_set_data (G_OBJECT (cursor_pixbuf), "y_hot", 
				   g_ascii_dtostr (
					   s2, 6,
					   (gdouble) cursor_image->yhot));
	}

	return cursor_pixbuf;
}

gint
gmag_cursor_init (Magnifier *magnifier, Display *dpy_conn)
{
	if (!XFixesQueryExtension (dpy_conn, &fixes_event_base,
				   &fixes_error_base)) {
		g_warning ("XFixes extension not currently active.\n");
	} else {
		dpy = dpy_conn;
		root_window = GDK_WINDOW_XWINDOW (magnifier->priv->root);
		XFixesSelectCursorInput (dpy_conn, root_window,
					 XFixesDisplayCursorNotifyMask);
		g_message ("added event source to xfixes cursor-notify " \
			   "connection");
	}

	return fixes_event_base;
}

void
gmag_cursor_events_handler (Magnifier *magnifier, XEvent *ev)
{
	XFixesCursorNotifyEvent *cev = (XFixesCursorNotifyEvent *) ev;
	gchar *s1, *s2;

	if (magnifier->priv->use_source_cursor) {
			GdkPixbuf *cursor_pixbuf =
				gmag_cursor_get_source_pixbuf (magnifier);
			magnifier_set_cursor_from_pixbuf (magnifier,
							  cursor_pixbuf);
			s1 = g_object_get_data (G_OBJECT (cursor_pixbuf),
						  "x_hot");
			s2 = g_object_get_data (G_OBJECT (cursor_pixbuf),
						  "y_hot");
			if (s1) {
				g_free (s1);
				g_free (s2);
			}
			if (cursor_pixbuf) 
				g_object_unref (cursor_pixbuf);
	} else {
		if (cev)
			magnifier_set_cursor_pixmap_by_name (
				magnifier,
				gdk_x11_get_xatom_name (cev->cursor_name),
				TRUE);
		else
			magnifier_set_cursor_pixmap_by_name (
				magnifier, "default", TRUE);
	}
	
	magnifier_transform_cursor (magnifier);
	magnifier_zoom_regions_update_pointer (magnifier);
#ifdef CURSOR_DEBUG
	if (cev)
		g_message ("cursor changed: subtype=%d, " \
			   "cursor_serial=%lu, name=[%x] %s\n",
			   (int) cev->subtype, cev->cursor_serial,
			   (int) cev->cursor_name,
			   gdk_x11_get_xatom_name (cev->cursor_name));
#endif /* CURSOR_DEBUG */
}
