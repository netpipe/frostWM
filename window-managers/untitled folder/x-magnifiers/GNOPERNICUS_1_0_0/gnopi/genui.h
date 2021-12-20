/* genui.h
 *
 * Copyright 2001 - 2005 Sun Microsystems, Inc.,
 * Copyright 2001 - 2005 BAUM Retec, A.G.
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

#ifndef __GENUI__
#define __GENUI__ 

#include <gnome.h>
#include "coreconf.h"

enum
{
    BRAILLE,
    MAGNIFIER,
    SPEECH,
    BRAILLE_MONITOR
};


gboolean	genui_load_general_settings_interface	(GtkWidget *parent_window);
gboolean	genui_value_add_to_widgets	(General 	*general_setting);
void		genui_set_sensitive_for 	(gint no, 
						gboolean val);
gboolean 	genui_run_brlmonitor ();
#endif
