/* mag_conf.c
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

#include <stdio.h>
#include <string.h>

#include "magxmlapi.h"
#include "SREvent.h"
#include "libsrconf.h"
#include "mag.h"

void mag_cb_fnc(SREvent *event, unsigned long data)
{
  gchar *str = NULL;
  switch (event->type)
  { 
  case SR_EVENT_CONFIG_CHANGED:
	switch(((SRConfigStructure *)(event->data))->module)
	{
	case CFGM_MAGNIFIER:
	    mag_config_changed(event->data);	
	    break;
	default :
	    break;
	}
	break;
  case SR_EVENT_KEYBOARD_ECHO:
	str = (char *)event->data;
	printf("\"%s\"\n",str);
	if((strcmp(str,"KYU") == 0) || (strcmp(str,"q") == 0) || (strcmp(str,"Q") == 0))
	{
	    sru_exit_loop();
	}
	break;
  default :
    break;    
  }
}

int
main(int argc, char **argv)
{
  sru_init();

  srconf_init( (SRConfCB)mag_cb_fnc, NULL);
  
  mag_xml_init ((MagEventCB)mag_cb_fnc);

  sru_entry_loop ();
  
  mag_xml_terminate ();
  
  srconf_terminate ();
  
  sru_terminate ();
  
  return 0;
}
