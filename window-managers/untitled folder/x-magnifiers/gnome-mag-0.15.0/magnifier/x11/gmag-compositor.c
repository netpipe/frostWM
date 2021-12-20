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
#include "gmag-graphical-server.h"
#include "gmag-compositor.h"
#include "magnifier.h"
#include "magnifier-private.h"

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#ifdef HAVE_COMPOSITE
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xcomposite.h>

#include <gdk/gdkx.h>

#define COMPOSITE_DEBUG
#undef  COMPOSITE_DEBUG

#define DAMAGE_DEBUG
#undef  DAMAGE_DEBUG


static Magnifier         *mag_ref;
static Display           *dpy;
static Window             root_window, mag_window;
static GQueue            *mag_windows_list;
static Picture            off_screen_picture;
static XserverRegion      tmp_region = None, new_region = None,
	                  old_region = None, exp_region = None;
static DamageHandlerFunc  gmag_compositor_damage_notify_handler;


/*
 * GCompareFunc used in g_queue_find_custom to find windows in
 * mag_windows_list.
 */
static gint
gmag_compositor_g_compare_func (GmagWinPtr pgmag_win, Window xwin)
{
	if (pgmag_win->xwin == xwin)
		return 0;

	return 1;
}

/*
 * Calculates the clip for all windows in mag_windows_list.
 */
static void
gmag_compositor_calculate_windows_clip ()
{
	GList         *elem = NULL;
	XserverRegion  clipSum;

	clipSum = XFixesCreateRegion (dpy, 0, 0);
	elem = g_queue_peek_tail_link (mag_windows_list);
	if (!elem) {
		XFixesDestroyRegion (dpy, clipSum);
		return;
	}
	do {
		GmagWinPtr pgmag_win = (GmagWinPtr) elem->data;
		if (pgmag_win->pic)
			if (pgmag_win->attr.map_state == IsViewable) {
				XFixesCopyRegion (dpy, pgmag_win->clip,
						  pgmag_win->win_region);
				XFixesSubtractRegion (dpy, pgmag_win->clip,
						      pgmag_win->clip,
						      clipSum);
				XFixesUnionRegion (dpy, clipSum, clipSum,
						   pgmag_win->clip);
			}
	} while ((elem = g_list_previous (elem)));
	XFixesDestroyRegion (dpy, clipSum);
}

/*
 * Calculates the clip of a single window in mag_windows_list.
 */
static void
gmag_compositor_calculate_window_clip (GmagWinPtr pgmag_win_newclip)
{
	GList         *elem = NULL;
	XserverRegion clipSum;

	clipSum = XFixesCreateRegion (dpy, 0, 0);
	elem = g_queue_peek_tail_link (mag_windows_list);
	if (!elem) {
		XFixesDestroyRegion (dpy, clipSum);
		return;
	}
	do {
		GmagWinPtr pgmag_win = (GmagWinPtr) elem->data;
		if (pgmag_win->xwin == pgmag_win_newclip->xwin) {
			/* the window that must have a new clip was founded */
			XFixesCopyRegion (dpy, pgmag_win->clip,
					  pgmag_win->win_region);
			XFixesSubtractRegion (dpy, pgmag_win->clip,
					      pgmag_win->clip, clipSum);
			break;
		}
		if (pgmag_win->pic)
			if (pgmag_win->attr.map_state == IsViewable) {
				XFixesUnionRegion (dpy, clipSum, clipSum,
						   pgmag_win->win_region);
			}
	} while ((elem = g_list_previous (elem)));
	XFixesDestroyRegion (dpy, clipSum);
}

/*
 * Paint a window. If region is None, the window clip region is painted, else
 * the intersection of the window clip region and region is painted.
 */
static void
gmag_compositor_paint_window (GmagWinPtr pgmag_win, XserverRegion region)
{
	static XserverRegion  final_clip = None;
	XRectangle           *rectlist;
	int                   i, howmany;

	if (!pgmag_win->pic)
		return;
	if (pgmag_win->attr.map_state != IsViewable)
		return;

	if (final_clip == None)
		final_clip = XFixesCreateRegion (dpy, 0, 0);

	if (region) {
		XFixesIntersectRegion (dpy, final_clip, region,
				       pgmag_win->clip);
		XFixesSetPictureClipRegion (dpy, pgmag_win->pic,
					    -(pgmag_win->attr.x),
					    -(pgmag_win->attr.y), final_clip);

		rectlist = XFixesFetchRegion (dpy, final_clip, &howmany);
	} else {
		XFixesSetPictureClipRegion (dpy,
					    pgmag_win->pic,
					    -(pgmag_win->attr.x),
					    -(pgmag_win->attr.y),
					    pgmag_win->clip);
		rectlist = XFixesFetchRegion (dpy, pgmag_win->clip, &howmany);
	}
	
	XRenderComposite (dpy, PictOpSrc,
			  pgmag_win->pic, None, off_screen_picture,
			  0, 0, 0, 0, pgmag_win->attr.x, pgmag_win->attr.y,
			  pgmag_win->attr.width, pgmag_win->attr.height);

	XSync (dpy, FALSE);

	if (rectlist != NULL) {/* reply from fetch */
		for (i=0; i < howmany; ++i) {
			gmag_gs_notify_damage (mag_ref, &rectlist[i]);
		}
		XFree (rectlist);
	}
}

/*
 * Paint all the windows in mag_windows_list with the specified
 * exposedRegion.
 */
static void
gmag_compositor_paint_windows (XserverRegion exposedRegion)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

	elem = g_queue_peek_head_link (mag_windows_list);

	while (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		gmag_compositor_paint_window (pgmag_win, exposedRegion);
		elem = g_list_next (elem);
	}
}

/*
 * Sometimes XGetWindowAttributes fail (when the window is destroied), so we
 * put default values in it to not have problems in other parts of the program.
 * I think that only some ones need to be setted, but this was copied from
 * Compiz, so...
 */
static void
gmag_compositor_set_default_window_attributes (XWindowAttributes *wa)
{
	wa->x		          = 0;
	wa->y		          = 0;
	wa->width		  = 1;
	wa->height		  = 1;
	wa->border_width	  = 0;
	wa->depth		  = 0;
	wa->visual		  = NULL;
	wa->root		  = None;
	wa->class		  = InputOnly;
	wa->bit_gravity	          = NorthWestGravity;
	wa->win_gravity	          = NorthWestGravity;
	wa->backing_store	  = NotUseful;
	wa->backing_planes	  = 0;
	wa->backing_pixel	  = 0;
	wa->save_under	          = FALSE;
	wa->colormap	          = None;
	wa->map_installed	  = FALSE;
	wa->map_state	          = IsUnviewable;
	wa->all_event_masks	  = 0;
	wa->your_event_mask	  = 0;
	wa->do_not_propagate_mask = 0;
	wa->override_redirect     = TRUE;
	wa->screen		  = NULL;
}

/*
 * Creates the necessary information of a redirected window and add it to
 * mag_windows_list.
 */
static void
gmag_compositor_add_window (Window xwin)
{
	GmagWinPtr                new;
	XRenderPictureAttributes  pic_attr;
	XRenderPictFormat        *format;

	new = (GmagWinPtr) malloc (sizeof (GmagWin));
	if (!new)
		g_error ("can't allocate GmagWin (struct _GmagWin)");

	if (!XGetWindowAttributes (dpy, xwin,
				   &new->attr))
		gmag_compositor_set_default_window_attributes (&new->attr);

	new->xwin = xwin;

	if (new->attr.class == InputOnly) {
		new->pic = None;
		new->damage = None;
	} else {
		format = XRenderFindVisualFormat (
			dpy, new->attr.visual);
		pic_attr.subwindow_mode = IncludeInferiors;
		new->pic = XRenderCreatePicture (
			dpy, xwin, format,
			CPSubwindowMode, &pic_attr);
		new->damage = XDamageCreate (dpy, xwin,
					     XDamageReportDeltaRectangles);
		XDamageSubtract (dpy, new->damage, None, None);
		new->clip = XFixesCreateRegion (dpy, 0, 0);
		new->win_region = XFixesCreateRegionFromWindow (
			dpy, xwin, WindowRegionBounding);
		XFixesTranslateRegion (dpy, new->win_region, new->attr.x,
				       new->attr.y);
	}
	
	g_queue_push_tail (mag_windows_list, new);
}

/*
 * Destroy the window resources and remove it from the
 * mag_windows_list.
 */
static void
gmag_compositor_remove_window (Window xwin)
{
	GList     *elem = NULL;
	GmagWinPtr pgmag_win;

	elem = g_queue_find_custom (
		mag_windows_list, (gconstpointer) xwin,
		(GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		g_queue_remove (mag_windows_list, pgmag_win);
		XFixesDestroyRegion (dpy, pgmag_win->clip);
		XFixesDestroyRegion (dpy, pgmag_win->win_region);
		free (pgmag_win);
	}
}

static void
gmag_compositor_remove_element (gpointer data, gpointer user_data)
{
	GmagWinPtr pgmag_win;

	pgmag_win = (GmagWinPtr) data;

	gmag_compositor_remove_window (pgmag_win->xwin);
}

/*
 * Destroy the mag_windows_list. This is needed when a call to change the
 * source/target display is made.
 */
static void
gmag_compositor_destroy_windows_list ()
{
	XGrabServer (dpy);

	g_queue_foreach (mag_windows_list, gmag_compositor_remove_element,
			 None);

	XUngrabServer (dpy);
}

/*
 * Create the mag_windows_list querying the xserver for the actual
 * windows and adding them to the windows list.
 */
static void
gmag_compositor_create_windows_list ()
{
	Window  root_return, parent_return, *children;
	guint   nchildren;
	gint    i;

	if (!mag_windows_list)
		mag_windows_list = g_queue_new ();
	else
		gmag_compositor_destroy_windows_list ();

	XGrabServer (dpy);
	XSelectInput (dpy, root_window, SubstructureNotifyMask);
	XQueryTree (dpy, root_window, &root_return, &parent_return, &children,
		    &nchildren);
	for (i = 0; i < nchildren; i++)
		gmag_compositor_add_window (children[i]);
	XFree (children);
	XUngrabServer (dpy);
}

static void
gmag_compositor_circulate_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received CirculateNotify event: 0x%x\n",
		(guint) ev->xcirculate.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xcirculate.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xcirculate.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		g_queue_remove (mag_windows_list, pgmag_win);
		if (ev->xcirculate.place == PlaceOnTop) {
			g_queue_push_tail (mag_windows_list,
					   pgmag_win);
			if (pgmag_win->attr.map_state == IsViewable) {
				XFixesSubtractRegion (
					dpy,
					tmp_region, pgmag_win->win_region,
					pgmag_win->clip);
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region, tmp_region);
			}
		} else {
			g_queue_push_head (mag_windows_list,
					   pgmag_win);
			if (pgmag_win->attr.map_state == IsViewable)
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region,
					pgmag_win->clip);
		}
	}
}

static void
gmag_compositor_configure_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;
	
#ifdef COMPOSITE_DEBUG
	printf ("Received ConfigureNotify event: 0x%x\n",
		(guint) ev->xconfigure.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xconfigure.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xconfigure.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);

	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		if (((pgmag_win->attr.x != ev->xconfigure.x) ||
		     (pgmag_win->attr.y != ev->xconfigure.y)) &&
		    ((pgmag_win->attr.width == ev->xconfigure.width) &&
		     (pgmag_win->attr.height == ev->xconfigure.height) &&
		     (pgmag_win->attr.border_width ==
		      ev->xconfigure.border_width))) {
			int dx, dy;

			dx = ev->xconfigure.x - pgmag_win->attr.x;
			dy = ev->xconfigure.y - pgmag_win->attr.y;

			XFixesTranslateRegion (dpy, pgmag_win->win_region,
					       dx, dy);
		}

		if ((pgmag_win->attr.width != ev->xconfigure.width) ||
		    (pgmag_win->attr.height != ev->xconfigure.height) ||
		    (pgmag_win->attr.border_width !=
		     ev->xconfigure.border_width)) {
			XFixesDestroyRegion (dpy, pgmag_win->win_region);
			pgmag_win->win_region = XFixesCreateRegionFromWindow (
				dpy, pgmag_win->xwin, WindowRegionBounding);
			XFixesTranslateRegion (dpy, pgmag_win->win_region,
					       ev->xconfigure.x,
					       ev->xconfigure.y);
		}

		if ((pgmag_win->attr.x != ev->xconfigure.x) ||
		    (pgmag_win->attr.y != ev->xconfigure.y) ||
		    (pgmag_win->attr.width != ev->xconfigure.width) ||
		    (pgmag_win->attr.height != ev->xconfigure.height) ||
		    (pgmag_win->attr.border_width !=
		     ev->xconfigure.border_width)) {
			/* If an attribute of the window has changed we could
			 * have an exposed area that is not reported due to the
			 * overlay window. So we subtract the new region, from
			 * the old one, and we have the value of the exposed
			 * region that must be repainted.
			 */
			pgmag_win->attr.x = ev->xconfigure.x;
			pgmag_win->attr.y = ev->xconfigure.y;
			pgmag_win->attr.width = ev->xconfigure.width;
			pgmag_win->attr.height = ev->xconfigure.height;
			pgmag_win->attr.border_width =
				ev->xconfigure.border_width;
			      
			if (pgmag_win->attr.map_state == IsViewable) {
				XFixesCopyRegion (
					dpy,
					old_region, pgmag_win->clip);
				gmag_compositor_calculate_window_clip (
					pgmag_win);
				XFixesCopyRegion (
					dpy,
					new_region, pgmag_win->clip);
				XFixesSubtractRegion (dpy, old_region,
						      old_region, new_region);
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region, old_region);
				/*
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region, new_region);
				*/
			}
		}
		if (!ev->xconfigure.above) {
			g_queue_remove (mag_windows_list, pgmag_win);
			g_queue_push_head (mag_windows_list,
					   pgmag_win);
			if (pgmag_win->attr.map_state == IsViewable) {
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region,
					pgmag_win->win_region);
			}
		} else {
			elem = g_queue_find_custom (
				mag_windows_list,
				(gconstpointer) ev->xconfigure.above,
				(GCompareFunc) gmag_compositor_g_compare_func);
			if (elem) {
				g_queue_remove (mag_windows_list,
						pgmag_win);
				g_queue_insert_after (mag_windows_list,
						      elem, pgmag_win);
				if (pgmag_win->attr.map_state == IsViewable) {
					XFixesUnionRegion (
						dpy,
						exp_region, exp_region,
						pgmag_win->win_region);
				}
			}
		}
	}
}

static void
gmag_compositor_create_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received CreateNotify event: 0x%x\n",
		(guint) ev->xcreatewindow.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xcreatewindow.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	gmag_compositor_add_window (ev->xcreatewindow.window);
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xcreatewindow.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		if (pgmag_win->attr.map_state == IsViewable) {
			gmag_compositor_calculate_window_clip (pgmag_win);
			XFixesUnionRegion (dpy,
					   exp_region, exp_region,
					   pgmag_win->clip);
		}
	}
}

static void
gmag_compositor_destroy_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received DestroyNotify event: 0x%x\n",
		(guint) ev->xdestroywindow.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xdestroywindow.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xdestroywindow.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		if (pgmag_win->attr.map_state == IsViewable)
			XFixesUnionRegion (dpy,
					   exp_region, exp_region,
					   pgmag_win->clip);
		gmag_compositor_remove_window (ev->xdestroywindow.window);
	}
}

static void
gmag_compositor_map_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received MapNotify event: 0x%x\n",
		(guint) ev->xmap.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xmap.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xmap.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		pgmag_win->attr.map_state = IsViewable;
		gmag_compositor_calculate_window_clip (pgmag_win);
		XFixesUnionRegion (dpy, exp_region,
				   exp_region, pgmag_win->clip);
	}
}

static void
gmag_compositor_unmap_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received UnmapNotify event: 0x%x\n",
		(guint) ev->xunmap.window);
#endif /* COMPOSITE_DEBUG */
	if (ev->xunmap.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	elem = g_queue_find_custom (mag_windows_list,
				    (gconstpointer) ev->xunmap.window,
				    (GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		pgmag_win->attr.map_state = IsUnmapped;
		XFixesUnionRegion (dpy, exp_region,
				   exp_region, pgmag_win->clip);
	}
}

static void
gmag_compositor_reparent_notify_handler (XEvent *ev)
{
	GList      *elem;
	GmagWinPtr  pgmag_win;

#ifdef COMPOSITE_DEBUG
	printf ("Received ReparentNotify event: 0x%x (Window), 0x%x (Parent)\n", (guint) ev->xreparent.window, (guint) ev->xreparent.parent);
#endif /* COMPOSITE_DEBUG */
	if  (ev->xreparent.window == mag_window) {
#ifdef HAVE_OVERLAY
#ifdef COMPOSITE_DEBUG
		printf ("Overlay window = 0x%x\n",
			(guint) mag_window);
#endif /* COMPOSITE_DEBUG */
#endif /* HAVE_OVERLAY */
		return;
	}
	if (ev->xreparent.parent != root_window) {
		gmag_compositor_remove_window (ev->xreparent.window);
	} else {
		gmag_compositor_add_window (ev->xreparent.window);
		elem = g_queue_find_custom (
			mag_windows_list,
			(gconstpointer) ev->xreparent.window,
			(GCompareFunc) gmag_compositor_g_compare_func);
		if (elem) {
			pgmag_win = (GmagWinPtr) elem->data;
			if (pgmag_win->attr.map_state == IsViewable) {
			        gmag_compositor_calculate_window_clip (pgmag_win);
				XFixesUnionRegion (
					dpy,
					exp_region, exp_region,
					pgmag_win->clip);
			}
		}
	}
}

static void
gmag_compositor_damage_notify_handler (XEvent *ev)
{
	GList                *elem;
	GmagWinPtr            pgmag_win;
	XDamageNotifyEvent   *dev = (XDamageNotifyEvent *) ev;

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

#ifdef DAMAGE_DEBUG
	g_message ("Window with damage: 0x%x", (unsigned int) dev->drawable);
#endif /* DAMAGE_DEBUG */
	elem = g_queue_find_custom (
		mag_windows_list, (gconstpointer) dev->drawable,
		(GCompareFunc) gmag_compositor_g_compare_func);
	if (elem) {
		pgmag_win = (GmagWinPtr) elem->data;
		XDamageSubtract (dpy, dev->damage, None, tmp_region);
		XFixesTranslateRegion (dpy, tmp_region, pgmag_win->attr.x,
				       pgmag_win->attr.y);
		gmag_compositor_paint_window (pgmag_win, tmp_region);
	}
}

void
gmag_compositor_paint_screen ()
{
	gmag_compositor_calculate_windows_clip ();
	gmag_compositor_paint_windows (exp_region);

	XFixesSetRegion (dpy, tmp_region, 0, 0);
	XFixesSetRegion (dpy, new_region, 0, 0);
	XFixesSetRegion (dpy, old_region, 0, 0);
	XFixesSetRegion (dpy, exp_region, 0, 0);
}

gboolean
gmag_compositor_events_handler (Magnifier *magnifier, XEvent *ev)
{
	gboolean calc_clip = FALSE;
#ifdef HAVE_OVERLAY
	if (magnifier->priv->overlay)
		mag_window = GDK_WINDOW_XID (magnifier->priv->overlay);
#else
	if (magnifier->priv->w && magnifier->priv->w->window)
		mag_window = GDK_WINDOW_XID (magnifier->priv->w->window);
#endif /* HAVE_OVERLAY */
	
	switch (ev->type) {
	case CirculateNotify:
		gmag_compositor_circulate_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case ConfigureNotify:
		gmag_compositor_configure_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case CreateNotify:
		gmag_compositor_create_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case DestroyNotify:
		gmag_compositor_destroy_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case MapNotify:
		gmag_compositor_map_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case UnmapNotify:
		gmag_compositor_unmap_notify_handler (ev);
		calc_clip = TRUE;
		break;
	case ReparentNotify:
		gmag_compositor_reparent_notify_handler (ev);
		calc_clip = TRUE;
		break;
	}

	return calc_clip;
}

void
gmag_compositor_init (Magnifier *magnifier, Display *dpy_conn)
{
	gint                      event_base, error_base;
	GdkDisplay               *gdk_display_connection;
	GdkScreen                *gdkscr;
	gint                      scr = 0, root_w, root_h;
	XRenderPictureAttributes  pic_attr;
	XRenderPictFormat        *format;

	if (!XCompositeQueryExtension (dpy_conn, &event_base, &error_base)) {
		g_warning ("Composite extension not currently active.\n");
	} else if (g_getenv ("MAGNIFIER_IGNORE_COMPOSITE")) {
		g_warning ("Composite extension being ignored at user " \
			   "request.");
	} else {
		dpy = dpy_conn;
		mag_ref = magnifier;
		root_window = GDK_WINDOW_XWINDOW (mag_ref->priv->root);

		if (exp_region != None) {
			XFixesDestroyRegion (dpy, tmp_region);
			XFixesDestroyRegion (dpy, new_region);
			XFixesDestroyRegion (dpy, old_region);
			XFixesDestroyRegion (dpy, exp_region);
			tmp_region = None;
			new_region = None;
			old_region = None;
			exp_region = None;
		}

#ifndef HAVE_OVERLAY
		g_warning ("update composite to version 0.3 or higher to " \
			   "have overlay window support.\n");
#endif /* HAVE_OVERLAY */

		gdk_drawable_get_size (magnifier->priv->root, &root_w, 
				       &root_h);
		magnifier->priv->source_drawable = gdk_pixmap_new (
			magnifier->priv->root, root_w, root_h, -1);
		/* GDK uses it's own connection with X, so we must flush that
		 * to not receive a BadDrawable when creating a picture of this
		 * drawable below. */
		gdk_flush ();

		gdk_display_connection = gdk_drawable_get_display (
			magnifier->priv->root);
		gdkscr = gdk_display_get_default_screen (
			gdk_display_connection);
	    
		scr = GDK_SCREEN_XNUMBER (gdkscr);

		XCompositeRedirectSubwindows (dpy, root_window,
					      CompositeRedirectAutomatic);

		format = XRenderFindVisualFormat (
			dpy,
			DefaultVisual (dpy, scr));
		pic_attr.subwindow_mode = IncludeInferiors;
		off_screen_picture = XRenderCreatePicture (
			dpy,
			GDK_DRAWABLE_XID (magnifier->priv->source_drawable),
			format, CPSubwindowMode, &pic_attr);
		gmag_gs_set_damage_handler_func (
			gmag_compositor_damage_notify_handler);
		gmag_compositor_create_windows_list (gdk_display_connection,
						     gdkscr);
		gmag_compositor_calculate_windows_clip ();
		gmag_compositor_paint_screen ();

		tmp_region = XFixesCreateRegion (dpy, 0, 0);
		new_region = XFixesCreateRegion (dpy, 0, 0);
		old_region = XFixesCreateRegion (dpy, 0, 0);
		exp_region = XFixesCreateRegion (dpy, 0, 0);

		g_message ("added event source to composite connection");
	}
}

#endif /* HAVE_COMPOSITE */
