/* srs-xml.h
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

#ifndef __SRS_XML_H__
#define __SRS_XML_H__

#include <glib.h>

typedef void (*SRSCallback) 	 (gchar* buffer, gint len);
typedef void (*SRSResetCallback) ();

gboolean  srs_init      (SRSCallback callback, SRSResetCallback reset_callback);
void 	  srs_terminate ();
gboolean  srs_output    (gchar* buffer, gint len);

#endif /* __SRS_XML_H__ */

