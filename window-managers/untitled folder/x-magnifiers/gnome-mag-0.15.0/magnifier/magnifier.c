/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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
#include "zoom-region.h"
#include "zoom-region-private.h"
#include "gmag-cursor.h"
#include "gmag-graphical-server.h"
#include "GNOME_Magnifier.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <libbonobo.h>
#include <login-helper/login-helper.h>

#include <gdk-pixbuf/gdk-pixbuf-io.h>
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* if you #define this, don't forget to set MAG_CLIENT_DEBUG env variable */
#define DEBUG_CLIENT_CALLS

#ifdef DEBUG_CLIENT_CALLS
static gboolean client_debug = FALSE;
#define DBG(a) if (client_debug) { (a); }
#else
#define DBG(a)
#endif

typedef struct
{
    LoginHelper parent;
    Magnifier *mag;
} MagLoginHelper;

typedef struct 
{
    LoginHelperClass parent_class;
} MagLoginHelperClass;

static GObjectClass *parent_class = NULL;

enum {
	MAGNIFIER_SOURCE_DISPLAY_PROP,
	MAGNIFIER_TARGET_DISPLAY_PROP,
	MAGNIFIER_SOURCE_SIZE_PROP,
	MAGNIFIER_TARGET_SIZE_PROP,
	MAGNIFIER_CURSOR_SET_PROP,
	MAGNIFIER_CURSOR_SIZE_PROP,
	MAGNIFIER_CURSOR_ZOOM_PROP,
	MAGNIFIER_CURSOR_COLOR_PROP,
	MAGNIFIER_CURSOR_HOTSPOT_PROP,
	MAGNIFIER_CURSOR_DEFAULT_SIZE_PROP,
	MAGNIFIER_CROSSWIRE_SIZE_PROP,
	MAGNIFIER_CROSSWIRE_LENGTH_PROP,
	MAGNIFIER_CROSSWIRE_CLIP_PROP,
	MAGNIFIER_CROSSWIRE_COLOR_PROP
} PropIdx;

typedef struct
{
	GNOME_Magnifier_RectBounds rectbounds;
	GNOME_Magnifier_RectBounds viewport;
	gboolean is_managed;
	gint scroll_policy;
	gfloat contrast;
	gfloat zx;
	gfloat zy;
	gint32 xalign;
	gint32 yalign;
	guint32 border_color;
	gint32 border_size;
	gchar *smoothing_type;
	gboolean inverse;

} MagnifierZoomRegionSaveProps;

#ifdef DEBUG_CLIENT_CALLS
gchar* mag_prop_names[MAGNIFIER_CROSSWIRE_COLOR_PROP + 1] = {
    "SOURCE_DISPLAY",
    "TARGET_DISPLAY",
    "SOURCE_SIZE",
    "TARGET_SIZE",
    "CURSOR_SET",
    "CURSOR_SIZE",
    "CURSOR_ZOOM",
    "CURSOR_COLOR",
    "CURSOR_HOTSPOT",
    "CURSOR_DEFAULT_SIZE",
    "CROSSWIRE_SIZE",
    "CROSSWIRE_LENGTH",
    "CROSSWIRE_CLIP",
    "CROSSWIRE_COLOR"
};
#endif

static Magnifier *_this_magnifier = NULL;
extern gint       fixes_event_base;

static void magnifier_init_cursor_set (Magnifier *magnifier, gchar *cursor_set);
static void magnifier_init_window (Magnifier *magnifier, GdkScreen *screen);
static gboolean magnifier_reset_struts_at_idle (gpointer data);
static void magnifier_init_window (Magnifier *magnifier, GdkScreen *screen);
static void magnifier_adjust_source_size (Magnifier *magnifier);
static gboolean _is_override_redirect = FALSE;

static Window*
mag_login_helper_get_raise_windows (LoginHelper *helper)
{
    Window *mainwin = NULL;
    MagLoginHelper *mag_helper = (MagLoginHelper *) helper;
    Magnifier *magnifier = MAGNIFIER (mag_helper->mag);

    if (magnifier && magnifier->priv && magnifier->priv->w)
    {
	mainwin = g_new0 (Window, 2);
	mainwin[0] = GDK_WINDOW_XWINDOW (magnifier->priv->w->window);
	mainwin[1] = None;
    }
    return mainwin;
}

static LoginHelperDeviceReqFlags
mag_login_helper_get_device_reqs (LoginHelper *helper)
{
    /* means "don't grab the xserver or core pointer", 
       and "we need to raise windows" */

    return LOGIN_HELPER_GUI_EVENTS | 
	LOGIN_HELPER_POST_WINDOWS | 
	LOGIN_HELPER_CORE_POINTER;
}

static gboolean
mag_login_helper_set_safe (LoginHelper *helper, gboolean ignored)
{
    return TRUE;
}

static void
mag_login_helper_class_init (MagLoginHelperClass *klass)
{
	LoginHelperClass *login_helper_class = LOGIN_HELPER_CLASS(klass);
	login_helper_class->get_raise_windows = mag_login_helper_get_raise_windows;
	login_helper_class->get_device_reqs = mag_login_helper_get_device_reqs;
	login_helper_class->set_safe = mag_login_helper_set_safe;
}

static void
mag_login_helper_init (MagLoginHelper *helper)
{
    helper->mag = NULL; /* we set this with mag_login_helper_set_magnifier */
}

static void
mag_login_helper_set_magnifier (MagLoginHelper *helper, Magnifier *mag)
{
    if (helper) 
	helper->mag = mag;
}

BONOBO_TYPE_FUNC (MagLoginHelper, 
		  LOGIN_HELPER_TYPE,
		  mag_login_helper)

static gboolean
can_open_display (gchar *display_name)
{
    Display *d;
    if ((d = XOpenDisplay (display_name)))
    {
	XCloseDisplay (d);
	return TRUE;
    }
    return FALSE;
}

static void
magnifier_warp_cursor_to_screen (Magnifier *magnifier)
{
	int x, y, unused_x, unused_y;
	unsigned int mask;
	Window root_return, child_return;

	if (magnifier->source_display)
	{
	    if (!XQueryPointer (GDK_DISPLAY_XDISPLAY (magnifier->source_display), 
				GDK_WINDOW_XWINDOW (magnifier->priv->root), 
				&root_return,
				&child_return,
				&x, &y,
				&unused_x, &unused_y,
				&mask))
	    {
		XWarpPointer (GDK_DISPLAY_XDISPLAY (magnifier->source_display),
			      None,
			      GDK_WINDOW_XWINDOW (magnifier->priv->root),
			      0, 0, 0, 0,
			      x, y);
		XSync (GDK_DISPLAY_XDISPLAY (magnifier->source_display), FALSE);
	    }
	}
}

void
magnifier_zoom_regions_update_pointer (Magnifier *magnifier)
{
	GList *list;

	g_assert (magnifier);

	list = magnifier->zoom_regions;
	while (list) {
		/* propagate the expose events to the zoom regions */
		GNOME_Magnifier_ZoomRegion zoom_region;
		CORBA_Environment ev;
		zoom_region = list->data;
		CORBA_exception_init (&ev);
		if (zoom_region)
			GNOME_Magnifier_ZoomRegion_updatePointer (
				CORBA_Object_duplicate (zoom_region, &ev), &ev);
		list = g_list_next (list);
	}
}

static void
magnifier_zoom_regions_mark_dirty (Magnifier *magnifier, GNOME_Magnifier_RectBounds rect_bounds)
{
	GList *list;

	g_assert (magnifier);

	list = magnifier->zoom_regions;
	while (list) 
	{
		/* propagate the expose events to the zoom regions */
		GNOME_Magnifier_ZoomRegion zoom_region;
		CORBA_Environment ev;
		zoom_region = list->data;
		CORBA_exception_init (&ev);
		if (zoom_region)
			GNOME_Magnifier_ZoomRegion_markDirty (CORBA_Object_duplicate (zoom_region, &ev),
							      &rect_bounds,
							      &ev);
		list = g_list_next (list);
	}
}

void
magnifier_set_cursor_from_pixbuf (Magnifier *magnifier,
				  GdkPixbuf *cursor_pixbuf)
{
	GdkPixmap *pixmap, *mask;
	gint width, height;
	GdkGC *gc;
	GdkDrawable *drawable = magnifier->priv->w->window;

	if (magnifier->priv->cursor) {
		g_object_unref (magnifier->priv->cursor);
		magnifier->priv->cursor = NULL;
	}
	if (drawable && cursor_pixbuf) {
		const gchar *xhot_string = NULL, *yhot_string = NULL;
		width = gdk_pixbuf_get_width (cursor_pixbuf);
		height = gdk_pixbuf_get_height (cursor_pixbuf);
		pixmap = gdk_pixmap_new (drawable, width, height, -1);
		gc = gdk_gc_new (pixmap);
		if (GDK_IS_DRAWABLE (pixmap))
			gdk_draw_pixbuf (pixmap, gc, cursor_pixbuf, 0, 0, 0, 0,
					 width, height, GDK_RGB_DITHER_NONE,
					 0, 0);
		else
			DBG (g_warning ("empty cursor pixmap created."));
		mask = gdk_pixmap_new (drawable, width, height, 1);
		gdk_pixbuf_render_threshold_alpha (cursor_pixbuf, mask,
						   0, 0, 0, 0, 
						   width, height,
						   200);
		g_object_unref (gc);
		magnifier->priv->cursor = pixmap;
		magnifier->priv->cursor_mask = mask;

		xhot_string = g_object_get_data (cursor_pixbuf, "x_hot");
		yhot_string = g_object_get_data (cursor_pixbuf, "y_hot");

		if (xhot_string)
			magnifier->cursor_hotspot.x = atoi (xhot_string);
		if (yhot_string)
			magnifier->cursor_hotspot.y = atoi (yhot_string);
		if (pixmap) {
			gdk_drawable_get_size (
				pixmap,
				&magnifier->priv->cursor_default_size_x,
				&magnifier->priv->cursor_default_size_y);
			magnifier->priv->cursor_hotspot_x =
				magnifier->cursor_hotspot.x;
			magnifier->priv->cursor_hotspot_y =
				magnifier->cursor_hotspot.y;
		}
	}
}


GdkPixbuf *
magnifier_get_pixbuf_for_name (Magnifier *magnifier, const gchar *cursor_name)
{
    GdkPixbuf *retval = NULL;
    if (magnifier->priv->cursorlist) 
	    retval = g_hash_table_lookup (magnifier->priv->cursorlist, cursor_name);
    if (retval) 
	    g_object_ref (retval);
    return retval;
}

void
magnifier_set_cursor_pixmap_by_name (Magnifier *magnifier,
				     const gchar *cursor_name, 
				     gboolean source_fallback)
{
	GdkPixbuf *pixbuf;
	/* search local table; if not found, use source screen's cursor if source_fallback is TRUE */
	if ((pixbuf = magnifier_get_pixbuf_for_name (magnifier, cursor_name)) == NULL) {
		if (source_fallback == TRUE)
		{
			pixbuf = gmag_cursor_get_source_pixbuf (magnifier);
		}
		else
		{
			pixbuf = magnifier_get_pixbuf_for_name (magnifier, "default");
		}
	}
	magnifier_set_cursor_from_pixbuf (magnifier, pixbuf);
	if (pixbuf) g_object_unref (pixbuf);
}

void
magnifier_notify_damage (Magnifier *magnifier, GdkRectangle *rect)
{
	GNOME_Magnifier_RectBounds rect_bounds;
	rect_bounds.x1 = rect->x;
	rect_bounds.y1 = rect->y;
	rect_bounds.x2 = rect->x + rect->width;
	rect_bounds.y2 = rect->y + rect->height;
#undef DEBUG_DAMAGE
#ifdef DEBUG_DAMAGE
	g_message ("damage");
	g_message ("dirty %d, %d to %d, %d", rect_bounds.x1, rect_bounds.y1,
		   rect_bounds.x2, rect_bounds.y2);
#endif
	magnifier_zoom_regions_mark_dirty (magnifier, rect_bounds);
}

static void
magnifier_set_extension_listeners (Magnifier *magnifier, GdkWindow *root)
{
	gmag_gs_client_init (magnifier);
	magnifier->source_initialized = TRUE;
}

static void
magnifier_size_allocate (GtkWidget *widget)
{
	gmag_gs_check_set_struts (_this_magnifier);
}

static void
magnifier_realize (GtkWidget *widget)
{
	gmag_gs_magnifier_realize (widget);
}

GdkWindow*
magnifier_get_root (Magnifier *magnifier)
{
    if (!magnifier->priv->root && magnifier->source_display) {
	magnifier->priv->root = gdk_screen_get_root_window (
	    gdk_display_get_screen (magnifier->source_display,
				    magnifier->source_screen_num));
    }
    return magnifier->priv->root;
}

static gint
magnifier_parse_display_name (Magnifier *magnifier, gchar *full_display_string,
			      gchar **display_name)
{
	gchar *screen_ptr;
	gchar **strings;
	
	if (display_name != NULL) {
		strings = g_strsplit (full_display_string, ":", 2);
		*display_name = strings [0];
		if (strings [1] != NULL)
			g_free (strings [1]);
	}

	screen_ptr = rindex (full_display_string, '.');
	if (screen_ptr != NULL) {
		return (gint) strtol (++screen_ptr, NULL, 10);
	}
	return 0;
}

static void
magnifier_get_display_rect_bounds (Magnifier *magnifier, GNOME_Magnifier_RectBounds *rect_bounds, gboolean is_target)
{
    if (is_target)
    {
	rect_bounds->x1 = 0;
	rect_bounds->x2 = gdk_screen_get_width (
	    gdk_display_get_screen (magnifier->target_display,
				    magnifier->target_screen_num));
	rect_bounds->y1 = 0;
	rect_bounds->y2 = gdk_screen_get_height (
	    gdk_display_get_screen (magnifier->target_display,
				    magnifier->target_screen_num));

    }
    else 
    {
	rect_bounds->x1 = 0;
	rect_bounds->x2 = gdk_screen_get_width (
	    gdk_display_get_screen (magnifier->source_display,
				    magnifier->source_screen_num));
	rect_bounds->y1 = 0;
	rect_bounds->y2 = gdk_screen_get_height (
	    gdk_display_get_screen (magnifier->source_display,
				    magnifier->source_screen_num));

    }
}

gboolean
magnifier_full_screen_capable (Magnifier *magnifier)
{
	if ((strcmp (magnifier->source_display_name,
		     magnifier->target_display_name) != 0) ||
		gmag_gs_use_compositor (magnifier))
		return TRUE;

	return FALSE;
}

static void
magnifier_adjust_source_size (Magnifier *magnifier)
{
	GNOME_Magnifier_RectBounds rect_bounds;	
	gdouble vfract_top, vfract_bottom, hfract_left, hfract_right;

	magnifier_get_display_rect_bounds (magnifier, &rect_bounds, FALSE);

	hfract_left = (double) (magnifier->target_bounds.x1) /
		(double) rect_bounds.x2;
	vfract_top = (double) (magnifier->target_bounds.y1) /
		(double) rect_bounds.y2;
	hfract_right = (double) (rect_bounds.x2 -
				 magnifier->target_bounds.x2) /
		(double) rect_bounds.x2;
	vfract_bottom = (double) (rect_bounds.y2 -
				  magnifier->target_bounds.y2) /
		(double) rect_bounds.y2;

	/* we make our 'source' rectangle the largest available subsection which we aren't occupying */
	if (magnifier_full_screen_capable (magnifier)) {
		magnifier->source_bounds = rect_bounds;
	} else if (MAX (hfract_left, hfract_right) >
		   MAX (vfract_top, vfract_bottom)) {  /* vertical split,
							* approximately */
		if (hfract_right > hfract_left) {
			magnifier->source_bounds.x1 =
				magnifier->target_bounds.x2;
			magnifier->source_bounds.x2 = rect_bounds.x2;
		} else  {
			magnifier->source_bounds.x1 = rect_bounds.x1;
			magnifier->source_bounds.x2 =
				magnifier->target_bounds.x1;
		}
		magnifier->source_bounds.y1 = rect_bounds.y1;
		magnifier->source_bounds.y2 = rect_bounds.y2;
	} else { /* more-or-less horizontally split */
		if (vfract_bottom > vfract_top) {
			magnifier->source_bounds.y1 =
				magnifier->target_bounds.y2;
			magnifier->source_bounds.y2 = rect_bounds.y2;
		} else {
			magnifier->source_bounds.y1 = rect_bounds.y1;
			magnifier->source_bounds.y2 =
				magnifier->target_bounds.y1;
		}
		magnifier->source_bounds.x1 = rect_bounds.x1;
		magnifier->source_bounds.x2 = rect_bounds.x2;
	}
	g_message ("set source bounds to %d,%d; %d,%d", 
		   magnifier->source_bounds.x1, magnifier->source_bounds.y1,
		   magnifier->source_bounds.x2, magnifier->source_bounds.y2);
}

static void
magnifier_unref_zoom_region (gpointer data, gpointer user_data)
{
/*	Magnifier *magnifier = user_data; NOT USED */
	CORBA_Environment ev;
	GNOME_Magnifier_ZoomRegion zoom_region = data;
	CORBA_exception_init (&ev);
	
	DBG(g_message ("unreffing zoom region"));

	GNOME_Magnifier_ZoomRegion_dispose (zoom_region, &ev);
	if (!BONOBO_EX (&ev))
	    Bonobo_Unknown_unref (zoom_region, &ev);
}

static GSList*
magnifier_zoom_regions_save (Magnifier *magnifier)
{
    GList *list;
    GSList *save_props = NULL;
    
    g_assert (magnifier);
    list = magnifier->zoom_regions;

    DBG(g_message ("saving %d regions", g_list_length (list)));

    while (list) 
    {
	GNOME_Magnifier_ZoomRegion zoom_region;
	CORBA_Environment ev;
	zoom_region = list->data;
	CORBA_exception_init (&ev);
	if (zoom_region)
	{
	    Bonobo_PropertyBag properties;
	    CORBA_any *value;
	    MagnifierZoomRegionSaveProps *zoomer_props = g_new0 (MagnifierZoomRegionSaveProps, 1);

	    zoomer_props->rectbounds = GNOME_Magnifier_ZoomRegion_getROI (zoom_region, &ev);
	    properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
	    value = bonobo_pbclient_get_value (properties, "viewport", TC_GNOME_Magnifier_RectBounds, &ev);
	    memcpy (&zoomer_props->viewport, value->_value, sizeof (GNOME_Magnifier_RectBounds));
	    CORBA_free (value);
	    zoomer_props->is_managed = bonobo_pbclient_get_boolean (properties, "is-managed", NULL);
	    zoomer_props->scroll_policy = bonobo_pbclient_get_short (properties, "smooth-scroll-policy", NULL);
	    zoomer_props->contrast = bonobo_pbclient_get_float (properties, "contrast", NULL);
	    zoomer_props->zx = bonobo_pbclient_get_float (properties, "mag-factor-x", NULL);
	    zoomer_props->zy = bonobo_pbclient_get_float (properties, "mag-factor-y", NULL);
	    zoomer_props->xalign = bonobo_pbclient_get_long (properties, "x-alignment", NULL);
	    zoomer_props->yalign = bonobo_pbclient_get_long (properties, "y-alignment", NULL);
	    zoomer_props->border_color = bonobo_pbclient_get_long (properties, "border-color", NULL); 
	    zoomer_props->border_size = bonobo_pbclient_get_long (properties, "border-size", NULL);
	    zoomer_props->smoothing_type = bonobo_pbclient_get_string (properties, "smoothing-type", NULL); 
	    zoomer_props->inverse = bonobo_pbclient_get_boolean (properties, "inverse-video", NULL); 

	    bonobo_object_release_unref (properties, &ev);
	    magnifier_unref_zoom_region ((gpointer) zoom_region, NULL);
	    save_props = g_slist_append (save_props, zoomer_props);
	}
	list = g_list_next (list);
    }   

    magnifier->zoom_regions = NULL;

    return save_props;
}

static void
magnifier_zoom_regions_restore (Magnifier *magnifier, GSList *region_params)
{
	GSList *list = region_params;

	while (list)
	{
		CORBA_Environment ev;
		MagnifierZoomRegionSaveProps *zoomer_props = list->data;
		GNOME_Magnifier_ZoomRegion new_region;
		Bonobo_PropertyBag new_properties;

		CORBA_exception_init (&ev);
		new_region = GNOME_Magnifier_Magnifier_createZoomRegion (BONOBO_OBJREF (magnifier), zoomer_props->zx, zoomer_props->zy, &zoomer_props->rectbounds, &zoomer_props->viewport, &ev);
		new_properties = GNOME_Magnifier_ZoomRegion_getProperties (new_region, &ev);
		bonobo_pbclient_set_boolean (new_properties, "is-managed", 
					     zoomer_props->is_managed, NULL);
		bonobo_pbclient_set_short (new_properties, "smooth-scroll-policy", 
					   zoomer_props->scroll_policy, NULL);
		bonobo_pbclient_set_float (new_properties, "contrast", 
					   zoomer_props->contrast, NULL);
/* NOT YET USED
		bonobo_pbclient_set_long (new_properties, "x-alignment", 
					     zoomer_props->xalign, NULL);
		bonobo_pbclient_set_long (new_properties, "y-alignment", 
					     zoomer_props->yalign, NULL);
*/
		bonobo_pbclient_set_long (new_properties, "border-color", 
					     zoomer_props->border_color, NULL);
		bonobo_pbclient_set_long (new_properties, "border-size", 
					     zoomer_props->border_size, NULL);
		bonobo_pbclient_set_string (new_properties, "smoothing-type", 
					     zoomer_props->smoothing_type, NULL);
		bonobo_pbclient_set_boolean (new_properties, "inverse-video", 
					     zoomer_props->inverse, NULL);
		GNOME_Magnifier_Magnifier_addZoomRegion (BONOBO_OBJREF (magnifier), new_region, &ev);
		g_free (zoomer_props->smoothing_type);
		g_free (zoomer_props);
		bonobo_object_release_unref (new_properties, &ev);
		list = g_slist_next (list);
	}
	g_slist_free (region_params);
}

static void
magnifier_init_display (Magnifier *magnifier, gchar *display_name, gboolean is_target)
{
    if (!can_open_display (display_name))
	return;

    if (is_target)
    {
	magnifier->target_screen_num =
	    magnifier_parse_display_name (magnifier,
					  display_name,
					  NULL);
	magnifier->target_display =
	    gdk_display_open (display_name);
	if (magnifier->target_display_name) g_free (magnifier->target_display_name);
	magnifier->target_display_name = g_strdup (display_name);
	magnifier->priv->root =
	    gdk_screen_get_root_window (
		gdk_display_get_screen (
		    magnifier->target_display,
		    magnifier->target_screen_num));
    }
    else 
    {
	magnifier->source_screen_num =
	    magnifier_parse_display_name (magnifier,
					  display_name,
					  NULL);
	magnifier->source_display =
	    gdk_display_open (display_name);
	if (magnifier->source_display)
	{
	    if (magnifier->source_display_name) g_free (magnifier->source_display_name);
	    magnifier->source_display_name = g_strdup (display_name);
	    magnifier->priv->root =
		gdk_screen_get_root_window (
		    gdk_display_get_screen (
			magnifier->source_display,
			magnifier->source_screen_num));
	}
    }
}

static void
magnifier_exit (GtkObject *object)
{
	gtk_main_quit ();
	exit (0);
}

#define GET_PIXEL(a,i,j,s,b) \
(*(guint32 *)(memcpy (b,(a) + ((j) * s + (i) * pixel_size_t), pixel_size_t)))

#define PUT_PIXEL(a,i,j,s,b) \
(memcpy (a + ((j) * s + (i) * pixel_size_t), &(b), pixel_size_t))

static void
magnifier_recolor_pixbuf (Magnifier *magnifier, GdkPixbuf *pixbuf)
{
	int rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	int i, j;
	int w = gdk_pixbuf_get_width (pixbuf);
	int h = gdk_pixbuf_get_height (pixbuf);
	guchar *pixels = gdk_pixbuf_get_pixels (pixbuf);
	guint32 pixval = 0, cursor_color = 0;
	size_t pixel_size_t = 3; /* FIXME: invalid assumption ? */

	cursor_color = ((magnifier->cursor_color & 0xFF0000) >> 16) +
		(magnifier->cursor_color & 0x00FF00) +
		((magnifier->cursor_color & 0x0000FF) << 16);
	for (j = 0; j < h; ++j) {
		for (i = 0; i < w; ++i) {
			pixval = GET_PIXEL (pixels, i, j, rowstride, &pixval);
			if ((pixval & 0x808080) == 0)
			{
				pixval = cursor_color;
				PUT_PIXEL (pixels, i, j, rowstride,
					   pixval);
			}
		}
	}
}

void
magnifier_transform_cursor (Magnifier *magnifier)
{
	if (magnifier->priv->cursor) /* don't do this if cursor isn't intialized yet */
	{
		int width, height;
		int size_x, size_y;
		GdkPixbuf *scaled_cursor_pixbuf;
		GdkPixbuf *scaled_mask_pixbuf;
		GdkPixbuf *scaled_mask_pixbuf_alpha;
		GdkPixbuf *cursor_pixbuf;
		GdkPixbuf *mask_pixbuf;
		GdkPixmap *cursor_pixmap = magnifier->priv->cursor;
		GdkPixmap *mask_pixmap = magnifier->priv->cursor_mask;
		GdkGC *cgc;
		GdkGC *mgc;

		if (magnifier->cursor_size_x)
		{
			size_x = magnifier->cursor_size_x;
			size_y = magnifier->cursor_size_y;
		}
		else
		{
			size_x = magnifier->priv->cursor_default_size_x * 
			       magnifier->cursor_scale_factor;
			size_y = magnifier->priv->cursor_default_size_y * 
			       magnifier->cursor_scale_factor;
		}
		gdk_drawable_get_size (magnifier->priv->cursor, &width, &height);
		if ((size_x == width) && (size_y == height) 
		    && (magnifier->cursor_color == 0xFF000000)) {
			return; /* nothing changes */
		}
		cgc = gdk_gc_new (cursor_pixmap);
		mgc = gdk_gc_new (mask_pixmap);
		cursor_pixbuf = gdk_pixbuf_get_from_drawable (NULL, cursor_pixmap,
							      NULL, 0, 0, 0, 0,
							      width, height);
		if (magnifier->cursor_color != 0xFF000000)
			magnifier_recolor_pixbuf (magnifier, cursor_pixbuf);
		mask_pixbuf = gdk_pixbuf_get_from_drawable (NULL,
							    mask_pixmap,
							    NULL, 0, 0, 0, 0,
							    width, height);
		scaled_cursor_pixbuf = gdk_pixbuf_scale_simple (
			cursor_pixbuf, size_x, size_y, GDK_INTERP_NEAREST);
		
		magnifier->cursor_hotspot.x = magnifier->priv->cursor_hotspot_x * size_x 
					    / magnifier->priv->cursor_default_size_x;
		magnifier->cursor_hotspot.y = magnifier->priv->cursor_hotspot_y * size_y 
					    / magnifier->priv->cursor_default_size_y;
					    
		scaled_mask_pixbuf = gdk_pixbuf_scale_simple (
			mask_pixbuf, size_x, size_y, GDK_INTERP_NEAREST);
		g_object_unref (cursor_pixbuf);
		g_object_unref (mask_pixbuf);
		g_object_unref (cursor_pixmap);
		g_object_unref (mask_pixmap);
		magnifier->priv->cursor = gdk_pixmap_new (
			magnifier->priv->w->window,
			size_x, size_y,
			-1);
		if (!GDK_IS_DRAWABLE (magnifier->priv->cursor)) 
		{
		    DBG (g_warning ("NULL magnifier cursor pixmap."));
		    return;
		}
		magnifier->priv->cursor_mask = gdk_pixmap_new (
			magnifier->priv->w->window,
			size_x, size_y,
			1);
		if (GDK_IS_DRAWABLE (magnifier->priv->cursor)) {
		    gdk_draw_pixbuf (magnifier->priv->cursor,
				     cgc,
				     scaled_cursor_pixbuf,
				     0, 0, 0, 0, size_x, size_y,
				     GDK_RGB_DITHER_NONE, 0, 0 );
		}
		else
		    DBG (g_warning ("cursor pixmap is non-drawable."));
		scaled_mask_pixbuf_alpha = gdk_pixbuf_add_alpha (
			scaled_mask_pixbuf, True, 0, 0, 0);
		gdk_pixbuf_render_threshold_alpha (scaled_mask_pixbuf_alpha,
						   magnifier->priv->cursor_mask,
						   0, 0, 0, 0, size_x, size_y,
						   0x80);
		g_object_unref (scaled_mask_pixbuf_alpha);
		g_object_unref (scaled_cursor_pixbuf);
		g_object_unref (scaled_mask_pixbuf);
		g_object_unref (mgc);
		g_object_unref (cgc);
	}	
}

static void
magnifier_init_cursor_set (Magnifier *magnifier, gchar *cursor_set)
{
	/*
	 * we check the cursor-set property string here,
	 * and create/apply the appropriate cursor settings
	 */
	magnifier->cursor_set = cursor_set;
	magnifier->priv->use_source_cursor = 
	    (!strcmp (cursor_set, "default") && 
	     (fixes_event_base != 0));
	if (magnifier->priv->use_source_cursor) return;

	if (!strcmp (magnifier->cursor_set, "none")) {
		magnifier->priv->cursor = NULL;
		return;
	}
	else 
	{
		GDir *cursor_dir;
		const gchar *filename;
		gchar *cursor_dirname;

		if (magnifier->priv->cursorlist)
		{
			g_hash_table_destroy (magnifier->priv->cursorlist);
		}
		magnifier->priv->cursorlist = g_hash_table_new_full (g_str_hash, g_str_equal,
								     g_free, g_object_unref);

		cursor_dirname = g_strconcat (CURSORSDIR, "/", magnifier->cursor_set, NULL);
		cursor_dir = g_dir_open (cursor_dirname, 0, NULL);
		/* assignment, not comparison, is intentional */
		while (cursor_dir && (filename = g_dir_read_name (cursor_dir)) != NULL) 
		{
			if (filename) 
			{
				gchar *path = g_strconcat (cursor_dirname, "/", filename, NULL);
				GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (path, NULL);
				if (pixbuf)
				{
					/* add this pixbuf and its name to our list */
					gchar **sv, *cname;
					cname = g_path_get_basename (filename);
					sv = g_strsplit (cname, ".", 2);
					g_hash_table_insert (magnifier->priv->cursorlist, 
							     g_strdup (sv[0]),
							     pixbuf);
					g_free (cname);
					g_strfreev (sv);
				}
				g_free (path);
			}
		} 
		g_free (cursor_dirname);
		if (cursor_dir) g_dir_close (cursor_dir);
	}
	/* don't fallover to source cursor here, we haven't initialized X yet */
	magnifier_set_cursor_pixmap_by_name (magnifier, "default", FALSE);
	magnifier_transform_cursor (magnifier);
}

static gboolean 
magnifier_reset_struts_at_idle (gpointer data)
{
	if (data)
	{
		Magnifier *magnifier = MAGNIFIER (data);
		if (magnifier->priv &&
		    GTK_WIDGET_REALIZED (magnifier->priv->w) && 
		    gmag_gs_check_set_struts (magnifier)) {
			return FALSE;
		}
	}
	return TRUE;
}

static void
magnifier_get_property (BonoboPropertyBag *bag,
			BonoboArg *arg,
			guint arg_id,
			CORBA_Environment *ev,
			gpointer user_data)
{
	Magnifier *magnifier = user_data;
	int csize = 0;

	DBG (fprintf (stderr, "Get property: \t%s\n", mag_prop_names[arg_id]));
	
	switch (arg_id) {
	case MAGNIFIER_SOURCE_SIZE_PROP:
		BONOBO_ARG_SET_GENERAL (arg, magnifier->source_bounds,
					TC_GNOME_Magnifier_RectBounds,
					GNOME_Magnifier_RectBounds, NULL);
		break;
	case MAGNIFIER_TARGET_SIZE_PROP:
	    	BONOBO_ARG_SET_GENERAL (arg, magnifier->target_bounds,
					TC_GNOME_Magnifier_RectBounds,
					GNOME_Magnifier_RectBounds, NULL);

		break;
	case MAGNIFIER_CURSOR_SET_PROP:
		BONOBO_ARG_SET_STRING (arg, magnifier->cursor_set);
		break;
	case MAGNIFIER_CURSOR_SIZE_PROP:
		BONOBO_ARG_SET_INT (arg, magnifier->cursor_size_x);
		BONOBO_ARG_SET_INT (arg, magnifier->cursor_size_y);
		break;
	case MAGNIFIER_CURSOR_ZOOM_PROP:
		BONOBO_ARG_SET_FLOAT (arg, magnifier->cursor_scale_factor);
		break;
	case MAGNIFIER_CURSOR_COLOR_PROP:
		BONOBO_ARG_SET_GENERAL (arg, magnifier->cursor_color,
					TC_CORBA_unsigned_long,
					CORBA_unsigned_long, NULL);
		break;
	case MAGNIFIER_CURSOR_HOTSPOT_PROP:
		BONOBO_ARG_SET_GENERAL (arg, magnifier->cursor_hotspot,
					TC_GNOME_Magnifier_Point,
					GNOME_Magnifier_Point, NULL);

		break;
	case MAGNIFIER_CURSOR_DEFAULT_SIZE_PROP:
		if (magnifier->priv->cursor)
			gdk_drawable_get_size (magnifier->priv->cursor,
					       &csize, &csize);
		BONOBO_ARG_SET_INT (arg, csize);
		break;
	case MAGNIFIER_CROSSWIRE_SIZE_PROP:
		BONOBO_ARG_SET_INT (arg, magnifier->crosswire_size);
		break;
	case MAGNIFIER_CROSSWIRE_LENGTH_PROP:
		BONOBO_ARG_SET_INT (arg, magnifier->crosswire_length);
		break;
	case MAGNIFIER_CROSSWIRE_CLIP_PROP:
		BONOBO_ARG_SET_BOOLEAN (arg, magnifier->crosswire_clip);
		break;
	case MAGNIFIER_CROSSWIRE_COLOR_PROP:
		BONOBO_ARG_SET_LONG (arg, magnifier->crosswire_color);
		break;
        case MAGNIFIER_SOURCE_DISPLAY_PROP:
		BONOBO_ARG_SET_STRING (arg, magnifier->source_display_name);
	        break;
	case MAGNIFIER_TARGET_DISPLAY_PROP:
		BONOBO_ARG_SET_STRING (arg, magnifier->target_display_name);
	        break;
	default:
		bonobo_exception_set (ev, ex_Bonobo_PropertyBag_NotFound);
	};
}

static void
magnifier_set_property (BonoboPropertyBag *bag,
			BonoboArg *arg,
			guint arg_id,
			CORBA_Environment *ev,
			gpointer user_data)
{
	Magnifier *magnifier = user_data;
	gchar *full_display_string;

	GNOME_Magnifier_RectBounds rect_bounds;
	rect_bounds.x1 = 0;
	rect_bounds.y1 = 0;
	rect_bounds.x2 = 4096;
	rect_bounds.y2 = 4096;

	switch (arg_id) {
	case MAGNIFIER_SOURCE_DISPLAY_PROP:
		full_display_string = BONOBO_ARG_GET_STRING (arg);
		if (can_open_display (full_display_string))
		{
		    GSList *zoom_region_params = NULL;
		    magnifier->source_screen_num =
			magnifier_parse_display_name (magnifier,
						      full_display_string,
						      NULL);
		    magnifier->source_display =
			gdk_display_open (full_display_string);
		    magnifier->source_display_name = g_strdup (full_display_string);
		    zoom_region_params = magnifier_zoom_regions_save (magnifier);
		    magnifier->priv->root =
			gdk_screen_get_root_window (
			    gdk_display_get_screen (
				magnifier->source_display,
				magnifier->source_screen_num));

		    /* remove the source_drawable, since in the new display the
		     * composite can be unavailable and the source_drawable is
		     * where the desktop screen is composed. If this is not
		     * freed it will be used even if the display doesn't use
		     * composite what will lead to wrong behavior and crashes.
		     * If composite is used in the new display this variable is
		     * re-created */
		    if (magnifier->priv->source_drawable) {
			    g_object_unref (magnifier->priv->source_drawable);
			    magnifier->priv->source_drawable = NULL;
		    }

		    /* attach listeners for DAMAGE, "dirty region", XFIXES
		     * cursor changes */
	            magnifier_set_extension_listeners (magnifier, magnifier_get_root (magnifier));
		    magnifier_get_display_rect_bounds (magnifier, &magnifier->source_bounds, FALSE);
		    magnifier_adjust_source_size (magnifier);
		    magnifier_zoom_regions_restore (magnifier, zoom_region_params);
		    magnifier_warp_cursor_to_screen (magnifier);
		    gmag_gs_check_set_struts (magnifier);
		    gmag_gs_reset_overlay_at_idle (magnifier);
		}
		DBG(fprintf (stderr, "Set source display: \t%s\n", full_display_string));
		break;
	case MAGNIFIER_TARGET_DISPLAY_PROP:
		full_display_string = BONOBO_ARG_GET_STRING (arg);
		if (can_open_display (full_display_string))
		{
		    magnifier->target_screen_num =
			magnifier_parse_display_name (magnifier,
						      full_display_string,
						      NULL);
		    magnifier->target_display =
			gdk_display_open (full_display_string);
		    magnifier->target_display_name = g_strdup (full_display_string);
		    if (GTK_IS_WINDOW (magnifier->priv->w)) 
		    {
#ifdef REPARENT_GTK_WINDOW_WORKS
			gtk_window_set_screen (GTK_WINDOW (magnifier->priv->w), 
					       gdk_display_get_screen (
						   magnifier->target_display,
						   magnifier->target_screen_num));
#else
			GSList *zoom_region_params = NULL;
			/* disconnect from the old window's destroy signal */
			g_object_disconnect (magnifier->priv->w,
				  "any_signal::realize", magnifier_realize, NULL,
				  "any_signal::size_allocate", magnifier_size_allocate, NULL,
				  "any_signal::destroy", magnifier_exit, NULL,
			          NULL);
			/* save the old zoom region state */
		        zoom_region_params = magnifier_zoom_regions_save (magnifier);
			/* destroy the old window */
			gtk_widget_destroy (magnifier->priv->w);
			/* and re-initialize... */
			magnifier_init_window (magnifier, gdk_display_get_screen (
						   magnifier->target_display,
						   magnifier->target_screen_num));
			/* restore the zoom regions in their new host magnifier window */
			magnifier_zoom_regions_restore (magnifier, zoom_region_params);
#endif
		    }
		    magnifier_get_display_rect_bounds (magnifier, &magnifier->source_bounds, FALSE);
		    magnifier_init_cursor_set (magnifier, magnifier->cursor_set); /* needed to reset pixmaps */
		    if (magnifier->priv->overlay)
			    gdk_window_move (magnifier->priv->overlay,
					     magnifier->target_bounds.x1,
					     magnifier->target_bounds.y1);
		    else
			    gtk_window_move (GTK_WINDOW (magnifier->priv->w),
					     magnifier->target_bounds.x1,
					     magnifier->target_bounds.y1);
		    
		    if ((magnifier->target_bounds.x2 - magnifier->target_bounds.x1 > 0) &&
			(magnifier->target_bounds.y2 - magnifier->target_bounds.y1) > 0)
		    {
			    if (magnifier->priv->overlay)
				    gdk_window_resize (
					    magnifier->priv->overlay,
					    magnifier->target_bounds.x2 -
					    magnifier->target_bounds.x1,
					    magnifier->target_bounds.y2 -
					    magnifier->target_bounds.y1);
			gtk_window_resize (GTK_WINDOW (magnifier->priv->w),
				       magnifier->target_bounds.x2 - magnifier->target_bounds.x1,
				       magnifier->target_bounds.y2 - magnifier->target_bounds.y1);
		    DBG(fprintf (stderr, "Set target size: \t%d,%d to %d,%d\n", 
			     magnifier->target_bounds.x1, magnifier->target_bounds.y1, magnifier->target_bounds.x2, magnifier->target_bounds.y2));
		    }
		    /* N. B. we don't reset the target bounds to the limits of the new display, because */
		    /* doing so would override the client-specified magnifier size */
		    /* magnifier_get_display_rect_bounds (magnifier, &magnifier->target_bounds, TRUE); */
		    gmag_gs_check_set_struts (magnifier);
		}
		DBG(fprintf (stderr, "Set target display: \t%s (screen %d)\n", 
			      full_display_string, magnifier->target_screen_num));
		break;
	case MAGNIFIER_SOURCE_SIZE_PROP:
	        magnifier->source_bounds = BONOBO_ARG_GET_GENERAL (arg,
								   TC_GNOME_Magnifier_RectBounds,
								   GNOME_Magnifier_RectBounds,
								   NULL);
		DBG (fprintf (stderr, "Set source size: \t%d,%d to %d,%d\n", 
			      magnifier->source_bounds.x1, magnifier->source_bounds.y1, magnifier->source_bounds.x2, magnifier->source_bounds.y2));
		break;
	case MAGNIFIER_TARGET_SIZE_PROP:
	        magnifier->target_bounds = BONOBO_ARG_GET_GENERAL (arg,
								   TC_GNOME_Magnifier_RectBounds,
								   GNOME_Magnifier_RectBounds,
				
				   NULL);
		if (magnifier->priv->overlay)
			gdk_window_move_resize (magnifier->priv->overlay,
						magnifier->target_bounds.x1,
						magnifier->target_bounds.y1,
						magnifier->target_bounds.x2 -
						magnifier->target_bounds.x1,
						magnifier->target_bounds.y2 -
						magnifier->target_bounds.y1);
		else
			gtk_window_move (GTK_WINDOW (magnifier->priv->w),
					 magnifier->target_bounds.x1,
					 magnifier->target_bounds.y1);
		
		gtk_window_resize (GTK_WINDOW (magnifier->priv->w),
				   magnifier->target_bounds.x2 - magnifier->target_bounds.x1,
				   magnifier->target_bounds.y2 - magnifier->target_bounds.y1);
		gmag_gs_check_set_struts (magnifier);
		DBG(fprintf (stderr, "Set target size: \t%d,%d to %d,%d\n", 
			      magnifier->target_bounds.x1, magnifier->target_bounds.y1, magnifier->target_bounds.x2, magnifier->target_bounds.y2));
		if (!strcmp (magnifier->target_display_name, magnifier->source_display_name) && 
		    (magnifier->target_screen_num == magnifier->source_screen_num)) 
		    magnifier_adjust_source_size (magnifier);
		break;
	case MAGNIFIER_CURSOR_SET_PROP:
		magnifier_init_cursor_set (magnifier, g_strdup (BONOBO_ARG_GET_STRING (arg)));
		DBG (fprintf (stderr, "Setting cursor set: \t%s\n", BONOBO_ARG_GET_STRING (arg)));
		break;
	case MAGNIFIER_CURSOR_SIZE_PROP:
		magnifier->cursor_size_x = BONOBO_ARG_GET_INT (arg);
		magnifier->cursor_size_y = BONOBO_ARG_GET_INT (arg);
		magnifier_transform_cursor (magnifier);
		DBG (fprintf (stderr, "Setting cursor size: \t%d\n", magnifier->cursor_size_x));
		break;
	case MAGNIFIER_CURSOR_ZOOM_PROP:
		magnifier->cursor_scale_factor = BONOBO_ARG_GET_FLOAT (arg);
		DBG (fprintf (stderr, "Setting cursor scale factor: \t%f\n", (float) magnifier->cursor_scale_factor));
		magnifier_transform_cursor (magnifier);
		break;
	case MAGNIFIER_CURSOR_COLOR_PROP:
		magnifier->cursor_color = BONOBO_ARG_GET_GENERAL (arg,
								  TC_CORBA_unsigned_long, 
								  CORBA_unsigned_long, 
								  NULL);
		magnifier_transform_cursor (magnifier);
		DBG (fprintf (stderr, "Setting cursor color: \t%u\n", (unsigned) magnifier->cursor_color));
		break;
	case MAGNIFIER_CURSOR_HOTSPOT_PROP:
		magnifier->cursor_hotspot = BONOBO_ARG_GET_GENERAL (arg,
								    TC_GNOME_Magnifier_Point,
								    GNOME_Magnifier_Point,
								    NULL);
		/* TODO: notify zoomers */
                /* FIXME: don't call init_cursor, it overwrites this property! */
		magnifier_transform_cursor (magnifier); 
		break;
	case MAGNIFIER_CURSOR_DEFAULT_SIZE_PROP:
		bonobo_exception_set (ev, ex_Bonobo_PropertyBag_ReadOnly);
		break;
	case MAGNIFIER_CROSSWIRE_SIZE_PROP:
		magnifier->crosswire_size = BONOBO_ARG_GET_INT (arg);
		DBG (fprintf (stderr, "Setting crosswire size: \t%d\n", magnifier->crosswire_size));
		magnifier_zoom_regions_update_pointer (magnifier);
		break;
	case MAGNIFIER_CROSSWIRE_LENGTH_PROP:
		magnifier->crosswire_length = BONOBO_ARG_GET_INT (arg);
		DBG (fprintf (stderr, "Setting crosswire length: \t%d\n", magnifier->crosswire_length));
		/* FIXME: The call below forces the repaint of all the screen,
		 * but I can't figure a better solution right now to the case
		 * where the crosswire lenght decrease */
		magnifier_zoom_regions_mark_dirty (magnifier, rect_bounds);
		break;
	case MAGNIFIER_CROSSWIRE_CLIP_PROP:
		magnifier->crosswire_clip = BONOBO_ARG_GET_BOOLEAN (arg);
		DBG (fprintf (stderr, "Setting crosswire clip: \t%s\n", magnifier->crosswire_clip ? "true" : "false"));
		magnifier_zoom_regions_update_pointer (magnifier);
		break;
	case MAGNIFIER_CROSSWIRE_COLOR_PROP:
		magnifier->crosswire_color = BONOBO_ARG_GET_LONG (arg);
		DBG (fprintf (stderr, "Setting crosswire size: \t%ld\n", (long) magnifier->crosswire_color));
		break;
	default:
		bonobo_exception_set (ev, ex_Bonobo_PropertyBag_NotFound);
		break;
	};
}

static void
magnifier_do_dispose (Magnifier *magnifier)
{
	/* FIXME: this is dead ropey code structuring */
	bonobo_activation_active_server_unregister (
		MAGNIFIER_OAFIID, BONOBO_OBJREF (magnifier));

	if (magnifier->zoom_regions)
		g_list_free (magnifier->zoom_regions);
	magnifier->zoom_regions = NULL;
	
	bonobo_main_quit ();
}

static void
magnifier_gobject_dispose (GObject *object)
{
	magnifier_do_dispose (MAGNIFIER (object));

	BONOBO_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

static
CORBA_boolean
impl_magnifier_support_colorblind_filters (Magnifier *magnifier)
{
#ifdef HAVE_COLORBLIND
	return CORBA_TRUE;
#else
	return CORBA_FALSE;
#endif
}

static
CORBA_boolean
impl_magnifier_full_screen_capable (PortableServer_Servant servant,
				    CORBA_Environment * ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));

	if ((strcmp (magnifier->source_display_name,
		     magnifier->target_display_name) != 0) ||
		gmag_gs_use_compositor (magnifier))
		return CORBA_TRUE;

	return CORBA_FALSE;
}

static void
impl_magnifier_set_source_display (PortableServer_Servant servant,
				   const CORBA_char *display,
				   CORBA_Environment *ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	BonoboArg *arg = bonobo_arg_new (BONOBO_ARG_STRING);
	gchar *full_display_string;

	full_display_string = g_strdup (display);
	if (strcmp (full_display_string, "") == 0)
		full_display_string = (gchar *) g_getenv ("DISPLAY");

	BONOBO_ARG_SET_STRING (arg, full_display_string);
	
	DBG (fprintf (stderr, "Set source display: \t%s\n",
		      full_display_string));

	if (strcmp (full_display_string, magnifier->source_display_name)) {
	    magnifier_set_property (magnifier->property_bag,
				    arg,
				    MAGNIFIER_SOURCE_DISPLAY_PROP,
				    ev,
				    magnifier);
	}
	else
	{
	    DBG (fprintf (stderr, "Attempt to set source to same value as previous: %s\n",
			  full_display_string));
	}
	bonobo_arg_release (arg);
}

static void
impl_magnifier_set_target_display (PortableServer_Servant servant,
				   const CORBA_char *display,
				   CORBA_Environment *ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	BonoboArg *arg = bonobo_arg_new (BONOBO_ARG_STRING);
	gchar *full_display_string;

	full_display_string = g_strdup (display);
	if (strcmp (full_display_string, "") == 0)
		full_display_string = (gchar *) g_getenv ("DISPLAY");

	BONOBO_ARG_SET_STRING (arg, full_display_string);
	
	DBG (fprintf (stderr, "Set target display: \t%s\n",
		      full_display_string));

	if (strcmp (full_display_string, magnifier->target_display_name)) 
	{
	    magnifier_set_property (magnifier->property_bag,
				    arg,
				    MAGNIFIER_TARGET_DISPLAY_PROP,
				    ev,
				    magnifier);
	}
	else
	{
	    DBG (fprintf (stderr, "Attempt to set target to same value as previous: %s\n",
			  full_display_string));
	}
	bonobo_arg_release (arg);
}

static 
CORBA_string
impl_magnifier_get_source_display (PortableServer_Servant servant,
				   CORBA_Environment *ev)
{
        Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	DBG (fprintf (stderr, "Get source display: \t%s\n", magnifier->source_display_name));

	return CORBA_string_dup (magnifier->source_display_name ? magnifier->source_display_name : "");
}

static 
CORBA_string
impl_magnifier_get_target_display (PortableServer_Servant servant,
				   CORBA_Environment *ev)
{
        Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	DBG (fprintf (stderr, "Get target display: \t%s\n", 
		      magnifier->target_display_name));

	return CORBA_string_dup (magnifier->target_display_name ? magnifier->target_display_name : "");
}

static GNOME_Magnifier_ZoomRegion
impl_magnifier_create_zoom_region (PortableServer_Servant servant,
				   const CORBA_float zx,
				   const CORBA_float zy,
				   const GNOME_Magnifier_RectBounds *roi,
				   const GNOME_Magnifier_RectBounds *viewport,
				   CORBA_Environment *ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	CORBA_any viewport_any;
	ZoomRegion *zoom_region = zoom_region_new ();
	Bonobo_PropertyBag properties;
	GNOME_Magnifier_ZoomRegion retval;

	DBG (fprintf (stderr, "Create zoom region: \tzoom %f,%f, viewport %d,%d to %d,%d\n", (float) zx, (float) zy, viewport->x1, viewport->y1, viewport->x2, viewport->y2));

	/* FIXME:
	 * shouldn't do this here, since it causes the region to get
	 * mapped onto the parent, if if it's not explicitly added!
	 */
	DBG(g_message ("creating zoom region with parent %p", magnifier));
	zoom_region->priv->parent = magnifier;

	retval = BONOBO_OBJREF (zoom_region);

	/* XXX: should check ev after each call, below */
	CORBA_exception_init (ev);
	GNOME_Magnifier_ZoomRegion_setMagFactor (retval, zx, zy, ev);

	if (ev->_major != CORBA_NO_EXCEPTION)
		fprintf (stderr, "EXCEPTION setMagFactor\n");

	CORBA_exception_init (ev);
	properties = GNOME_Magnifier_ZoomRegion_getProperties (retval, ev);
	if (ev->_major != CORBA_NO_EXCEPTION)
		fprintf (stderr, "EXCEPTION getProperties\n");

	viewport_any._type = TC_GNOME_Magnifier_RectBounds;
	viewport_any._value = (gpointer) viewport;
	Bonobo_PropertyBag_setValue (
		properties, "viewport", &viewport_any, ev);

	GNOME_Magnifier_ZoomRegion_setROI (retval, roi, ev);
	if (ev->_major != CORBA_NO_EXCEPTION)
		fprintf (stderr, "EXCEPTION setROI\n");

	CORBA_exception_init (ev);

	gtk_widget_set_size_request (magnifier->priv->canvas,
			   viewport->x2 - viewport->x1,
			   viewport->y2 - viewport->y1);
	gtk_widget_show (magnifier->priv->canvas);
	gtk_widget_show (magnifier->priv->w);

	bonobo_object_release_unref (properties, ev);
	
	return CORBA_Object_duplicate (retval, ev);
}

static
CORBA_boolean
impl_magnifier_add_zoom_region (PortableServer_Servant servant,
				const GNOME_Magnifier_ZoomRegion region,
				CORBA_Environment * ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));

	if (!magnifier->source_initialized) 
	{
		magnifier_set_extension_listeners (magnifier, magnifier_get_root (magnifier));
	}

	/* FIXME: this needs proper lifecycle management */
	magnifier->zoom_regions = g_list_append (magnifier->zoom_regions, region);
	gmag_gs_check_set_struts (magnifier);

	return CORBA_TRUE;
}

static Bonobo_PropertyBag
impl_magnifier_get_properties (PortableServer_Servant servant,
			       CORBA_Environment *ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	return bonobo_object_dup_ref (
		BONOBO_OBJREF (magnifier->property_bag), ev);
}

GNOME_Magnifier_ZoomRegionList *
impl_magnifier_get_zoom_regions (PortableServer_Servant servant,
				 CORBA_Environment * ev)
{
	Magnifier *magnifier =
		MAGNIFIER (bonobo_object_from_servant (servant));

	GNOME_Magnifier_ZoomRegionList *list;
	CORBA_Object objref;
	int i, len;

	len = g_list_length (magnifier->zoom_regions);
	list = GNOME_Magnifier_ZoomRegionList__alloc ();
	list->_length = len;
	list->_buffer =
		GNOME_Magnifier_ZoomRegionList_allocbuf (list->_length);
	for (i = 0; i < len; ++i) {
		objref = g_list_nth_data (magnifier->zoom_regions, i);
		list->_buffer [i] =
			CORBA_Object_duplicate (objref, ev);
	}
	CORBA_sequence_set_release (list, CORBA_TRUE);

	DBG (fprintf (stderr, "Get zoom regions: \t%d\n", len));
	
	return list; 
}

static void
impl_magnifier_clear_all_zoom_regions (PortableServer_Servant servant,
				       CORBA_Environment * ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	fprintf (stderr, "Clear all zoom regions.\n");

	g_list_foreach (magnifier->zoom_regions,
			magnifier_unref_zoom_region, magnifier);
	g_list_free (magnifier->zoom_regions);
	magnifier->zoom_regions = NULL;
}

static void
impl_magnifier_dispose (PortableServer_Servant servant,
			CORBA_Environment *ev)
{
	magnifier_do_dispose (
		MAGNIFIER (bonobo_object_from_servant (servant)));
}

static void
magnifier_class_init (MagnifierClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_GNOME_Magnifier_Magnifier__epv *epv = &klass->epv;
	parent_class = g_type_class_peek (BONOBO_TYPE_OBJECT); /* needed by BONOBO_CALL_PARENT! */

	object_class->dispose = magnifier_gobject_dispose;

	epv->fullScreenCapable = impl_magnifier_full_screen_capable;
	epv->supportColorblindFilters =
		impl_magnifier_support_colorblind_filters;
        epv->_set_SourceDisplay = impl_magnifier_set_source_display;
	epv->_set_TargetDisplay = impl_magnifier_set_target_display;
        epv->_get_SourceDisplay = impl_magnifier_get_source_display;
	epv->_get_TargetDisplay = impl_magnifier_get_target_display;
	epv->getProperties = impl_magnifier_get_properties;
	epv->getZoomRegions = impl_magnifier_get_zoom_regions;
	epv->createZoomRegion = impl_magnifier_create_zoom_region;
	epv->addZoomRegion = impl_magnifier_add_zoom_region;
	epv->clearAllZoomRegions = impl_magnifier_clear_all_zoom_regions;
	epv->dispose = impl_magnifier_dispose;
}

static void
magnifier_properties_init (Magnifier *magnifier)
{
	BonoboArg *def;
	GNOME_Magnifier_RectBounds rect_bounds;
	gchar *display_env;

	magnifier->property_bag =
		bonobo_property_bag_new_closure (
			g_cclosure_new_object (
				G_CALLBACK (magnifier_get_property),
				G_OBJECT (magnifier)),
			g_cclosure_new_object (
				G_CALLBACK (magnifier_set_property),
				G_OBJECT (magnifier)));

	/* Aggregate so magnifier implements Bonobo_PropertyBag */
	bonobo_object_add_interface (BONOBO_OBJECT (magnifier),
				     BONOBO_OBJECT (magnifier->property_bag));

	def = bonobo_arg_new (BONOBO_ARG_STRING);
	display_env = getenv ("DISPLAY");
	BONOBO_ARG_SET_STRING (def, display_env);

	bonobo_property_bag_add (magnifier->property_bag,
				 "source-display-screen",
				 MAGNIFIER_SOURCE_DISPLAY_PROP,
				 BONOBO_ARG_STRING,
				 def,
				 "source display screen",
				 Bonobo_PROPERTY_WRITEABLE);

	bonobo_property_bag_add (magnifier->property_bag,
				 "target-display-screen",
				 MAGNIFIER_TARGET_DISPLAY_PROP,
				 BONOBO_ARG_STRING,
				 def,
				 "target display screen",
				 Bonobo_PROPERTY_WRITEABLE);

	bonobo_arg_release (def);

	magnifier_init_display (magnifier, display_env, TRUE);
	magnifier_init_display (magnifier, display_env, FALSE);

	magnifier_get_display_rect_bounds (magnifier, &rect_bounds, FALSE);
	def = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds, &rect_bounds);
        
	bonobo_property_bag_add (magnifier->property_bag,
				 "source-display-bounds",
				 MAGNIFIER_SOURCE_SIZE_PROP,
				 TC_GNOME_Magnifier_RectBounds,
				 def,
				 "source display bounds/size",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	bonobo_arg_release (def);
	
	magnifier_get_display_rect_bounds (magnifier, &rect_bounds, TRUE);
	def = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds, &rect_bounds);

	bonobo_property_bag_add (magnifier->property_bag,
				 "target-display-bounds",
				 MAGNIFIER_TARGET_SIZE_PROP,
				 TC_GNOME_Magnifier_RectBounds,
				 def,
				 "target display bounds/size",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	bonobo_arg_release (def);

	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-set",
				 MAGNIFIER_CURSOR_SET_PROP,
				 BONOBO_ARG_STRING,
				 NULL,
				 "name of cursor set",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);

	def = bonobo_arg_new (BONOBO_ARG_INT);
	BONOBO_ARG_SET_INT (def, 64);
	
	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-size",
				 MAGNIFIER_CURSOR_SIZE_PROP,
				 BONOBO_ARG_INT,
				 def,
				 "cursor size, in pixels",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	bonobo_arg_release (def);
	
	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-scale-factor",
				 MAGNIFIER_CURSOR_ZOOM_PROP,
				 BONOBO_ARG_FLOAT,
				 NULL,
				 "scale factor for cursors (overrides size)",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	
	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-color",
				 MAGNIFIER_CURSOR_COLOR_PROP,
				 TC_CORBA_unsigned_long,
				 NULL,
				 "foreground color for 1-bit cursors, as ARGB",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);	

	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-hotspot",
				 MAGNIFIER_CURSOR_HOTSPOT_PROP,
				 TC_GNOME_Magnifier_Point,
				 NULL,
				 "hotspot relative to cursor's upper-left-corner, at default resolition",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	
	bonobo_property_bag_add (magnifier->property_bag,
				 "cursor-default-size",
				 MAGNIFIER_CURSOR_DEFAULT_SIZE_PROP,
				 BONOBO_ARG_INT,
				 NULL,
				 "default size of current cursor set",
				 Bonobo_PROPERTY_READABLE);

	bonobo_property_bag_add (magnifier->property_bag,
				 "crosswire-size",
				 MAGNIFIER_CROSSWIRE_SIZE_PROP,
				 BONOBO_ARG_INT,
				 NULL,
				 "thickness of crosswire cursor, in target pixels",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);

	bonobo_property_bag_add (magnifier->property_bag,
				 "crosswire-length",
				 MAGNIFIER_CROSSWIRE_LENGTH_PROP,
				 BONOBO_ARG_INT,
				 NULL,
				 "length of crosswire cursor, in target pixels",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
	
	bonobo_property_bag_add (magnifier->property_bag,
				 "crosswire-color",
				 MAGNIFIER_CROSSWIRE_COLOR_PROP,
				 BONOBO_ARG_LONG,
				 NULL,
				 "color of crosswire, as A-RGB; note that alpha is required. (use 0 for XOR wire)",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);

	bonobo_property_bag_add (magnifier->property_bag,
				 "crosswire-clip",
				 MAGNIFIER_CROSSWIRE_CLIP_PROP,
				 BONOBO_ARG_BOOLEAN,
				 NULL,
				 "whether to inset the cursor over the crosswire or not",
				 Bonobo_PROPERTY_READABLE |
				 Bonobo_PROPERTY_WRITEABLE);
}

static void
magnifier_init_window (Magnifier *magnifier, GdkScreen *screen)
{
	GtkWindowType mag_win_type = GTK_WINDOW_TOPLEVEL;
	if (_is_override_redirect) mag_win_type = GTK_WINDOW_POPUP;

	magnifier->priv->w =
		g_object_connect (gtk_widget_new (gtk_window_get_type (),
						  "user_data", NULL,
						  "can_focus", FALSE,
						  "type", mag_win_type,  
						  "title", "magnifier",
						  "allow_grow", TRUE,
						  "allow_shrink", TRUE,
						  "border_width", 0,
						  NULL),
				  "signal::realize", magnifier_realize, NULL,
				  "signal::size_allocate", magnifier_size_allocate, NULL,
				  "signal::destroy", magnifier_exit, NULL,
				  NULL);
	gtk_window_set_screen (GTK_WINDOW (magnifier->priv->w), screen);
	magnifier->priv->canvas = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (magnifier->priv->w),
			   magnifier->priv->canvas);
	magnifier->priv->root = NULL;
}

static void
magnifier_init (Magnifier *magnifier)
{
	magnifier->priv = g_new0 (MagnifierPrivate, 1);
	magnifier_properties_init (magnifier);
	magnifier->zoom_regions = NULL;
	magnifier->source_screen_num = 0;
	magnifier->target_screen_num = 0;
	magnifier->source_display_name = g_strdup (":0.0");
	magnifier->target_display_name = g_strdup (":0.0");
	magnifier->cursor_size_x = 0;
	magnifier->cursor_size_y = 0;
	magnifier->cursor_scale_factor = 1.0F;
	magnifier->cursor_color = 0xFF000000;
	magnifier->crosswire_size = 1;
	magnifier->crosswire_length = 0;
	magnifier->crosswire_color = 0;
	magnifier->crosswire_clip = FALSE;
	magnifier->cursor_hotspot.x = 0;
	magnifier->cursor_hotspot.y = 0;
	magnifier->target_bounds.x1 = 0;
	magnifier->target_bounds.y1 = 0;
	magnifier->target_bounds.x2 = 0;
	magnifier->target_bounds.y2 = 0;
	magnifier->priv->cursor = NULL;
	magnifier->priv->w = NULL;
	magnifier->priv->use_source_cursor = TRUE;
	magnifier->priv->cursorlist = NULL;
	magnifier->priv->source_drawable = NULL;
	magnifier->priv->overlay = NULL;
	magnifier_init_window (magnifier, 
			       gdk_display_get_screen (magnifier->target_display, 
						       magnifier->target_screen_num));
	magnifier_init_cursor_set (magnifier, "default");

	mag_timing.process = g_timer_new ();
	mag_timing.frame = g_timer_new ();
	mag_timing.scale = g_timer_new ();
	mag_timing.idle = g_timer_new ();
#ifdef DEBUG_CLIENT_CALLS
	client_debug = (g_getenv ("MAG_CLIENT_DEBUG") != NULL);
#endif
}

GdkDrawable *
magnifier_get_cursor (Magnifier *magnifier)
{
        if (magnifier->priv->cursor == NULL) {
	    if ((fixes_event_base == 0) && 
		strcmp (magnifier->cursor_set, "none")) 
	    {
		GdkPixbuf *pixbuf;
		gchar *default_cursor_filename = 
		    g_strconcat (CURSORSDIR, "/", "default-cursor.xpm", NULL);
		pixbuf = gdk_pixbuf_new_from_file (default_cursor_filename, NULL);
		if (pixbuf) 
		{
		    magnifier_set_cursor_from_pixbuf (magnifier, pixbuf);
		    g_object_unref (pixbuf);
		    magnifier_transform_cursor (magnifier);
		}
		g_free (default_cursor_filename);
	    } else {
		GdkPixbuf *cursor_pixbuf = gmag_cursor_get_source_pixbuf (
			magnifier);
		magnifier_set_cursor_from_pixbuf (magnifier, cursor_pixbuf);
		if (cursor_pixbuf) g_object_unref (cursor_pixbuf);
		magnifier_transform_cursor (magnifier);
	    }
        }
	return magnifier->priv->cursor;
}

Magnifier *
magnifier_new (gboolean override_redirect)
{
	Magnifier *mag;
	MagLoginHelper *helper;
	int ret;

	_is_override_redirect = override_redirect;

	mag = g_object_new (magnifier_get_type(), NULL);

	_this_magnifier = mag; /* FIXME what about multiple instances? */

	helper = g_object_new (mag_login_helper_get_type (), NULL);
	mag_login_helper_set_magnifier (helper, mag);

	bonobo_object_add_interface (bonobo_object (mag), 
				     BONOBO_OBJECT (helper));
	
	ret = bonobo_activation_active_server_register (
		MAGNIFIER_OAFIID, BONOBO_OBJREF (mag));
	if (ret != Bonobo_ACTIVATION_REG_SUCCESS) {
	    if ( ret == Bonobo_ACTIVATION_REG_ALREADY_ACTIVE)
	    {
		printf("An instance of magnifier is already active. Exiting Program.\n");
		exit(0);
	    } else
		g_error ("Error registering magnifier server.\n");
	}

	g_timeout_add (500, magnifier_reset_struts_at_idle, mag);
	g_timeout_add (500, gmag_gs_reset_overlay_at_idle, mag);

	return mag;
}

BONOBO_TYPE_FUNC_FULL (Magnifier, 
		       GNOME_Magnifier_Magnifier,
		       BONOBO_TYPE_OBJECT,
		       magnifier)

