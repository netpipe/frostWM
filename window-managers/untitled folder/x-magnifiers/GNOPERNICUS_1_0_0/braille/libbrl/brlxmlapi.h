/* brlxmlapi.h
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

#ifndef __BRLXMLAPI_H__
#define __BRLXMLAPI_H__

#include <glib.h>

typedef void (*BRLXmlInputProc) (gchar *buffer,
				 gint   len);

gint  brl_xml_init     (gchar          *device_name,
		       gchar           *port,
		       BRLXmlInputProc input_callback_proc);
void brl_xml_terminate (void);
void brl_xml_output    (gchar          *buffer,
                       gint            len);

#endif
