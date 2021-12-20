/* ecobrl.c
 *
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
#include <string.h>

#include "ecobrl.h"
#include "sercomm.h"


#define MAX_GET_BUFFER 9 
#define MAX_BUFFER 90 
#define MAX_KEYCODE_LENGHT 30
#define NUM_TECLAS_DISPLAY 5

/* Supported ECO Devices */
typedef enum
{
    ECODT_GENERIC,
    ECODT_20,
    ECODT_40,
    ECODT_80,
} ECODeviceType;

/* Device data type */
typedef struct
{
  guchar display_keys[32];
  guchar front_keys[32];
  guchar sensor_codes[32];
  gulong pressed_front_keys;
  gulong pressed_display_keys;
} ECODeviceData;


typedef enum
{
    BEGINFIRST=0,
    BEGINSECOND,
    KEY_PRESSED
} FrameType;



static guchar teclas[] ={0x01, 0x02, 0x04, 0x08, 0x10 };
 
static BRLDevCallback		client_callback = NULL;
static ECODeviceType		eco_dev_type = ECODT_80;
static ECODeviceData		ecodd;
static guchar                   old_sensor_code=0;
static guchar                   old_display_code=0;
static guchar                   old_front_code=0;
static guchar                   getbuffer[MAX_GET_BUFFER];
static gshort                   gb_index = 0;
static FrameType                frame_type=BEGINFIRST;

static guint indice=0;


/* Display keys, in eco devices, are the 5 keys nearest to the user to
make scroll.*/
void 
eco_on_display_changed (guchar display_code)
{
    BRLEventCode bec; 
    BRLEventData bed; 
    guint i;
  
    /* If no key pressed, then send the last situation */
    if (display_code == 0)
    {  
	bec = BRL_EVCODE_KEY_CODES;
        bed.key_codes = ecodd.display_keys;
        client_callback (bec, &bed);

        for (i = 0; i < MAX_KEYCODE_LENGHT; i++)
    	    ecodd.display_keys[i] = '\0';
        indice = 0;
    }
    else
    {
	/* We have to include something else to the string of DK's */
        for (i = 0; i < NUM_TECLAS_DISPLAY; i++)
        {
    	    if (display_code & teclas[i])
            {
        	indice += sprintf (&ecodd.display_keys[indice], "DK%02d", i);
            }
        }
    }

    old_display_code=display_code;
}


/* There are 12 front keys in the eco device named from F0 to F9 and
   keys A and S. This function must to be developed in depth. Now only
   send F1 to F5 keys */
void 
eco_on_front_changed (guchar front_code)
{
    BRLEventCode bec;
    BRLEventData bed;
    guint i;
  
    if (front_code==0)
    {
	bec = BRL_EVCODE_KEY_CODES;
        bed.key_codes = ecodd.front_keys;
        client_callback (bec, &bed);

        for (i = 0; i < MAX_KEYCODE_LENGHT; i++)
    	    ecodd.front_keys[i] = '\0';
        indice=0;
    }
    else
    {
	for (i = 0; i < NUM_TECLAS_DISPLAY; i++)
        {
    	    if (front_code & teclas[i] )
            {
        	indice += sprintf (&ecodd.front_keys[indice], "FK%02d", i );
            }
        }
    }

  old_front_code=front_code;
}


/* Sensor keys are the ones up in the braille cell */
void 
eco_on_sensors_changed (guchar sensor_code)
{
    BRLEventCode bec;
    BRLEventData bed;

    /* If there was some cell pressed, we send it*/
    if (old_sensor_code & 0x80)
    {
        old_sensor_code &= ~0x80;

        /* Status cell are named 0x50, 0x51, 0x55 and 0x56*/
        if (!( (old_sensor_code >= 0x50) && (old_sensor_code <= 0x56)))
        {
    	    /*If is not a status cell, we send a sensor pressed event*/
            sprintf (&ecodd.sensor_codes[0], "HMS%02d", old_sensor_code);
          
            bec = BRL_EVCODE_SENSOR;
            bed.sensor.sensor_codes = ecodd.sensor_codes;
            client_callback (bec, &bed);
        }
    }

    old_sensor_code=sensor_code;
}


/* Check is frame is complete */
gshort 
is_complete_frame_eco()
{
    static gint countdown = -1;
  
    if (countdown == 0)  /* frame complete */
    {
	countdown = -1;

        /*If last bytes are 0x10 0x03 then it's a correct frame*/
        if ((getbuffer[gb_index-1] == 0x03) && 
            (getbuffer[gb_index-2] == 0x10))
        {
    	    return 1;
        }
        else
        {
    	    /* There is something strange with this frame. We forget it. */
            fprintf (stderr, "\nStrange frame...");
            gb_index = 0;
        }
    }
    else
    {
	if (gb_index==3)
        {
    	    switch (getbuffer[2])
            {
        	case 0xf1:
            	    frame_type=BEGINFIRST;
                break;
        	case 0x2e:
            	    frame_type=BEGINSECOND;
                break;
        	case 0x88:
            	    frame_type=KEY_PRESSED;
            	break;
        	default:
            	    fprintf (stderr, "\n Frame not in switch. Imposible?\n");
              
            }
             /* Only left 5 bytes are read */
            countdown = 5; 
        }      
        else if (countdown >= 0)  /* If there is a size for the frame, only decrease it */
        {
    	    countdown--;
        }
    }
    return 0;
}


gshort
eco_brl_input_parser (gint new_val)
{
    static BRLEventCode	bec;
    static BRLEventData	bed;
    guchar sendbuffer[10];
    gint n;
  
    if (!client_callback)
	return 0;	  /* if we don't have a client callback doesn't make sense to go further */
  
    /* raw byte callback */
    bec = BRL_EVCODE_RAW_BYTE;
    bed.raw_byte = new_val;
    client_callback (bec, &bed);
  
    if (gb_index == 0)
	memset (getbuffer, 0x00, MAX_GET_BUFFER);
  
    getbuffer[gb_index++] = new_val;

    if (((gb_index == 1) && (new_val != 0x10)) || 
        ((gb_index == 2) && (new_val != 0x02)))
	gb_index = 0; /* It's a bad beginning for a frame */

    if (is_complete_frame_eco())
    {
	switch (frame_type)
        {
    	    case BEGINFIRST:
        	/* I send my answer to the begining protocol */
        	strcpy (sendbuffer,"\x61\x10\x02\xF1\x57\x57\x57\x10\x03");
        	n = brl_ser_send_data (sendbuffer, 9 , 1);
            break;
            case BEGINSECOND:
        	/*Eco device it's ready to be used */
            break;
            case KEY_PRESSED:
        	if (getbuffer[3] != old_sensor_code )
        	    eco_on_sensors_changed (getbuffer[3]);
        	if (getbuffer[4] != old_display_code)
            	    eco_on_display_changed(getbuffer[4]);
        	if (getbuffer[6] != old_front_code)
        	    eco_on_front_changed(getbuffer[6]);
            break;
            default:
            break;
        }
        gb_index=0;
    }

    return 0; /* 0-give next byte, 1-repeat last byte */
}


gint 
eco_brl_send_dots (guchar *dots, 
                   gshort count, 
		   gshort blocking)
{
    gint rv = 0;
    gint i, j;
    guchar sendbuffer[MAX_BUFFER];
    guint8 old_mask []={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    guint8 new_mask []={0x10, 0x20, 0x40, 0x01, 0x02, 0x04, 0x80, 0x08};
    guint8 new_dots;

    /*Clean the buffer*/
    memset (sendbuffer, 0x00, MAX_BUFFER);

    /* Write begin of the frame to send */
    sendbuffer[0] = 0x10;
    sendbuffer[1] = 0x02;
    sendbuffer[2] = 0xbc;

    for(i = 0; i<count; i++)
    {
        new_dots = 0;
        for(j = 0; j < 8; j++)
        {
    	    if ((dots[i] & old_mask[j] ))
    	    {
    		new_dots |= new_mask[j];
            }
        }
        dots[i]=new_dots;
    }
    
    memcpy (&sendbuffer[3], dots, count);

    /*Write the end of the frame*/
    sendbuffer[count+3] = 0x10;
    sendbuffer[count+4] = 0x03;

    /* We send to the device */
    rv = brl_ser_send_data (sendbuffer, count+5, blocking);

    return rv;
}


void 
eco_brl_close_device ()
{
    /* close serial communication */
    brl_ser_set_callback (NULL);
    brl_ser_stop_timer ();
    brl_ser_close_port ();
}

gint 
eco_brl_open_device (gchar          *device_name,
                     gchar          *port,
                     BRLDevCallback device_callback,
                     BRLDevice      *device)
{
    gint rv = 0;
  
    if (strcmp ("ECO20", device_name) == 0)
    {
	device->cell_count = 25;
        device->input_type = BRL_INP_MAKE_CODES;
        device->key_count = 5;
      
        device->display_count = 2;
      
        device->displays[0].start_cell = 5;
        device->displays[0].width = 21;
        device->displays[0].type = BRL_DISP_MAIN;
      
        device->displays[1].start_cell = 0;
        device->displays[1].width = 4;
        device->displays[1].type = BRL_DISP_STATUS;
      
        eco_dev_type = ECODT_20;
      
        rv = 1;
    }
  
    else if (strcmp ("ECO40", device_name) == 0)
    {
        device->cell_count = 45;
        device->input_type = BRL_INP_MAKE_BREAK_CODES;
        device->key_count = 5;
      
        device->display_count = 2;
      
        device->displays[0].start_cell = 5;
        device->displays[0].width = 41;
        device->displays[0].type = BRL_DISP_MAIN;
      
        device->displays[1].start_cell = 0;
        device->displays[1].width = 4;
        device->displays[1].type = BRL_DISP_STATUS;
      
        eco_dev_type = ECODT_40;
      
        rv = 1;
    }
  
    else if (strcmp ("ECO80", device_name) == 0)
    {
        device->cell_count = 85;
        device->input_type = BRL_INP_BITS;
        device->key_count = 16;
      
        device->display_count = 2;
      
        device->displays[0].start_cell = 5;
        device->displays[0].width = 81;
        device->displays[0].type = BRL_DISP_MAIN;
      
        device->displays[1].start_cell = 0;
        device->displays[1].width = 4;
        device->displays[1].type = BRL_DISP_STATUS;
      
        eco_dev_type = ECODT_80;
      
        rv = 1;
    }
  
    else
    {
	/* unknown device */
        eco_dev_type = ECODT_GENERIC;
    }
  
    if (rv)
    {
        /* fill device functions for the upper level */
        device->send_dots = eco_brl_send_dots;
        device->close_device = eco_brl_close_device;
      
        /* open serial communication */
        if (brl_ser_open_port (port))
        {
            brl_ser_set_callback (eco_brl_input_parser);
            rv = eco_set_comm_param ();
            brl_ser_init_glib_poll ();
            client_callback = device_callback;
        }
        else
        {
            /* We can not open the device port */
            rv = 0;
        }
    }
  
    return rv;
}
