//https://stackoverflow.com/questions/12465552/c-and-xlib-center-window
//XDisplayWidth(dpy, sreen); XDisplayHeight(dpy, sreen);
//getenv("DISPLAY")
//putenv( mypath ); 
//xwininfo -tree -root
//sts = system(command);
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

//#include "xinput.h"
#define HAVE_XI2

#include <ctype.h>
#include <string.h>

//gcc x11.cpp -o test -lX11 -lXi

int screen;
Display *display;
XSetWindowAttributes window_attributes;
Window mainwindow;
XEvent events;
    int root_x, root_y; //<--two
    unsigned int mask; //<--three


XDeviceInfo*
find_device_info(Display	*display,
		 char		*name,
		 Bool		only_extended)
{
    XDeviceInfo	*devices;
    XDeviceInfo *found = NULL;
    int		loop;
    int		num_devices;
    int		len = strlen(name);
    Bool	is_id = True;
    XID		id = (XID)-1;

    for(loop=0; loop<len; loop++) {
	if (!isdigit(name[loop])) {
	    is_id = False;
	    break;
	}
    }

    if (is_id) {
	id = atoi(name);
    }

    devices = XListInputDevices(display, &num_devices);

    for(loop=0; loop<num_devices; loop++) {
	if ((!only_extended || (devices[loop].use >= IsXExtensionDevice)) &&
	    ((!is_id && strcmp(devices[loop].name, name) == 0) ||
	     (is_id && devices[loop].id == id))) {
	    if (found) {
	        fprintf(stderr,
	                "Warning: There are multiple devices named '%s'.\n"
	                "To ensure the correct one is selected, please use "
	                "the device ID instead.\n\n", name);
		return NULL;
	    } else {
		found = &devices[loop];
	    }
	}
    }
    return found;
}

int main(int argc, char** argv) {
    display = XOpenDisplay(NULL);screen = DefaultScreen(display);
    window_attributes.background_pixel = XWhitePixel(display, screen);
    window_attributes.border_pixel = XBlackPixel(display, screen);
    window_attributes.win_gravity = SouthWestGravity;

    mainwindow = XCreateWindow(display,
                             RootWindow(display, screen),
                             1, 1,
                             600, 400,
                             0,
                             CopyFromParent,
                             InputOutput,
                             CopyFromParent,
                             CWBackPixel|CWBorderPixel,
                             &window_attributes
                            );


    XGrabPointer(display, mainwindow, False, ButtonPressMask, GrabModeAsync,
         GrabModeAsync, None, None, CurrentTime);

int button;
int x=-1,y=-1;
    XMapWindow(display, mainwindow);
    XFlush(display);
    XSelectInput(display, mainwindow, ExposureMask|KeyPressMask|ButtonPressMask);
    while (1)  {
        XNextEvent(display, &events);
        //switch  (events.type) {

 //   XQueryPointer(display, DefaultRootWindow(display), &mainwindow, &mainwindow, &root_x, &root_y, &root_x, &root_y, &mask); //<--four
   // printf("Mouse coordinates (X: %d, Y: %d)\n", root_x, root_y);

//printf("    device: %d (%d)\n", events.deviceid, events.sourceid);
	//XDeviceInfo* xi2_find_device_info(Display *display, char *name);

        XDeviceInfo *info;
        info = find_device_info(display,  "keyboard",1);

	if (info==true){
//printf("%d",info->id);
printf("%d",info->num_devices);
}
     //   printf("%d",info->id);
	//printf("%s\n", info->name);

       // XDeviceInfo *info;
       // info = xi2_find_device_info(display,"");


	switch(events.type){
	    case ButtonPress:
		switch(events.xbutton.button){
		    case Button1:
		        x=events.xbutton.x;
		        y=events.xbutton.y;
		        button=Button1;
			printf("button1\n");
		        break;

		    case Button3:
		        x=events.xbutton.x;
		        y=events.xbutton.y;
		        button=Button3;
			printf("button3\n");
		        break;
		    default:
		        break;
		}
	}

        if (events.type == KeyPress)
        {
            printf( "KeyPress: %x\n", events.xkey.keycode );
            if ( events.xkey.keycode == 0x09 )
                break;
        }
        else if (events.type == KeyRelease)
        {
            printf( "KeyRelease: %x\n", events.xkey.keycode );
        }
    }
    return 0;
}
