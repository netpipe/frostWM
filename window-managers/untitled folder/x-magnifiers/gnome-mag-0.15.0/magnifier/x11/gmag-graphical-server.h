/*
 * GNOME-MAG Magnification service for GNOME
 *
 * Copyright 2006 Carlos Eduardo Rodrigues Diógenes
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

#ifndef GMAG_GRAPHICAL_SERVER_H_
#define GMAG_GRAPHICAL_SERVER_H_

#include "magnifier.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#ifdef HAVE_COMPOSITE
#include <X11/extensions/Xrender.h>
#endif /* HAVE_COMPOSITE */

#include <gtk/gtk.h>

typedef void DamageHandlerFunc (XEvent *ev);

void       gmag_gs_client_init (Magnifier *magnifier);
gboolean   gmag_gs_source_has_damage_extension (Magnifier *magnifier);
void       gmag_gs_set_damage_handler_func (DamageHandlerFunc *func);
void       gmag_gs_notify_damage (Magnifier *magnifier, XRectangle *rect);
void       gmag_gs_magnifier_realize (GtkWidget *widget);
gboolean   gmag_gs_error_check (void);
gboolean   gmag_gs_reset_overlay_at_idle (gpointer data);
gboolean   gmag_gs_check_set_struts (Magnifier *magnifier);
gboolean   gmag_gs_use_compositor (Magnifier *magnifier);

#endif /* GMAG_GRAPHICAL_SERVER_H_ */
