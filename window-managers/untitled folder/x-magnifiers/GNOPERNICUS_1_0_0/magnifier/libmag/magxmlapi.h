/* magxmlapi.h
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

#ifndef _MAGXMLAPI_H
#define _MAGXMLAPI_H

#include "SREvent.h"
#define MagEventCB SROnEventProc

int  mag_xml_init      (MagEventCB mag_event_cb);
void mag_xml_terminate ();
void mag_xml_output    (char *buffer, 
			int   length);

#endif

