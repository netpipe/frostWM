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
#include "gmag-graphical-server.h"
#include "GNOME_Magnifier.h"

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <libbonobo.h>

#define ENV_STRING_MAX_SIZE 128

GNOME_Magnifier_ZoomRegion zoom_region;

typedef struct {
	gchar    *target_display;
	gchar    *source_display;
	gchar    *cursor_set;
	gchar    *smoothing_type;
	gdouble   zoom_factor;
	gdouble   zoom_factor_x;
	gdouble   zoom_factor_y;
	gint      refresh_time;
	gint      mouse_poll_time;
	gint      cursor_size;
	gdouble   cursor_scale_factor;
	gint64    cursor_color;
	gboolean  vertical_split;
	gboolean  horizontal_split;
	gboolean  fullscreen;
	gboolean  mouse_follow;
	gboolean  invert_image;
	gboolean  no_initial_region;
	gint      timing_iterations;
	gboolean  timing_output;
	gint      timing_delta_x;
	gint      timing_delta_y;
	gint      timing_pan_rate;
	gboolean  smooth_scroll;
	gint      border_width;
	gint64    border_color;
	gboolean  test_pattern;
	gboolean  is_override_redirect;
	gboolean  ignore_damage;
#ifdef HAVE_COMPOSITE
	gboolean   ignore_composite;
#endif /* HAVE_COMPOSITE */
	gboolean  print_version;
	gboolean  hide_pointer;
	gboolean  show_crosswires;
} MagnifierOptions;

static MagnifierOptions global_options = { NULL,      /* target_display */
					   NULL,      /* source_display */
					   "default", /* cursor_set */
					   "none",    /* smoothing_type */
					   2.0,       /* zoom_factor */
					   0.0,       /* zoom_factor_x */
					   0.0,       /* zoom_factor_y */
					   500,       /* refresh_time */
					   50,        /* mouse_poll_time */
					   0,         /* cursor_size */
					   0.0F,      /* cursor_scale_factor */
					   0xFF000000,/* cursor_color */
					   0,         /* vertical_split */
					   0,         /* horizontal_split */
					   0,         /* fullscreen */
					   0,         /* mouse_follow */
					   0,	      /* invert_image */
                                   	   0,         /* no_initial_region */
					   0,         /* timing_iterations */
					   0,         /* timing_output */
					   10,        /* timing_delta_x */
					   10,        /* timing_delat_y */
					   0,         /* timing_pan_rate */
					   1,         /* smooth_scroll */
					   0,         /* border_width */
					   0,         /* border_color */
					   0,         /* test_pattern */
					   0,         /* is_override_redirect*/
					   0          /* ignore_damage */
#ifdef HAVE_COMPOSITE
					   ,0         /* ignore_composite */
#endif /* HAVE_COMPOSITE */
					   ,0,        /* print_version */
					   0,         /* hide_pointer */
					   0          /* show_crosswires */ 
                                         };

static GOptionEntry magnifier_options [] = {
	{"target-display", 't', 0, G_OPTION_ARG_STRING, &global_options.target_display, "specify display on which to show magnified view", NULL},
	{"source-display", 's', 0, G_OPTION_ARG_STRING, &global_options.source_display, "specify display to magnify", NULL},
	{"cursor-set", 0, 0, G_OPTION_ARG_STRING, &global_options.cursor_set, "cursor set to use in target display", NULL},
	{"cursor-size", 0, 0, G_OPTION_ARG_INT, &global_options.cursor_size, "cursor size to use (overrides cursor-scale-factor)", NULL},
	{"cursor-scale-factor", 0, 0, G_OPTION_ARG_DOUBLE, &global_options.cursor_scale_factor, "cursor scale factor", NULL},
	{"cursor-color", 0, 0, G_OPTION_ARG_INT64, &global_options.cursor_color, "cursor color (applied to \'black\' pixels)", NULL},
	{"vertical", 'v', 0, G_OPTION_ARG_NONE, &global_options.vertical_split, "split screen vertically (if target display = source display)", NULL},
	{"horizontal", 'h', 0, G_OPTION_ARG_NONE, &global_options.horizontal_split, "split screen horizontally (if target display = source display)", NULL},
	{"mouse-follow", 'm', 0, G_OPTION_ARG_NONE, &global_options.mouse_follow, "track mouse movements", NULL},
	{"refresh-time", 'r', 0, G_OPTION_ARG_INT, &global_options.refresh_time, "minimum refresh time for idle, in ms", NULL},
	{"mouse-latency", 0, 0, G_OPTION_ARG_INT, &global_options.mouse_poll_time, "maximum mouse latency time, in ms", NULL},
	{"zoom-factor", 'z', 0, G_OPTION_ARG_DOUBLE, &global_options.zoom_factor, "zoom (scale) factor used to magnify source display", NULL}, 
	{"invert-image", 'i', 0, G_OPTION_ARG_NONE, &global_options.invert_image, "invert the image colormap", NULL}, 
	{"no-initial-region", 0, 0, G_OPTION_ARG_NONE, &global_options.no_initial_region, "don't create an initial zoom region", NULL},
	{"timing-iterations", 0, 0, G_OPTION_ARG_INT, &global_options.timing_iterations, "iterations to run timing benchmark test (0=continuous)", NULL},
	{"timing-output", 0, 0, G_OPTION_ARG_NONE, &global_options.timing_output, "display performance ouput", NULL},
	{"timing-pan-rate", 0, 0, G_OPTION_ARG_INT, &global_options.timing_pan_rate, "timing pan rate in lines per frame", NULL},
	{"timing-delta-x", 0, 0, G_OPTION_ARG_INT, &global_options.timing_delta_x, "pixels to pan in x-dimension each frame in timing update test", NULL},
	{"timing-delta-y", 0, 0, G_OPTION_ARG_INT, &global_options.timing_delta_y, "pixels to pan in y-dimension each frame in timing update test", NULL},
	{"smoothing-type", 0, 0, G_OPTION_ARG_STRING, &global_options.smoothing_type, "image smoothing algorithm to apply (bilinear-interpolation | none)", NULL},
	{"fullscreen", 'f', 0, G_OPTION_ARG_NONE, &global_options.fullscreen, "fullscreen magnification, covers entire target display [REQUIRES --source-display and --target-display]", NULL},
	{"smooth-scrolling", 0, 0, G_OPTION_ARG_NONE, &global_options.smooth_scroll, "use smooth scrolling", NULL},
	{"border-size", 'b', 0, G_OPTION_ARG_INT, &global_options.border_width, "width of border", NULL},
	{"border-color", 'c', 0, G_OPTION_ARG_INT64, &global_options.border_color, "border color specified as (A)RGB 23-bit value, Alpha-MSB", NULL},
	{"hide-pointer", 0, 0, G_OPTION_ARG_NONE, &global_options.hide_pointer, "hide magnifier pointer when passed", NULL},
	{"show-crosswires", 0, 0, G_OPTION_ARG_NONE, &global_options.show_crosswires, "show crosswires when passed", NULL},
	{"use-test-pattern", 0, 0, G_OPTION_ARG_NONE, &global_options.test_pattern, "use test pattern as source", NULL},
	{"override-redirect", 0, 0, G_OPTION_ARG_NONE, &global_options.is_override_redirect, "make the magnifier window totally unmanaged by the window manager", NULL},
	{"ignore-damage", 0, 0, G_OPTION_ARG_NONE, &global_options.ignore_damage, "ignore the X server DAMAGE extension, if present", NULL},
#ifdef HAVE_COMPOSITE
	{"ignore-composite", 0, 0, G_OPTION_ARG_NONE, &global_options.ignore_composite, "ignore the X server COMPOSITE extension, if present", NULL},
#endif /* HAVE_COMPOSITE */
	{"version", 0, 0, G_OPTION_ARG_NONE, &global_options.print_version, "print version", NULL},
	{NULL}
};

static void
init_rect_bounds (GNOME_Magnifier_RectBounds *bounds,
		  long x1, long y1, long x2, long y2)
{
	bounds->x1 = x1;
	bounds->y1 = y1;
	bounds->x2 = x2;
	bounds->y2 = y2;
}

static int target_width, target_height;

static int
magnifier_main_test_image (gpointer data)
{
	static long timing_counter = 0;
	static int timing_x_pos = 0;
	static int timing_y_pos = 0;
	static int x_direction = 1;
	static int y_direction = 1;
	Magnifier *magnifier = (Magnifier *) data;
	GNOME_Magnifier_ZoomRegionList *zoom_regions;
	Bonobo_PropertyBag properties;
	CORBA_Environment ev;
	GNOME_Magnifier_RectBounds roi;
	int x_roi, y_roi;

	/* Only iterate the number of times specified */
	if (global_options.timing_iterations > 0) {
		if (timing_counter > global_options.timing_iterations) {
  			CORBA_exception_init (&ev);
	  		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
			if (BONOBO_EX (&ev))
				fprintf (stderr, "EXCEPTION\n");

	  		bonobo_pbclient_set_boolean (properties, "exit-magnifier",
				       TRUE, &ev);
		}
	}

	CORBA_exception_init (&ev);

	x_roi  = global_options.timing_delta_x * timing_x_pos;
	roi.x1 = x_roi;
	roi.x2 = (target_width / global_options.zoom_factor) + roi.x1;
	x_roi  = global_options.timing_delta_x * (timing_x_pos + x_direction);

	/* Determine if magnifier hit an edge and should reverse direction */
	if (x_roi + (target_width / global_options.zoom_factor) > target_width)
		x_direction = -1;
	else if (x_roi < 0)
		x_direction = 1;

	timing_x_pos += x_direction;

	y_roi = global_options.timing_delta_y * timing_y_pos;

	/* Calculate size of screen not covered by magnifier */
	if (global_options.horizontal_split)
		roi.y1 = y_roi + target_height;
	else
		roi.y1 = y_roi;
	roi.y2 = (target_height / global_options.zoom_factor) + roi.y1;

	y_roi = global_options.timing_delta_y * (timing_y_pos + y_direction);

	/* The counter is increased each time the y-direction changes */
	if (y_roi + (target_height / global_options.zoom_factor) >
	    target_height) {
		timing_counter++;
		y_direction = -1;
	}
	else if (y_roi < 0) {
		timing_counter++;
		y_direction = 1;
	}

	timing_y_pos += y_direction;

	if (!IS_MAGNIFIER (magnifier))
		return FALSE;

	magnifier->priv->cursor_x = (roi.x2 + roi.x1) / 2;
	magnifier->priv->cursor_y = (roi.y2 + roi.y1) / 2;

	zoom_regions =
		GNOME_Magnifier_Magnifier_getZoomRegions (
			BONOBO_OBJREF (magnifier),
			&ev);

	if (zoom_regions && (zoom_regions->_length > 0)) {

		GNOME_Magnifier_ZoomRegion_setROI (
			zoom_regions->_buffer[0], &roi, &ev);
	}

	return TRUE;
}

static int last_x = 0, last_y = 0;

static int
magnifier_main_pan_image (gpointer data)
{
  Magnifier *magnifier = (Magnifier *) data;
  GNOME_Magnifier_ZoomRegionList *zoom_regions;
  GNOME_Magnifier_ZoomRegion zoom_region;
  CORBA_Environment ev;
  GNOME_Magnifier_RectBounds roi;
  int mouse_x_return, mouse_y_return;
  int w, h;
  GdkModifierType mask_return;

  CORBA_exception_init (&ev);

  if (global_options.mouse_follow && IS_MAGNIFIER (magnifier))
  {
	  gdk_window_get_pointer (
		  magnifier_get_root (magnifier),
		  &mouse_x_return,
		  &mouse_y_return,
		  &mask_return);
	  
	  if (last_x != mouse_x_return || last_y != mouse_y_return)
	  {
		  last_x = mouse_x_return;
		  last_y = mouse_y_return;
		  w = (magnifier->target_bounds.x2 - magnifier->target_bounds.x1);
		  h = (magnifier->target_bounds.y2 - magnifier->target_bounds.y1);
		  roi.x1 = mouse_x_return;
		  roi.y1 = mouse_y_return;
		  roi.x2 = roi.x1 + 1;
		  roi.y2 = roi.y1 + 1;
		  
		  zoom_regions =
			  GNOME_Magnifier_Magnifier_getZoomRegions (
				  BONOBO_OBJREF (magnifier),
				  &ev);
		  if (zoom_regions && (zoom_regions->_length > 0))
		  {
			  int i;
			  for (i = 0; i < zoom_regions->_length; ++i)
			  {
				  /* fprintf (stderr, "panning region %d\n", i);*/
				  zoom_region =
					  CORBA_Object_duplicate (
						  ( (CORBA_Object *)
						    (zoom_regions->_buffer))[i], &ev);
				  if (zoom_region != CORBA_OBJECT_NIL) {
					  GNOME_Magnifier_ZoomRegion_setROI (zoom_region,
									     &roi,
									     &ev);
				  } else fprintf (stderr, "nil region!\n");
			  }
		  }
	  }
	  return TRUE;
  }
  
  return FALSE;
}

static int
magnifier_main_refresh_all (gpointer data)
{
	int i;
	Magnifier *magnifier = data;
	CORBA_any *dirty_bounds_any;
	CORBA_Environment ev;
	Bonobo_PropertyBag properties;
	GNOME_Magnifier_RectBounds *dirty_bounds;
	GNOME_Magnifier_ZoomRegionList *regions;
	
	CORBA_exception_init (&ev);

	if (!IS_MAGNIFIER (magnifier))
		return FALSE;
	
	regions = GNOME_Magnifier_Magnifier_getZoomRegions (
		BONOBO_OBJREF (magnifier),
		&ev);

#ifdef DEBUG_REFRESH
	fprintf (stderr, "refreshing %d regions\n", regions->_length);
#endif

	properties = GNOME_Magnifier_Magnifier_getProperties (BONOBO_OBJREF (magnifier), &ev);

	dirty_bounds_any = Bonobo_PropertyBag_getValue (properties, "source-display-bounds", &ev);
	if (BONOBO_EX (&ev)) {
		g_warning ("Error getting source-display-bounds");
		bonobo_main_quit ();
		return FALSE;
	}

	dirty_bounds = (GNOME_Magnifier_RectBounds *) dirty_bounds_any->_value;

	  fprintf (stderr, "region to update: %d %d %d %d\n",
		 dirty_bounds->x1, dirty_bounds->y1, dirty_bounds->x2, dirty_bounds->y2);

	for (i = 0; i < regions->_length; ++i)
		GNOME_Magnifier_ZoomRegion_markDirty (
			regions->_buffer [i], dirty_bounds, &ev);

	bonobo_object_release_unref (properties, NULL);

	return TRUE;
}

int
main (int argc, char** argv)
{
  GOptionContext *context;
  GNOME_Magnifier_RectBounds *roi = GNOME_Magnifier_RectBounds__alloc();
  GNOME_Magnifier_RectBounds *viewport = GNOME_Magnifier_RectBounds__alloc();
  CORBA_any *viewport_any;
  int x = 0, y = 0, src_width, src_height;
  guint pan_handle = 0, refresh_handle = 0;
  CORBA_Environment ev;
  Bonobo_PropertyBag properties;

  Magnifier *magnifier;
  
  if (!bonobo_init (&argc, argv)) {
    g_error ("Could not initialize Bonobo");
  }
  CORBA_exception_init (&ev);
  
  context = g_option_context_new ("- a screen magnifier for Gnome");
  g_option_context_set_description (context, "Report bugs to http://bugzilla.gnome.org\n");
  g_option_context_add_main_entries (context, magnifier_options, "main options");
  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_parse(context, &argc, &argv, NULL);
  g_option_context_free(context);

  if (global_options.print_version) {
	  g_print ("%s\n", VERSION);
	  return 0;
  }

  /** 
   * slight hack, we set $DISPLAY if --target-display is set.
   * this is because I don't think GTK+ works well with across multiple 
   * servers yet.
   **/
  if (global_options.target_display) {
	  gchar *string;
	  string = g_strconcat ("DISPLAY=", global_options.target_display, NULL);
	  putenv (string);
  } else {
		  global_options.target_display = getenv ("DISPLAY");
		  if (!global_options.target_display) {
			  fprintf (stderr, _("Can't open display: DISPLAY is not set"));
			  exit (1);
		  }
  }  

  if (!global_options.source_display) {
		  global_options.source_display = global_options.target_display;
  }

  if (global_options.timing_pan_rate && global_options.timing_iterations == 0)
  {
    g_error ("Must specify timing_iterations when running pan test");
  }

  /* FIXME */
  gtk_init (&argc, &argv);

  if (global_options.ignore_damage)
  {
      g_setenv ("MAGNIFIER_IGNORE_DAMAGE", "1", TRUE);
  }
#ifdef HAVE_COMPOSITE
  if (global_options.ignore_composite)
	  g_setenv ("MAGNIFIER_IGNORE_COMPOSITE", "1", TRUE);
#endif /* HAVE_COMPOSITE */

  magnifier = magnifier_new (global_options.is_override_redirect);
  
  properties = GNOME_Magnifier_Magnifier_getProperties (
	  BONOBO_OBJREF (magnifier), &ev);
  if (ev._major != CORBA_NO_EXCEPTION) fprintf (stderr, "EXCEPTION\n");

  if (global_options.source_display)
	  bonobo_pbclient_set_string (properties, "source-display-screen",
				      global_options.source_display, NULL);

  if (global_options.target_display)
	  bonobo_pbclient_set_string (properties, "target-display-screen",
				      global_options.target_display, NULL);

  if (global_options.cursor_set)
	  bonobo_pbclient_set_string (properties, "cursor-set",
				      global_options.cursor_set, NULL);

  if (global_options.cursor_size)
	  bonobo_pbclient_set_long (properties, "cursor-size",
				    global_options.cursor_size, NULL);

  else if (global_options.cursor_scale_factor != 0.0F)
	  bonobo_pbclient_set_float (properties, "cursor-scale-factor",
				     global_options.cursor_scale_factor, NULL);
  else 
	  bonobo_pbclient_set_float (properties, "cursor-scale-factor",
				     global_options.zoom_factor, NULL);

  if (global_options.cursor_color)
	  bonobo_pbclient_set_ulong (properties, "cursor-color",
				     global_options.cursor_color, 
				     NULL);

  if (!global_options.show_crosswires)
	  bonobo_pbclient_set_long (properties, "crosswire-size", 0, NULL);

  src_width = gdk_screen_get_width (gdk_display_get_screen (
					    magnifier->source_display,
					    magnifier->source_screen_num));
  src_height = gdk_screen_get_height (gdk_display_get_screen (
					      magnifier->source_display,
					      magnifier->source_screen_num));

  target_width = gdk_screen_get_width (gdk_display_get_screen (
					       magnifier->target_display,
					       magnifier->target_screen_num));
  target_height = gdk_screen_get_height (
	  gdk_display_get_screen (magnifier->target_display,
				  magnifier->target_screen_num));

  if (global_options.vertical_split) {
	  target_width /= 2;
	  x = target_width;
  }
  if (global_options.horizontal_split) {
	  target_height /= 2;
	  y = target_height;
  }

  fprintf (stderr, "initial viewport %d %d\n", (int) target_width,
	   (int) target_height);

  
  if (global_options.vertical_split || global_options.horizontal_split ||
      global_options.fullscreen) {
	  init_rect_bounds (viewport, x, y, x + target_width,
			    y + target_height);
	  viewport_any = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds,
					      viewport);
  
	  bonobo_pbclient_set_value (properties, "target-display-bounds",
				     viewport_any, &ev);
	  bonobo_arg_release (viewport_any);
  }

  if (global_options.vertical_split || global_options.horizontal_split) 
  {
#ifdef HAVE_COMPOSITE
	  if (!g_getenv ("MAGNIFIER_IGNORE_COMPOSITE"))
		  init_rect_bounds (viewport, 0, 0,
				    src_width, src_height);
	  else
#endif /* HAVE_COMPOSITE */
		  init_rect_bounds (viewport, 0, 0,
				    src_width-x, src_height-y);
      viewport_any = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds,
					  viewport);
      bonobo_pbclient_set_value (properties, "source-display-bounds",
				 viewport_any,
				 &ev);

      bonobo_arg_release (viewport_any);
  } else if (global_options.fullscreen) {
	  init_rect_bounds (viewport, 0, 0, src_width, src_height);
	  viewport_any = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds,
					      viewport);
	  bonobo_pbclient_set_value (properties, "source-display-bounds",
				     viewport_any,
				     &ev);
	  bonobo_arg_release (viewport_any);
  }

  bonobo_object_release_unref (properties, NULL);
  properties = NULL;

  if (global_options.vertical_split ||
      global_options.horizontal_split ||
      global_options.fullscreen)
  {
	  int scroll_policy;
	  
	  init_rect_bounds (roi, 0, 0,
			    target_width / global_options.zoom_factor,
			    target_height / global_options.zoom_factor);
	  init_rect_bounds (viewport, 0, 0, target_width, target_height);
	  zoom_region =
		  GNOME_Magnifier_Magnifier_createZoomRegion (
			  BONOBO_OBJREF (magnifier),
			  global_options.zoom_factor,
			  global_options.zoom_factor,
			  roi,
			  viewport,
			  &ev);
	  
	  properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
	  if (BONOBO_EX (&ev))
		  fprintf (stderr, "EXCEPTION\n");

	  scroll_policy = global_options.smooth_scroll ?
		  GNOME_Magnifier_ZoomRegion_SCROLL_SMOOTHEST :
		  GNOME_Magnifier_ZoomRegion_SCROLL_FASTEST;

	  bonobo_pbclient_set_long (properties, "timing-iterations",
				       global_options.timing_iterations, &ev);
	  bonobo_pbclient_set_boolean (properties, "timing-output",
				       global_options.timing_output, &ev);
	  bonobo_pbclient_set_long (properties, "timing-pan-rate",
				       global_options.timing_pan_rate, &ev);
	  bonobo_pbclient_set_long    (properties, "border-size",
				       global_options.border_width, &ev);
	  bonobo_pbclient_set_long    (properties, "border-color",
				       global_options.border_color, &ev);
	  bonobo_pbclient_set_short   (properties, "smooth-scroll-policy",
				       (short) scroll_policy, &ev);
	  bonobo_pbclient_set_boolean (properties, "use-test-pattern",
				       global_options.test_pattern, &ev);
	  bonobo_pbclient_set_boolean (properties, "draw-cursor",
				       !global_options.hide_pointer, &ev);

	  if (strcmp (global_options.smoothing_type, "none"))
		  bonobo_pbclient_set_string (properties, "smoothing-type",
					      global_options.smoothing_type, &ev);

	  if (global_options.invert_image)
		  bonobo_pbclient_set_boolean (properties, "inverse-video",
					       global_options.invert_image, NULL);

	  GNOME_Magnifier_Magnifier_addZoomRegion (
		  BONOBO_OBJREF (magnifier),
		  zoom_region,
		  &ev);

	  bonobo_object_release_unref (properties, &ev);
	  properties = NULL;
  }

  if (global_options.timing_pan_rate)
  {
	  GNOME_Magnifier_ZoomRegionList *zoom_regions;
	  GNOME_Magnifier_RectBounds roi;
	  roi.x1 = 100;
	  roi.x2 = 100 + (target_width / global_options.zoom_factor);
	  roi.y1 = 0;
	  roi.y2 = target_height / global_options.zoom_factor;
	  
	  zoom_regions = GNOME_Magnifier_Magnifier_getZoomRegions (
		  BONOBO_OBJREF (magnifier), &ev);
	  
	  if (zoom_regions && (zoom_regions->_length > 0))
	  {
		  GNOME_Magnifier_ZoomRegion_setROI (
			  zoom_regions->_buffer[0], &roi, &ev);
	  }
  }
  else if (global_options.timing_iterations)
  {
	  refresh_handle = g_timeout_add (global_options.refresh_time,
					  magnifier_main_test_image,
					  magnifier);
  }
  else
  {
	  if (global_options.ignore_damage ||
	      !gmag_gs_source_has_damage_extension (magnifier)) 
	  {
	  	  refresh_handle = g_timeout_add (
	  		  global_options.refresh_time,
	  		  magnifier_main_refresh_all, magnifier);
	  }
	  
	  pan_handle = g_timeout_add (
		  global_options.mouse_poll_time,
		  magnifier_main_pan_image, magnifier);
  }
  
  bonobo_main ();
  
  if (refresh_handle)
	  g_source_remove (refresh_handle);
  
  if (pan_handle)
	  g_source_remove (pan_handle);

  return 0;
}
