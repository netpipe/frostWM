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


int screen;
Display *display;
XSetWindowAttributes window_attributes;
Window mainwindow;
XEvent events;
    int root_x, root_y; //<--two
    unsigned int mask; //<--three

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
