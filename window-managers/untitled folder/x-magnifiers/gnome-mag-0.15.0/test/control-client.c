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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xfuncs.h>
#include <libbonobo.h>
#include <gdk/gdk.h>
#include "GNOME_Magnifier.h"

static void
init_rect_bounds (GNOME_Magnifier_RectBounds *bounds,
		  long x1, long y1, long x2, long y2)
{
	bounds->x1 = x1;
	bounds->y1 = y1;
	bounds->x2 = x2;
	bounds->y2 = y2;
}

static GNOME_Magnifier_Magnifier 
get_magnifier()
{
  static GNOME_Magnifier_Magnifier magnifier = NULL;
  static gboolean is_error = FALSE;
  CORBA_Object oclient;
  char *obj_id;
  CORBA_Environment ev;
  
  if (!magnifier && !is_error)
  {
    CORBA_exception_init (&ev);
    obj_id = "OAFIID:GNOME_Magnifier_Magnifier:0.9";

    oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
    if (ev._major != CORBA_NO_EXCEPTION) {
      fprintf (stderr,
            ("Activation error: during magnifier activation: %s\n"),
            CORBA_exception_id(&ev));
      CORBA_exception_free(&ev);
      is_error = TRUE;
    }

    if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate magnifier");
      is_error = TRUE;
    }

    magnifier = (GNOME_Magnifier_Magnifier) oclient;

    /* bonobo_activate (); ? */
  }

  return magnifier;
}

static GNOME_Magnifier_ZoomRegion
test_client_magnifier_get_zoomer (GNOME_Magnifier_Magnifier magnifier, int index)
{
	CORBA_Environment ev;
	GNOME_Magnifier_ZoomRegionList *zoomers;
	CORBA_exception_init (&ev);
	zoomers = GNOME_Magnifier_Magnifier_getZoomRegions (magnifier, &ev);
	if (zoomers && index < zoomers->_length)
		return zoomers->_buffer [index];
	else
		return CORBA_OBJECT_NIL;
}

static void
magnifier_clear_all_regions ()
{
  GNOME_Magnifier_Magnifier magnifier = get_magnifier();
  CORBA_Environment ev;
  CORBA_exception_init (&ev);

  if (magnifier)
       GNOME_Magnifier_Magnifier_clearAllZoomRegions (magnifier,
						    &ev);
}

static int
magnifier_create_region (float zx, float zy, int x1, int y1, int x2, int y2)
{
  GNOME_Magnifier_Magnifier magnifier = get_magnifier();
  GNOME_Magnifier_ZoomRegion zoomer;
  CORBA_Environment ev;
  CORBA_exception_init (&ev);
  
  if (magnifier)
  {
	  GNOME_Magnifier_RectBounds *roi = GNOME_Magnifier_RectBounds__alloc ();
	  GNOME_Magnifier_RectBounds *viewport = GNOME_Magnifier_RectBounds__alloc ();
	  roi->x1 = 0;
	  roi->y1 = 0;
	  roi->x2 = x2 - x1;
	  roi->y2 = y2 - y1;
	  viewport->x1 = x1;
	  viewport->y1 = y1;
	  viewport->x2 = x2;
	  viewport->y2 = y2;
	  zoomer = GNOME_Magnifier_Magnifier_createZoomRegion (magnifier,
							       (const CORBA_float) zx,
							       (const CORBA_float) zy,
							       roi,
							       viewport,
							       &ev);
	  GNOME_Magnifier_Magnifier_addZoomRegion (magnifier,
						   zoomer,
						   &ev);
  }
  return 1;
}

int main(int argc, char ** argv){

	GNOME_Magnifier_Magnifier magnifier;
	GNOME_Magnifier_RectBounds *viewport = NULL;
	GNOME_Magnifier_ZoomRegion zoom_region;
	Bonobo_PropertyBag properties;
	CORBA_Environment ev;
	CORBA_any *rect_any;
	GNOME_Magnifier_RectBounds *rectangle;
	CORBA_boolean boo;
	CORBA_string target_disp = NULL;
	CORBA_string source_disp = NULL;
	int x, y, width, height;

	CORBA_exception_init (&ev);

	float R, G, B;
	
	if (!bonobo_init (&argc, argv))
	{
		g_error ("Could not initialize Bonobo");
	}
	
	if (argc >= 2){
		magnifier = get_magnifier ();
	}
	else
		return 0;

	properties = GNOME_Magnifier_Magnifier_getProperties (magnifier, &ev);
	viewport = GNOME_Magnifier_RectBounds__alloc ();
	rect_any = bonobo_pbclient_get_value (properties,
					      "target-display-bounds",
					      TC_GNOME_Magnifier_RectBounds,
					      &ev);
	memcpy (viewport, rect_any->_value,
		sizeof (GNOME_Magnifier_RectBounds));
	if (viewport->x1 == 0 && viewport->y1 == 0 && viewport->x2 == 0 &&
	    viewport->y2 == 0) {
		gdk_init (&argc, &argv);
		width = gdk_screen_get_width (gdk_screen_get_default ());
		height = gdk_screen_get_height (gdk_screen_get_default ());
		x = width / 2;
		y = 0;
		init_rect_bounds (viewport, x, y, width, height);
		rect_any = bonobo_arg_new_from (TC_GNOME_Magnifier_RectBounds,
						viewport);
		bonobo_pbclient_set_value (properties, "target-display-bounds",
					   rect_any, &ev);
		bonobo_arg_release (rect_any);
	}
	bonobo_object_release_unref (properties, NULL);

	zoom_region = test_client_magnifier_get_zoomer (magnifier, 0);

	switch (*argv[1])
        {
	case 'z':	
		if (zoom_region == CORBA_OBJECT_NIL) return -1;
		printf ("setting mag factor to %f\n", (float) atof (argv[1]+1));
		GNOME_Magnifier_ZoomRegion_setMagFactor (zoom_region,
							 (float) atof (argv[1]+1),
							 (float) atof (argv[1]+1),
							 &ev);
		break;
	case 'b':
		if (zoom_region == CORBA_OBJECT_NIL) return -1;
		printf ("resizing region 0 to 100x100 at (200, 0)\n");
		viewport = GNOME_Magnifier_RectBounds__alloc ();
		viewport->x1 = 0;
		viewport->y1 = 0;
		viewport->x2 = 680;
		viewport->y2 = 510;
		GNOME_Magnifier_ZoomRegion_moveResize (zoom_region,
						       viewport,
						       &ev);
		break;
	case 'd':
		printf ("destroying/clearing all regions.\n");
		magnifier_clear_all_regions ();
		break;
	case 'c':
		fprintf (stderr, "creating 2.5x by 5x region at 100,100; 300x200\n");
		magnifier_create_region (2.5, 2.5, 341, 0, 1023, 510);
		break;
        case 's':
		GNOME_Magnifier_Magnifier__set_SourceDisplay (magnifier, argv[1]+1, &ev);
		fprintf (stderr, "\n Set Source To : %s\n", argv[1]+1);
		break;
	case 'o':	
		target_disp = GNOME_Magnifier_Magnifier__get_TargetDisplay (magnifier, &ev);
		fprintf (stderr, "\n Magnifier Target is : %s\n", target_disp);
		break;
        case 'p':
	    	source_disp = GNOME_Magnifier_Magnifier__get_SourceDisplay (magnifier, &ev);
		fprintf (stderr, "\n Magnifier Source is : %s\n", source_disp);
		break;
        case 't':
		GNOME_Magnifier_Magnifier__set_TargetDisplay (magnifier, argv[1]+1, &ev);
		fprintf (stderr, "\n Set Target to : %s\n", argv[1]+1);
		break;
        case 'm':
		properties = GNOME_Magnifier_Magnifier_getProperties (magnifier, &ev);
		bonobo_pbclient_set_float (properties, "cursor-scale-factor", 2.0, NULL);
		bonobo_pbclient_set_ulong (properties, "cursor-color", 0x255, NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
	case 'f':
		properties = GNOME_Magnifier_Magnifier_getProperties (magnifier, &ev);
		bonobo_pbclient_set_long (properties, "crosswire-size", atoi (argv[2]), NULL);
		bonobo_pbclient_set_long (properties, "crosswire-length", atoi (argv[3]), NULL);
		bonobo_pbclient_set_boolean (properties, "crosswire-clip", atoi (argv[4]), NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
	case 'a':
		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
		switch (bonobo_pbclient_get_long (properties, "x-alignment", NULL)) {
		case GNOME_Magnifier_ZoomRegion_ALIGN_CENTER:
			bonobo_pbclient_set_long (properties, "x-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_MIN, NULL);
			bonobo_pbclient_set_long (properties, "y-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_MIN, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_ALIGN_MIN:
			bonobo_pbclient_set_long (properties, "x-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_MAX, NULL);
			bonobo_pbclient_set_long (properties, "y-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_MAX, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_ALIGN_MAX:
			bonobo_pbclient_set_long (properties, "x-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_CENTER, NULL);
			bonobo_pbclient_set_long (properties, "y-alignment", GNOME_Magnifier_ZoomRegion_ALIGN_CENTER, NULL);
			break;
		}
		bonobo_object_release_unref (properties, NULL);
		break;
	case 'k':
		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
		bonobo_pbclient_set_long (properties, "border-size-left", atoi (argv[2]), NULL);
		bonobo_pbclient_set_long (properties, "border-size-top", atoi (argv[3]), NULL);
		bonobo_pbclient_set_long (properties, "border-size-right", atoi (argv[4]), NULL);
		bonobo_pbclient_set_long (properties, "border-size-bottom", atoi (argv[5]), NULL);
		bonobo_pbclient_set_long (properties, "border-color", 255, NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
	case 'l':
		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
		bonobo_pbclient_set_long (properties, "border-size", atoi (argv[2]), NULL);
		bonobo_pbclient_set_long (properties, "border-color", 255, NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
	case 'x':
		GNOME_Magnifier_Magnifier_dispose (magnifier, &ev);
		break;
	case 'B':
		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
		switch (bonobo_pbclient_get_short (properties, "color-blind-filter", NULL)) {
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_NO_FILTER:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_RED, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_RED:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_GREEN, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_GREEN:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_BLUE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE_BLUE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_RED, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_RED:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_GREEN, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_GREEN:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_BLUE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE_BLUE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_HUE_SHIFT_POSITIVE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_HUE_SHIFT_POSITIVE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_HUE_SHIFT_NEGATIVE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_HUE_SHIFT_NEGATIVE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_SATURATE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_SELECTIVE_DESSATURATE:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_MONOCHRONE_OTHERS, NULL);
			break;
		case GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_MONOCHRONE_OTHERS:
			bonobo_pbclient_set_short (properties, "color-blind-filter", GNOME_Magnifier_ZoomRegion_COLORBLIND_FILTER_T_NO_FILTER, NULL);
			break;
		}
		break;
	case 'C':
		GNOME_Magnifier_ZoomRegion_setContrast (zoom_region,
							atof (argv[2]),
							atof (argv[3]),
							atof (argv[4]),
							&ev);
		GNOME_Magnifier_ZoomRegion_getContrast (zoom_region, &R, &G, &B, &ev);
		printf ("ZoomRegion R, G, B: %f, %f, %f\n", R, G, B);
		break;
	case 'D':
		GNOME_Magnifier_ZoomRegion_setBrightness (zoom_region,
							  atof (argv[2]),
							  atof (argv[3]),
							  atof (argv[4]),
							  &ev);
		GNOME_Magnifier_ZoomRegion_getBrightness (zoom_region, &R, &G, &B, &ev);
		printf ("ZoomRegion R, G, B: %f, %f, %f\n", R, G, B);
		break;
        case 'S':
		properties = GNOME_Magnifier_Magnifier_getProperties (magnifier, &ev);
		bonobo_pbclient_set_long (properties, "cursor-size", 200, NULL);
		bonobo_pbclient_set_ulong (properties, "cursor-color", 0xFFF, NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
        case 'T':
		rect_any = CORBA_any__alloc ();
		rectangle = GNOME_Magnifier_RectBounds__alloc ();
		rectangle->x1 = 341;
		rectangle->x2 = 1023;
		rectangle->y1 = 0;
		rectangle->y2 = 510;
		rect_any->_type = TC_GNOME_Magnifier_RectBounds;
		rect_any->_value = ORBit_copy_value (rectangle, TC_GNOME_Magnifier_RectBounds);
		properties = GNOME_Magnifier_Magnifier_getProperties (magnifier, &ev);
		Bonobo_PropertyBag_setValue (properties, "target-display-bounds", rect_any, NULL);
		bonobo_object_release_unref (properties, NULL);
		break;
        case 'X':
		if (zoom_region == CORBA_OBJECT_NIL) return -1;
		printf ("toggle mouse polling\n");
		properties = GNOME_Magnifier_ZoomRegion_getProperties (zoom_region, &ev);
		boo = bonobo_pbclient_get_boolean (properties, "poll-mouse", NULL);
		bonobo_pbclient_set_boolean (properties, "poll-mouse", !boo, NULL);
		bonobo_object_release_unref (properties, NULL);
		GNOME_Magnifier_ZoomRegion_setPointerPos (zoom_region,
							  50, 50,
							  &ev);
		break;
        }
return 0;
}

