/***************************************************************************
 *   Copyright (C) 2004 by Sascha Hlusiak                                  *
 *   Spam84@gmx.de                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <math.h>

#include "joymouse.h"
#include "xkey.h"


const int JOYSTICK_DEADZONE=4000;

static char *fin="/dev/input/js0";
static char *fout="/dev/joymouse";
int verbose=0;
static int daemon_mode=0;
static int enabled_mouse=1,enabled_keys=1;
static unsigned char num_axis,num_buttons;

/**
 * Reads data from the joystick device and fills our JOYSTICKDATA struct
 *
 * Returns 1 in succss, 0 otherwise
 **/
int ReadJoyData(int d,struct JOYSTICKDATA *JoystickData)
{
	struct js_event js;
	if (read(d, &js, sizeof(struct js_event)) != sizeof(struct js_event)) return 0;

	switch(js.type & ~JS_EVENT_INIT) {
	case JS_EVENT_BUTTON:
		if (js.number<32)
			JoystickData->button[js.number].pressed=js.value;
		break;
	case JS_EVENT_AXIS:
		if (js.number<32)
		{
			JoystickData->axis[js.number].value=js.value;
			if (abs(js.value)<JOYSTICK_DEADZONE)
			{
				JoystickData->axis[js.number].value=0;
			}
			break;
		}
	}

	return 1;
}

/**
 * Sends mouse data to fd.
 * 
 * Returns 1 in success, 0 otherwise
 **/
int WriteMouseData(int fd,char x,char y,int zx,int zy,unsigned char buttons)
{	/* Write all the data in a certain format to the given file */
#if (1) /* This is ImPS/2 or ExplorerPS/2, not quite sure :-S */
	char data[4];
	data[0]=(0x08)|(buttons&0x07)|((x<0)?0x10:0)|((y<0)?0x20:0);
	data[1]=x;
	data[2]=y;
	data[3]=0x00;
	if (zx)data[3]|=(zx<0)?2:-2;
	if (zy)data[3]|=(zy<0)?1:-1;
#endif

	if (!enabled_mouse)return 1;
	if (write(fd,data,sizeof(data))!=sizeof(data))return 0;
	
	return 1;
}

/**
 * Opens the joystick device (fin) for reading.
 * 
 * Returns the filedescriptor
 **/
int OpenInput()
{
	int fd;

	fd=open(fin,O_RDONLY | O_NONBLOCK);
        if (fd<0)
	{
		/* In case of error, die */
		perror(fin);
		exit(1);
	}
	
	if (verbose)
	{
		int version;
		char joy_name[128];

		printf("Open input device: %s\n",fin);	
		ioctl(fd, JSIOCGVERSION, &version);
		ioctl(fd, JSIOCGAXES, &num_axis);
		ioctl(fd, JSIOCGBUTTONS, &num_buttons);
		ioctl(fd, JSIOCGNAME(128), joy_name);

		printf("joystick detected: %s\n",joy_name);
		printf("axes: %d, buttons: %d, driver: %d.%d.%d\n\n",num_axis, num_buttons, version >> 16,(version >> 8) & 0xff, version & 0xff);
	}
	
	return (fd);
}

/**
 * Display usage and die
 **/
void Help()
{
	const char* message=
		"Usage: joymouse [options]\n"
		"	-h --help		display this message\n"
		"	   --version		display version and copyright.\n"
		"	-v -vv			verbose, more verbose\n"
		"	-i device		joystick input device\n"
		"	-o pipe			output pipe\n"
		"	-d			run in daemon mode\n"
		"	-t [type]		type of the axis\n"
		"				types: joypad, joystick\n"
		"				(use several times for each axis)\n"
		"	-M [mapping]		mapping of the axis\n"
		"				one of: -, x, y, zx, zy\n"
		"				(use several times for each axis)\n"
		"	-B [mapping]		mapping of the button\n"
		"				one of: -, [0-32], x+, x-,\n"
		"				y+, y-, zx+, zx-, zy+, zy-\n"
		"				m=[factor]\n"
		"				disable, disable-mouse, diable-keys\n"
#ifdef HAVE_LIBX11
		"				key=[key]\n"
#endif
		"				(use several times for each button)\n"
		"\n"
		"Default:  'joymouse -i /dev/js0 -o /dev/joymouse -t joystick -Mx -t joystick -My \\\n"
		"		-t joypad -M zx -t joypad -M zy -B0 -B1 -B2 -B3 -B4 -B5 -B6 ...'\n"
		"Output pipe points to a pipe file. The program attempts to create it.\n"
		"Create one yourself using the 'mkfifo' command.\n\n"
		"The program reads joystick data and sends mouse data (in ExplorerPS2 format).\n Use as you wish.\n";	
	
	printf(message);
	exit(0);
}

/**
 * Display version and die
 **/
void Version()
{
	const char* message=
		PACKAGE" "VERSION"\n"
		"Written by Sascha Hlusiak, 2006.\n"
		"Distributed unter the GPL.\n";
		
	printf(message);
	exit(0);
}

/**
 * Parses the parameter line and fills our JOYSTICKDATA struct with the config values
 **/
void ParseCmdline(int argc,char *argv[],struct JOYSTICKDATA* j)
{
	int opt;
	/* Counts the index of the current item to set */
	int axistype=0,axismapping=0,button=0;
	static char buf[512];

	while ((opt = getopt(argc, argv,"hvdi:o:t:M:B:")) != -1)
	{
         	switch (opt) 
	 	{
		 	case 'i': fin=(char*)strdup(optarg); break;
			case 'o': fout=(char*)strdup(optarg); break;
			case 'v': verbose++; break;
			case 'h': Help(); break;
			case 'd': daemon_mode=1; break;
			case 't': {
					enum JOYSTICKTYPE type_joystick=TYPE_NONE;
					if (strcmp("joypad",optarg)==0)type_joystick=TYPE_ACCELERATED;
					if (strcmp("joystick",optarg)==0)type_joystick=TYPE_BYVALUE;
					
					if (type_joystick==TYPE_NONE)
					{
						printf("Unrecognized joystick type given - %s\n",optarg);
						exit(1);
					}
					j->axis[axistype++].type=type_joystick;
					break;
				}
			case 'M': {
					enum MAPPING map=MAPPING_NONE;
					if (strcmp("x",optarg)==0)map=MAPPING_X;
					if (strcmp("y",optarg)==0)map=MAPPING_Y;
					if (strcmp("zx",optarg)==0)map=MAPPING_ZX;
					if (strcmp("zy",optarg)==0)map=MAPPING_ZY;
		
					if (map==MAPPING_NONE && strcmp("-",optarg)!=0)
					{
						printf("Unrecognized mapping for axis given - %s\n",optarg);
						exit(1);
					}
					j->axis[axismapping++].mapping=map;
					break;
				}
			case 'B': {
					enum MAPPING map=MAPPING_NONE;
					int v=0;
					float f;
					if (strcmp("x+",optarg)==0)map=MAPPING_X, v=1;
					if (strcmp("x-",optarg)==0)map=MAPPING_X, v=-1;
					if (strcmp("y+",optarg)==0)map=MAPPING_Y, v=1;
					if (strcmp("y-",optarg)==0)map=MAPPING_Y, v=-1;
					if (strcmp("zx+",optarg)==0)map=MAPPING_ZX, v=1;
					if (strcmp("zx-",optarg)==0)map=MAPPING_ZX, v=-1;
					if (strcmp("zy+",optarg)==0)map=MAPPING_ZY, v=1;
					if (strcmp("zy-",optarg)==0)map=MAPPING_ZY, v=-1;
					if (strcmp("disable",optarg)==0)map=MAPPING_DISABLE;
					if (strcmp("disable-keys",optarg)==0)map=MAPPING_DISABLE_KEYS;
					if (strcmp("disable-mouse",optarg)==0)map=MAPPING_DISABLE_MOUSE;

					if (map==MAPPING_NONE && sscanf(optarg,"m=%f",&f)==1)
					{
						v=(int)(f*100.0f);
						if (v>0) map=MAPPING_SPEED_MULTIPLY;
					}
					if (map==MAPPING_NONE && sscanf(optarg,"key=%100s",buf)==1)
					{
#ifdef HAVE_LIBX11
						v=(int)GetKeyCode(buf);
						if (v==NoSymbol)
						{
							printf("Unrecognized key mapping ignored: %s\n",buf);
						}else map=MAPPING_KEY;
#else
						printf("%s ignored, because built without X support!\n",optarg);
#endif
					}
		
					if (map==MAPPING_NONE && strcmp("-",optarg)!=0)
					{
						v=atoi(optarg);
						map=MAPPING_BUTTON;
						if (strcmp("0",optarg)!=0 && v==0)
						{
							printf("Unrecognized or malformed mapping for button given - %s\n",optarg);
							exit(1);
						}
					}
					j->button[button].mapping=map;
					j->button[button++].value=v;
					break;
				}
			default: printf("Not implemented yet...\n"); exit(1);
         	}
   	}
	if (axistype==1)
	{
		if (verbose)printf("Setting type of second axis to type of first.\n");
		/* If specified only one axis, set second axis to the same */
		j->axis[1].type=j->axis[0].type;
	}
}

/**
 * Transformes pressed buttons of joystick to mouse buttons to simulate with taking button mapping
 * into account.
 *
 * Returns bitfield with mouse-buttons (LSB is first button, etc)
 **/
int GetMousebuttonBitfield(struct JOYSTICKDATA *data)
{
	int i;
	int field=0;
	for (i=0;i<32;i++)
		if ((data->button[i].mapping==MAPPING_BUTTON)&&(data->button[i].pressed))
		field|=(1<<data->button[i].value);
	return field;
}

/**
 * Returns an int, where each bit is 1, when the button is down and has >mapping<
 **/
int GetPressedButtons(struct JOYSTICKDATA *data)
{
	int i;
	int field=0;
	for (i=0;i<32;i++)
		if (data->button[i].pressed)
			field|=(1<<i);
	return field;
}

/**
 * Handles state change of a joystick button.
 * Called once, when a button is pressed down, or released.
 **/
void HandleJoystickButton(struct BUTTON* button,int fdout,struct JOYSTICKDATA *JoystickData)
{
	switch(button->mapping)
	{
	case MAPPING_BUTTON:
		if (WriteMouseData(fdout,0,0,0,0,GetMousebuttonBitfield(JoystickData))==0)
		{
			if (verbose)perror(PACKAGE" writing mouse data");
			return;
		}
		break;
#ifdef HAVE_LIBX11
	case MAPPING_KEY:
		if (enabled_keys){
			SendKey(button->value,button->pressed);
			if (verbose>=2)printf("%s Keycode %d\n",(button->pressed)?"Pressing":"Releasing",button->value);
		}
		break;
#endif
	case MAPPING_DISABLE:if (button->pressed)
		{
			enabled_mouse=1-enabled_mouse;
			enabled_keys=enabled_mouse;
			if (verbose)printf("Joymouse temporarily %s\n",enabled_mouse?"enabled":"disabled");
		}
		break;
	case MAPPING_DISABLE_KEYS:if (button->pressed)
		{
			enabled_keys=1-enabled_keys;
			if (verbose)printf("Sending keys temporarily %s\n",enabled_keys?"enabled":"disabled");
		}
		break;
	case MAPPING_DISABLE_MOUSE:if (button->pressed)
		{
			enabled_mouse=1-enabled_mouse;
			if (verbose)printf("Sending mouse events temporarily %s\n",enabled_mouse?"enabled":"disabled");
		}
		break;

	default: break;
	}
}


/**
 * Processes events, reads joystick data and writes mouse data
 *
 * Function returns, when the pipe has been closed, or there was an error
 **/
void Loop(int fdin,int fdout,struct JOYSTICKDATA *JoystickData)
{
	const float startmultiplier=25.0f;
	const float mintime=1.1f;	/* in ms */
	const float exponent=0.96f;
	float multiplier=startmultiplier,speedmultiplier;
	int x,y,zx,zy,buttons,adjustmultiplier;
	int i,sleeptime;
	
	buttons=0;
	while (1)
	{
		while (ReadJoyData(fdin,JoystickData)==1)
		{	/* Write any changes to pressed buttons to pipe immediately */
		 	if (buttons!=GetPressedButtons(JoystickData))
			{
				buttons^=GetPressedButtons(JoystickData);
				for (i=0;i<32;i++)if ((buttons & (1<<i)))
					HandleJoystickButton(&JoystickData->button[i],fdout,JoystickData);
				buttons=GetPressedButtons(JoystickData);
			}
			if (verbose>=2)
			{
				int i;
				for (i=0;i<num_axis;i++)
					printf("axis[%d]: %d\t",i,JoystickData->axis[i].value);
				printf("buttons: ");
				for (i=0;i<num_buttons;i++)if (JoystickData->button[i].pressed)
					printf("%d ",i);
				printf("\n");
			}
			
		}
		x=y=zx=zy=0;
		adjustmultiplier=0;
		speedmultiplier=1.0;

		/* Calculate effect of the axes */
		for (i=0;i<32;i++)if (JoystickData->axis[i].value!=0 && JoystickData->axis[i].mapping!=MAPPING_NONE)
		{
			struct AXIS *a=&JoystickData->axis[i];
			switch(a->type)
			{
				case TYPE_BYVALUE:
				{
					#define speed(x) ((x<0)?-1:1)*(char)(pow(abs(x)/5000.0f,1.4f))

					sleeptime=29000;
					if (a->mapping==MAPPING_X)x+=speed(a->value);
					if (a->mapping==MAPPING_Y)y-=speed(a->value);
					if (a->mapping==MAPPING_ZX)
					{
						zx-=speed(a->value);
						sleeptime=240000-5*abs(a->value);
					}
					if (a->mapping==MAPPING_ZY)
					{
						zy-=speed(a->value);
						sleeptime=240000-5*abs(a->value);
					}

					#undef speed
					break;
				}
				case TYPE_ACCELERATED:
				{
					int speed=((multiplier<1.4f)?3:(multiplier<8.0)?2:1);
					sleeptime=(int)(1000.0f*mintime*multiplier*(float)(speed));

					if (a->mapping==MAPPING_X && a->value<0)x-=speed;
					if (a->mapping==MAPPING_X && a->value>0)x+=speed;
					if (a->mapping==MAPPING_Y && a->value<0)y+=speed;
					if (a->mapping==MAPPING_Y && a->value>0)y-=speed;
					
					if (a->mapping==MAPPING_ZX)
					{
						zx=-a->value;
						sleeptime=(int)(1000.0f*6.0*mintime*multiplier);
					}
					if (a->mapping==MAPPING_ZY)
					{
						zy=-a->value;
						sleeptime=(int)(1000.0f*6.0*mintime*multiplier);
					}
						

					adjustmultiplier=1;
					break;
				}
				default: break;
			}
		}

		/* Calculating effect of pressed buttons */
		for (i=0;i<32;i++)if (JoystickData->button[i].pressed)
		{
			struct BUTTON *b=&JoystickData->button[i];
			if (b->mapping==MAPPING_NONE || b->mapping==MAPPING_BUTTON || b->mapping==MAPPING_KEY)continue;

			if (b->mapping==MAPPING_SPEED_MULTIPLY)
			{
				speedmultiplier*=(((float)b->value)/100.0f);
				continue;
			}

			int speed=b->value*((multiplier<1.4f)?3:(multiplier<8.0)?2:1);
			sleeptime=(int)(1000.0f*mintime*multiplier*(float)(abs(speed)));

			if (b->mapping==MAPPING_X)x+=speed;
			if (b->mapping==MAPPING_Y)y-=speed;
			
			if (b->mapping==MAPPING_ZX)
			{
				zx=-b->value;
				sleeptime=(int)(1000.0f*6.0*mintime*multiplier);
			}
			if (b->mapping==MAPPING_ZY)
			{
				zy=-b->value;
				sleeptime=(int)(1000.0f*6.0*mintime*multiplier);
			}
			adjustmultiplier=1;
		}

		/* Send data, if there is something to send */
		if ((x || y || zx || zy)&&(enabled_mouse))
		{
			if (adjustmultiplier)multiplier=powf(multiplier,exponent);
			if (x || y)if (WriteMouseData(fdout,x,y,0,0,GetMousebuttonBitfield(JoystickData))==0)return;
			if (zx) if(WriteMouseData(fdout,0,0,zx,0,GetMousebuttonBitfield(JoystickData))==0)return;
			if (zy) if(WriteMouseData(fdout,0,0,0,zy,GetMousebuttonBitfield(JoystickData))==0)return;
			sleeptime=(int)((float)sleeptime / speedmultiplier);
			usleep(sleeptime);
		}else{	/* Nothing to do, world stands still. Sleep for a great event */
			multiplier=startmultiplier;
			fd_set fdsread;
		        FD_ZERO(&fdsread);
           		FD_SET(fdin,&fdsread);
			
			select(FD_SETSIZE,&fdsread,NULL,NULL,NULL);
		}
	};
}

/**
 * Setups default mappings for joymouse
 **/
void SetupDefaultMappings(struct JOYSTICKDATA *j)
{
	int i;
	memset(j,0,sizeof(struct JOYSTICKDATA));

	j->axis[0].type=TYPE_BYVALUE;
	j->axis[0].mapping=MAPPING_X;
	j->axis[1].type=TYPE_BYVALUE;
	j->axis[1].mapping=MAPPING_Y;

	j->axis[2].mapping=MAPPING_ZX;
	j->axis[2].type=TYPE_ACCELERATED;
	j->axis[3].mapping=MAPPING_ZY;
	j->axis[3].type=TYPE_ACCELERATED;

	for (i=0;i<32;i++)
	{
		j->button[i].value=i;
		j->button[i].mapping=MAPPING_BUTTON;
	}
}

/**
 * MAIN
 **/
int main(int argc, char *argv[])
{
	int i;		/* temporary variable */
	int fdin,fdout; /* Filedescriptors for input and output device */
	struct JOYSTICKDATA JoystickData;

	/* Display help screen on demand */
	for (i=0;i<argc;i++)
	{	
		if (strcmp(argv[i],"--version")==0)Version();
		if (strcmp(argv[i],"--help")==0)Help();
	}

#ifdef HAVE_LIBX11
	InitX11();
#endif
	SetupDefaultMappings(&JoystickData);

	/* Parse commandline and store parameters */
	ParseCmdline(argc,argv,&JoystickData);
	

	
	if (daemon_mode)
	{	/* Fork into background */
		if (verbose)printf("WARNING: Verbose Daemon mode is evil!\n");
		
		pid_t pid;

		if ((pid = fork()) < 0) 
		{
			perror(PACKAGE" fork");
			exit(1);
		}

		if (pid != 0) 
		{
			printf("Entering daemon mode: pid=%i\n",pid);
			exit(0);
		}
	}
	
	
	if (verbose)
	{
		printf("Input:  %s\n",fin);
		printf("Output: %s\n",fout);
	}

	/* Disable Brokenpipe signal to keep on working, when the pipe client quits */
	signal(SIGPIPE,SIG_IGN);
	
	/* Create our pipe node (Parameter: -o) */
	if (verbose)printf("Creating pipe: %s\n",fout);
	int e=mkfifo(fout,0644);
	if (e!=0 && verbose>=2) perror(PACKAGE" creating pipe");
	
	fdin=OpenInput();
	
	if (!daemon_mode)printf("I'm happy, starting processing-loop now...\n");
	do
	{
		if (verbose)printf("Wait for pipe for writing...\n");
		fdout=open(fout,O_RDWR|O_SYNC);
	
        	if (fdout<0)
		{	/* Output pipe could not be opened */
			perror(PACKAGE" opening pipe for writing");
			exit(1);
		}
		
		Loop(fdin,fdout,&JoystickData);

		/* When returned, reopen pipe and restart processing-loop. */
		if (verbose)perror(PACKAGE);
		if (close(fdout)!=0)if (verbose)perror(PACKAGE" close output: ");
	}while(1);	/* To infinity, and beyond */
			
	/* Nice code to clean up. But in fact it is never called. What a pity. */
	if (close(fdin)!=0)perror(PACKAGE" closing device");
	if (close(fdout)!=0)perror(PACKAGE" closing pipe");
	return EXIT_SUCCESS;
}
