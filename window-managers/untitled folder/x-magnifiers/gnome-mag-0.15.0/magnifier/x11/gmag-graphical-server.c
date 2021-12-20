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

#include "config.h"
#include "magnifier.h"
#include "magnifier-private.h"
#include "gmag-graphical-server.h"
#include "gmag-cursor.h"
#include "gmag-damage.h"
#ifdef HAVE_COMPOSITE
#include "gmag-compositor.h"
#endif /* HAVE_COMPOSIE */

#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#ifdef HAVE_COMPOSITE
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xcomposite.h>
#ifdef HAVE_SHAPEINPUT
#include <X11/extensions/shape.h>
#endif /* HAVE_SHAPEINPUT */
#endif /* HAVE_COMPOSITE */

#include <stdlib.h>

#include <glib.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

enum {
	STRUT_LEFT = 0,
	STRUT_RIGHT = 1,
	STRUT_TOP = 2,
	STRUT_BOTTOM = 3,
	STRUT_LEFT_START = 4,
	STRUT_LEFT_END = 5,
	STRUT_RIGHT_START = 6,
	STRUT_RIGHT_END = 7,
	STRUT_TOP_START = 8,
	STRUT_TOP_END = 9,
	STRUT_BOTTOM_START = 10,
	STRUT_BOTTOM_END = 11
};

static Display           *dpy = NULL;
static guint              dpy_gsource = 0;
static Window             root_window, mag_window;
static gboolean           use_damage, use_compositor;
static gint               fixes_event_base = 0;
static DamageHandlerFunc *damage_events_handler = NULL;
static gint               damage_event_base = 0;
static gint               _x_error = 0;

#define EVENTS_DEBUG
#undef  EVENTS_DEBUG

static void
gmag_gs_cursor_convert_to_rgba (Magnifier *magnifier,
				XFixesCursorImage *cursor_image)
{
	int i, count = cursor_image->width * cursor_image->height;
	for (i = 0; i < count; ++i) {
		guint32 pixval = GUINT_TO_LE (cursor_image->pixels[i]);
		cursor_image->pixels[i] = pixval;
	}
}

static void
gmag_gs_free_cursor_pixels (guchar *pixels, gpointer data)
{
    /* XFree (data); FIXME why doesn't this work properly? */
}

GdkPixbuf *
gmag_gs_get_source_pixbuf (Magnifier *magnifier)
{
	XFixesCursorImage *cursor_image = XFixesGetCursorImage (dpy);
        GdkPixbuf *cursor_pixbuf = NULL;
	gchar s[6];

	if (cursor_image) {
	        gmag_gs_cursor_convert_to_rgba (magnifier, cursor_image);
		cursor_pixbuf = gdk_pixbuf_new_from_data (
			(guchar *) cursor_image->pixels, GDK_COLORSPACE_RGB,
			TRUE, 8, cursor_image->width, cursor_image->height,
			cursor_image->width * 4,
			gmag_gs_free_cursor_pixels, cursor_image);
		gdk_pixbuf_set_option (cursor_pixbuf, "x_hot", 
				       g_ascii_dtostr (
					       s, 6,
					       (gdouble) cursor_image->xhot));
		gdk_pixbuf_set_option (cursor_pixbuf, "y_hot", 
				       g_ascii_dtostr (
					       s, 6,
					       (gdouble) cursor_image->yhot));
	}

	return cursor_pixbuf;
}

gboolean
gmag_gs_source_has_damage_extension (Magnifier *magnifier)
{
	gint event_base, error_base;
	Display *dpy;
	g_assert (magnifier);
	dpy = GDK_DISPLAY_XDISPLAY (magnifier->source_display);
	if (g_getenv ("MAGNIFIER_IGNORE_DAMAGE"))
	        return FALSE;
	if (XDamageQueryExtension (dpy, &event_base, &error_base))
		return TRUE;
	return FALSE;
}

void
gmag_gs_notify_damage (Magnifier *magnifier, XRectangle *rect)
{
	GdkRectangle gdk_rect;

	gdk_rect.x      = rect->x;
	gdk_rect.y      = rect->y;
	gdk_rect.width  = rect->width;
	gdk_rect.height = rect->height;

	magnifier_notify_damage (magnifier, &gdk_rect);
}

gboolean
gmag_gs_check_set_struts (Magnifier *magnifier)
{
#ifdef HAVE_COMPOSITE
	if (gmag_gs_use_compositor (magnifier))
		return TRUE;
#endif

	if (magnifier &&
	    magnifier->priv && magnifier->priv->w && GTK_WIDGET_REALIZED (magnifier->priv->w) &&
	    magnifier->priv->w->window) 
	{
		Atom atom_strut = gdk_x11_get_xatom_by_name ("_NET_WM_STRUT");
		Atom atom_strut_partial = gdk_x11_get_xatom_by_name ("_NET_WM_STRUT_PARTIAL");
		guint32 struts[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		GtkWidget *widget = magnifier->priv->w;
		gint width = gdk_screen_get_width (
			gdk_display_get_screen (magnifier->target_display,
						magnifier->target_screen_num));
		gint height = gdk_screen_get_height (
			gdk_display_get_screen (magnifier->target_display,
						magnifier->target_screen_num));

		gint right_margin, left_margin, top_margin, bottom_margin;
		gint wx, wy, ww, wh;

		gtk_window_get_position (GTK_WINDOW (magnifier->priv->w), &wx, &wy);
		gtk_window_get_size (GTK_WINDOW (magnifier->priv->w), &ww, &wh);

		left_margin = wx;
		right_margin = (width - ww) - wx;
		top_margin = wy;
		bottom_margin = (height - wh) - wy;

		/* set the WM_STRUT properties on the appropriate side */
		if (bottom_margin > top_margin && 
		    bottom_margin > left_margin &&
		    bottom_margin > right_margin)
		{
			struts[STRUT_TOP] = wh + wy;
			struts[STRUT_TOP_START] = wx;
			struts[STRUT_TOP_END] = wx + ww;
		} 
		else if (top_margin > bottom_margin && 
			 top_margin > left_margin &&
			 top_margin > right_margin)
		{
			struts[STRUT_BOTTOM] = height - wy;
			struts[STRUT_BOTTOM_START] = wx;
			struts[STRUT_BOTTOM_END] = wx + ww;
		}
		else if (right_margin > left_margin &&
			 right_margin > top_margin &&
			 right_margin > bottom_margin)
		{
			struts[STRUT_LEFT] = wx;
			struts[STRUT_LEFT_START] = wy;
			struts[STRUT_LEFT_END] = wh + wy;
		}
		else 
		{
			struts[STRUT_RIGHT] = width - wx;
			struts[STRUT_RIGHT_START] = wy;
			struts[STRUT_RIGHT_END] = wy + wh;
		}
		
		gdk_error_trap_push ();
		XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window), 
				 GDK_WINDOW_XWINDOW (widget->window), 
				 atom_strut,
				 XA_CARDINAL, 32, PropModeReplace,
				 (guchar *) &struts, 4);
		XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window), 
				 GDK_WINDOW_XWINDOW (widget->window), 
				 atom_strut_partial,
				 XA_CARDINAL, 32, PropModeReplace,
				 (guchar *) &struts, 12); 
		gdk_error_trap_pop ();

#ifdef DEBUG_STRUTS
		g_message ("struts TOP %d (%d - %d)", struts[STRUT_TOP], struts[STRUT_TOP_START], struts[STRUT_TOP_END]);
		g_message ("struts BOTTOM %d (%d - %d)", struts[STRUT_BOTTOM], struts[STRUT_BOTTOM_START], struts[STRUT_BOTTOM_END]);
		g_message ("struts LEFT %d (%d - %d)", struts[STRUT_LEFT], struts[STRUT_LEFT_START], struts[STRUT_LEFT_END]);
		g_message ("struts RIGHT %d (%d - %d)", struts[STRUT_RIGHT], struts[STRUT_RIGHT_START], struts[STRUT_RIGHT_END]);
#endif
		return TRUE;
	}
	return FALSE;
}

gboolean
gmag_gs_reset_overlay_at_idle (gpointer data)
{
#ifdef HAVE_COMPOSITE
	if (!use_compositor)
		return FALSE;
	if (data) {
		Magnifier *magnifier = MAGNIFIER (data);
		if (magnifier->priv &&
		    GTK_WIDGET_REALIZED (magnifier->priv->w)) 
			if (magnifier->priv->w->window) {
#ifdef HAVE_OVERLAY
				gint      x, y, over_w, over_h;
				if (!magnifier->priv->overlay) {
					magnifier->priv->overlay = gdk_window_foreign_new (XCompositeGetOverlayWindow (GDK_WINDOW_XDISPLAY (magnifier->priv->w->window), GDK_WINDOW_XID (magnifier->priv->root)));
					gdk_window_get_position (
						magnifier->priv->w->window, &x,
						&y);
					gdk_drawable_get_size (
						magnifier->priv->w->window,
						&over_w, &over_h);
					gdk_window_move_resize (
						magnifier->priv->overlay, x, y,
						over_w, over_h);
					gdk_window_reparent (
						magnifier->priv->w->window,
						magnifier->priv->overlay, 0, 0);
				}
#ifdef HAVE_SHAPEINPUT			
				XShapeCombineRectangles (GDK_WINDOW_XDISPLAY (magnifier->priv->w->window), GDK_WINDOW_XID (magnifier->priv->overlay), ShapeInput, 0, 0, NULL, 0, ShapeSet, YXBanded);
#endif /* HAVE_SHAPEINPUT */
#else
#ifdef HAVE_SHAPEINPUT
				XShapeCombineRectangles (GDK_WINDOW_XDISPLAY (magnifier->priv->w->window), GDK_WINDOW_XID (magnifier->priv->w->window), ShapeInput, 0, 0, NULL, 0, ShapeSet, YXBanded);
#endif /* HAVE_SHAPEINPUT */
#endif /* HAVE_OVERLAY */
				return FALSE;
			}
	}
	return TRUE;
#endif /* HAVE_COMPOSITE */
	return FALSE;
}

gboolean
gmag_gs_error_check (void)
{
	if (_x_error) {
		_x_error = 0;
		return TRUE;
	}
	return FALSE;
}

static int
gmag_gs_x_error_handler (Display *display, XErrorEvent *error)
{
	if (error->error_code == BadAlloc) {
		_x_error = error->error_code;
	}
	else {
		return -1;
	}
	return 0;
}

void
gmag_gs_magnifier_realize (GtkWidget *widget)
{
	XWMHints wm_hints;
	Atom wm_window_protocols[2];
	Atom wm_type_atoms[1];
	Atom net_wm_window_type;
	GdkDisplay *target_display = gdk_drawable_get_display (widget->window);
	
	static gboolean initialized = FALSE;

#ifndef MAG_WINDOW_OVERRIDE_REDIRECT	
	if (!initialized) {
		wm_window_protocols[0] = gdk_x11_get_xatom_by_name_for_display (target_display,
										"WM_DELETE_WINDOW");
		wm_window_protocols[1] = gdk_x11_get_xatom_by_name_for_display (target_display,
										"_NET_WM_PING");
		/* use DOCK until Metacity RFE for new window type goes in */
                wm_type_atoms[0] = gdk_x11_get_xatom_by_name_for_display (target_display,
									  "_NET_WM_WINDOW_TYPE_DOCK");
	}
  
	wm_hints.flags = InputHint;
	wm_hints.input = False;
	
	XSetWMHints (GDK_WINDOW_XDISPLAY (widget->window),
		     GDK_WINDOW_XWINDOW (widget->window), &wm_hints);
	
	XSetWMProtocols (GDK_WINDOW_XDISPLAY (widget->window),
			 GDK_WINDOW_XWINDOW (widget->window), wm_window_protocols, 2);

	net_wm_window_type = gdk_x11_get_xatom_by_name_for_display 
		(target_display, "_NET_WM_WINDOW_TYPE");

	if (net_wm_window_type && wm_type_atoms[0])
		XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
				 GDK_WINDOW_XWINDOW (widget->window),
				 net_wm_window_type,
				 XA_ATOM, 32, PropModeReplace,
				 (guchar *)wm_type_atoms,
				 1);
#else
#endif
	/* TODO: make sure this works/is reset if the DISPLAY 
	 * (as well as the SCREEN) changes.
	 */
}

void
gmag_gs_set_damage_handler_func (DamageHandlerFunc *func)
{
	damage_events_handler = func;
}

static gboolean
gmag_gs_events_handler (GIOChannel *source, GIOCondition condition,
			gpointer data)
{
	XEvent                   ev;
	Magnifier               *magnifier = (Magnifier *) data;
#ifdef HAVE_COMPOSITE
	gboolean                 calc_clip = FALSE;
#endif /* HAVE_COMPOSITE */

#ifdef HAVE_OVERLAY
	if (magnifier->priv->overlay)
		mag_window = GDK_WINDOW_XID (magnifier->priv->overlay);
#else
	if (magnifier->priv->w && magnifier->priv->w->window)
		mag_window = GDK_WINDOW_XID (magnifier->priv->w->window);
#endif /* HAVE_OVERLAY */

	do
	{
		XNextEvent(dpy, &ev);

#ifdef HAVE_COMPOSITE
		if (use_compositor) {
			switch (ev.type) {
			case CirculateNotify:
			case ConfigureNotify:
			case CreateNotify:
			case DestroyNotify:
			case MapNotify:
			case UnmapNotify:
			case ReparentNotify:
				calc_clip = gmag_compositor_events_handler (
					magnifier, &ev);
				break;
			}
		}
#endif /* HAVE_COMPOSITE */

		if (ev.type == damage_event_base + XDamageNotify) {
			damage_events_handler (&ev);
		}

		if (ev.type == fixes_event_base + XFixesCursorNotify) {
			gmag_cursor_events_handler (magnifier, &ev);
		}

	} while (XPending (dpy));

/* TODO: If HAVE_OVERLAY is not defined we raise the magnifier window to be
 * sure that it's above pop-up windows. It's only works if the magnifier is
 * started with the --override-redirect option. Probably we can verify, while
 * creating the magnifier window, if HAVE_OVERLAY isn't defined and the
 * COMPOSITE support is enable, so in this situtation always make the magnifier
 * window override-redirect.
 */
#ifndef HAVE_OVERLAY
	if (use_compositor && mag_window) {
		XRaiseWindow (dpy, mag_window);
	}
#endif /* HAVE_OVERLAY */

#ifdef HAVE_COMPOSITE
	if (use_compositor && calc_clip) {
		gmag_compositor_paint_screen ();
	}
#endif /* HAVE_COMPOSITE */

	XFlush (dpy);
	return TRUE;
}

/*
 * Verify if the graphical server support send notifications about Damage
 * regions, i.e., regions that changed it's content.
 */
static gboolean
gmag_gs_use_damage ()
{
	gint major, event, error;
	if (XQueryExtension (
		    dpy, "DAMAGE", &major, &damage_event_base, &error) &&
	    !g_getenv ("MAGNIFIER_IGNORE_DAMAGE")) {
		return TRUE;
	}
	return FALSE;
}


/*
 * Verify if the graphical server being used need to receive events to
 * composite. In the X11 we need this due the use of the Composite Extension,
 * since, when using this extension, the application is responsible in how the
 * windows being showed must be displayed to the user. So, by receiving these
 * events wen can keep track of the window position, size, stack-order and
 * others.
 */
gboolean
gmag_gs_use_compositor (Magnifier *magnifier)
{
	if (!gmag_gs_use_damage ()) {
		return FALSE;
	}
#ifdef HAVE_COMPOSITE
	gint major, event, error;

	/* if the source and target display are different we don't need the
	 * composite extension. This is the case when using the dummy driver or
	 * a dual head configuration */
	if (strcmp (magnifier->source_display_name,
		    magnifier->target_display_name) != 0)
		return FALSE;

	if (XQueryExtension (dpy, "Composite", &major, &event, &error) &&
	    !g_getenv ("MAGNIFIER_IGNORE_COMPOSITE"))
		return TRUE;
	return FALSE;
#else
	return FALSE;
#endif /* HAVE_COMPOSITE */
}

void
gmag_gs_client_init (Magnifier *magnifier)
{
	GIOChannel               *ioc;
	gint                      fd;

	XSetErrorHandler (gmag_gs_x_error_handler);

	if (dpy) {
		/* remove the old watch */
		if (dpy_gsource) 
			g_source_remove (dpy_gsource);
		XCloseDisplay (dpy);
	}

	if (magnifier) {
		/* we need our own connection here to keep from gumming up the
		 * works */
		dpy = XOpenDisplay (magnifier->source_display_name); 
		root_window = GDK_WINDOW_XWINDOW (magnifier->priv->root);
	} else {
		dpy = XOpenDisplay (NULL);
		root_window = RootWindow (dpy, DefaultScreen (dpy));
		g_message ("warning - using DefaultScreen for X connection.");
	}

#ifdef EVENTS_DEBUG
	XSynchronize (dpy, True);
#endif /* EVENTS_DEBUG */

	fd = ConnectionNumber (dpy);
	ioc = g_io_channel_unix_new (fd);
	dpy_gsource = g_io_add_watch (ioc,
				      G_IO_IN | G_IO_HUP | G_IO_PRI | G_IO_ERR,
				      gmag_gs_events_handler, magnifier);
	g_io_channel_unref (ioc); 

	use_damage = gmag_gs_use_damage ();
	use_compositor = gmag_gs_use_compositor (magnifier);

	fixes_event_base = gmag_cursor_init (magnifier, dpy);

	/* If the user doesn't want to use the composite extension we init the
	 * Damage connection here, since when we init composite, it uses other
	 * damage notify handler, more suitable to the composite task. I think
	 * that this leave the logic more simple.
	 */
	if (!use_compositor)
		gmag_damage_init (magnifier, dpy);

#ifdef HAVE_COMPOSITE
	if (use_compositor)
		gmag_compositor_init (magnifier, dpy);
#endif /* HAVE_COMPOSITE */

	XFlush (dpy);
}
