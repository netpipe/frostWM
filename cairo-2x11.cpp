/*
	Grabs a screenshot of the root window.

	Usage	: ./scr_tool <display> <output file>
	Example	: ./scr_tool :0 /path/to/output.png

	Author: S Bozdag <selcuk.bozdag@gmail.com>

gcc cairo-test.cpp -o test -lX11 -lcairo

*/

#include <assert.h>
#include <stdio.h>
#include <cairo/cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

int main(int argc, char** argv) {
    Display *disp;
    Window root;
    cairo_surface_t *surface;
    int scr;
    /* The only checkpoint only concerns about the number of parameters, see "Usage" */
    if( argc != 3) {
    	fprintf(stderr, "Wrong number of parameters given \n");
    	fprintf(stderr, "Usage: ./ahenk_import <display> <output file> \n");
    	return 1;
    }
    /* try to connect to display, exit if it's NULL */
    disp = XOpenDisplay( argv[1] );
    if( disp == NULL ){
    	fprintf(stderr, "Given display cannot be found, exiting: %s\n" , argv[1]);
    	return 1;
    }
    scr = DefaultScreen(disp);
    root = DefaultRootWindow(disp);
    /* get the root surface on given display */
    surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
                                                    DisplayWidth(disp, scr),
                                                    DisplayHeight(disp, scr));
    XGrabPointer(disp, root, False, ButtonPressMask, GrabModeAsync,
         GrabModeAsync, None, None, CurrentTime);

int button;
int x=-1,y=-1;
    XMapWindow(disp, root);
    XFlush(disp);
    XSelectInput(disp, root, ExposureMask|KeyPressMask|ButtonPressMask);
    while (1)  {
        XNextEvent(disp, &events);
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

    /* right now, the tool only outputs PNG images */
  //  cairo_surface_write_to_png( surface, argv[2] );
    /* free the memory*/
    cairo_surface_destroy(surface);
    /* return with success */
    return 0;
}
