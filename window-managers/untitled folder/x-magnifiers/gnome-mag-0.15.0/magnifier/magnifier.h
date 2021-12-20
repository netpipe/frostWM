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

#ifndef MAGNIFIER_H_
#define MAGNIFIER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "GNOME_Magnifier.h"

#include <glib.h>

#include <gdk/gdk.h>

#include <bonobo/bonobo-object.h>
#include <bonobo/bonobo-property-bag.h>

#include <login-helper/login-helper.h>

#define MAGNIFIER_TYPE         (magnifier_get_type ())
#define MAGNIFIER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MAGNIFIER_TYPE, Magnifier))
#define MAGNIFIER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), MAGNIFIER_TYPE, MagnifierClass))
#define IS_MAGNIFIER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), MAGNIFIER_TYPE))
#define IS_MAGNIFIER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAGNIFIER_TYPE))

#define MAGNIFIER_OAFIID "OAFIID:GNOME_Magnifier_Magnifier:0.9"

typedef struct _MagnifierPrivate MagnifierPrivate;

typedef struct {
        BonoboObject parent;
	BonoboPropertyBag *property_bag;
	GdkDisplay *source_display;
	GdkDisplay *target_display;
	gchar *source_display_name;
	gchar *target_display_name;
	int source_screen_num;
	int target_screen_num;
	GList *zoom_regions;
        gboolean source_initialized;
	gint crosswire_size;
	gint crosswire_length;
	guint32 crosswire_color;
	gboolean crosswire_clip;
	gchar *cursor_set;
	gint cursor_size_x;
	gint cursor_size_y;
	guint32 cursor_color;
	float cursor_scale_factor;
	GNOME_Magnifier_RectBounds source_bounds;
	GNOME_Magnifier_RectBounds target_bounds;
	GNOME_Magnifier_Point cursor_hotspot;
	MagnifierPrivate *priv;
} Magnifier;

typedef struct {
        BonoboObjectClass parent_class;
        POA_GNOME_Magnifier_Magnifier__epv epv;
} MagnifierClass;

/*
 * When running the magnifier in timing test mode (--timing-pan-rate and/or
 * --timing-iterations), then the following output is displayed:
 *
 * Pan Rate           = 200
 * Frames Processed   = 1225
 * Width/Height/Depth = 640/1024/8
 * Zoom Factor (x/y)  = 3.000000/3.000000
 * Update Duration    = (avg. 0.010232) (max. 0.002685) (tot. 1.084589) seconds
 *   Update Pixels    = (avg. 3046) pixels/frame
 *   Update Rate      = (avg. 97.732909) (max. 372.439485) updates/second
 *   Net Update Rate  = (avg. 0.297724) (max. 0.457741) Mpex/second
 * Pan Latency        = (avg. 0.008229) (max. 0.562326) seconds
 * Tot Frame Duration = (avg. 0.008363) (max. 0.562485) (tot. 10.235981)seconds
 * Frame Rate         = (avg. 119.578189) (max. 4854.369141) frames/second
 * Scroll Delta (x)   = (avg. 0.000000) (tot. 0) lines
 * Scroll Delta (y)   = (avg. 5.019608) (tot. 6144) lines
 * Scroll Rate (x)    = (avg. 0.000000) lines/second
 * Scroll Rate (y)    = (avg. 600.235657) lines/second
 * Net Render Rate    = (avg. 8.685322) (max. 3181.359360) Mpex/second
 *
 * Pan Rate             - Displays the pan rate specified on the command line.
 *                        Only displayed if --timing-pan-rate is used.
 * Frames Processed     - Number of frames displayed
 * Width/Height/Depth   - The width/height/depth of magnifier display.
 * Zoom Factor (x/y)    - The zoom factor used.
 * Update Duration      - Average and total time spent updating new sections of
 *                        the screen.
 *   Update Pixels      - Number of pixels updated
 *   Update Rate        - Number of updates possible per second.
 *   Net Update Rate    - Number of Megapixels updated per second.
 * Pan Latency          - Time spent from the beginning of the setROI request
 *                        until GTK+ has finished processing the update (in
 *                        GDK's gdk_window_update_idle idle handler.
 * Total Frame Duration - Average and total time processing frames.
 * Frame Rate           - Number of frames per second.
 * Scroll Delta         - Average and total number of lines scrolled.
 * Scroll Rate          - Number of lines scrolled per second.  Note that this
 *                        value will be equal to the --timing-pan-rate value
 *                        times the y zoom factor.
 * Net Render Rate      - Number of Megapixels processed per second.
 *
 * When running with the --timing-output argument, similar information is
 * displayed for each frame plus timing information for the frame.  Update
 * information is not displayed for frames where updates do not occur.  Pan 
 * Rate, Frames Processed, and Width/Height/Depth is not displayed for each
 * frame, only when displaying summary information.
 */

typedef struct timing {
	GTimer *process;
	GTimer *frame;
	GTimer *scale;
	GTimer *idle;
	gdouble scale_val;
	gdouble idle_val;
	gdouble frame_val;
	int   dx;
	int   dy;
	gdouble scale_total;
	gdouble idle_total;
	gdouble frame_total;
	int   dx_total;
	int   dy_total;
	long  update_pixels_total;
	long  num_scale_samples;
	long  num_idle_samples;
	long  num_frame_samples;
	long  num_line_samples;
	gdouble last_frame_val;
	int   last_dy;
} TimingStruct;

TimingStruct mag_timing;

GdkDrawable *magnifier_get_cursor    (Magnifier *magnifier);
GType        magnifier_get_type      (void);
GdkWindow   *magnifier_get_root      (Magnifier *magnifier);
Magnifier   *magnifier_new           (gboolean override_redirect);
void         magnifier_notify_damage (Magnifier *magnifier,
				      GdkRectangle *rect);
void         magnifier_set_cursor_from_pixbuf (Magnifier *magnifier,
					       GdkPixbuf *pointer_pixbuf);
void         magnifier_set_cursor_pixmap_by_name (Magnifier *magnifier,
						  const gchar *cursor_name, 
						  gboolean source_fallback);
void         magnifier_transform_cursor (Magnifier *magnifier);
void         magnifier_zoom_regions_update_pointer (Magnifier *magnifier);
gboolean     magnifier_full_screen_capable (Magnifier *magnifier);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MAGNIFIER_H_ */
