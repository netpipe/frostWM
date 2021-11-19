
/* Save as: 'xshape_example.c'
 * Compiling: 
 * gcc -Wall -g xshape_example.c -o xshape_example -lX11 -lXext
 */

/* This program *attempts* to create a "transparent" window (using
 * the XShape extension) and draw four white (opaque) squares in
 * each corner of the window
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

/* size of the window */
#define W_WIDTH 640
#define W_HEIGHT 480

/* size of the four rectangles that will be drawn in the window */
#define R_WIDTH 80
#define R_HEIGHT 60

Display *dpy;
Window w;

/* convenience variables */
int BLACK_PIXEL;
int WHITE_PIXEL;

/* the four rectangles that will be drawn: one in each corner of the
 * window */
XRectangle rectangles[4] =
{
    { 0, 0, R_WIDTH, R_HEIGHT },
    { 0, W_HEIGHT-R_HEIGHT, R_WIDTH, R_HEIGHT },
    { W_WIDTH-R_WIDTH, W_HEIGHT-R_HEIGHT, R_WIDTH, R_HEIGHT },
    { W_WIDTH-R_WIDTH, 0, R_WIDTH, R_HEIGHT }
};
        
int main(int argc, char **argv)
{
    XGCValues shape_xgcv;
    Pixmap pmap;
    GC shape_gc;
    GC gc;
    XGCValues gcv;
    int run = 1; /* loop control variable */

    /* open the display */
    if(!(dpy = XOpenDisplay(getenv("DISPLAY")))) {
        fprintf(stderr, "can't open display\n");
        return EXIT_FAILURE;
    }

    /* convenience */
    BLACK_PIXEL = BlackPixel(dpy, DefaultScreen(dpy));
    WHITE_PIXEL = WhitePixel(dpy, DefaultScreen(dpy));

    w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0,
            W_WIDTH, W_HEIGHT, 0, CopyFromParent,
            InputOutput, CopyFromParent, 0, NULL);
    
    /* Try to create a transparent background.
     *
     * The idea/technique attempts to mimic lines 342--360 of
     * "Eyes.c", from the "xeyes" source.  (The xeyes source is part
     * of the X11 source package.)
     *
     * Every other example I've seen uses a pixmap, but I'd like to
     * not have a pixmap as a requirement.
     */
    pmap = XCreatePixmap(dpy, w, W_WIDTH, W_HEIGHT, 1);
    shape_gc = XCreateGC(dpy, pmap, 0, &shape_xgcv);
    XSetForeground(dpy, shape_gc, 0);
    XFillRectangle(dpy, pmap, shape_gc, 0, 0, W_WIDTH, W_HEIGHT);

    XShapeCombineMask (dpy, w, ShapeBounding,
            0, 0, pmap, ShapeSet);

    /* If I remove everything above (until the comment), and replace
     * with the following, this application works as expected (e.g.,
     * draws a black window with white rectanles at each corner */
    /* XSetWindowBackground(dpy, w, BLACK_PIXEL); */

    /* create a graphics context for drawing */
    gcv.foreground = WHITE_PIXEL;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gc = XCreateGC(dpy, w,
            GCForeground | GCLineWidth | GCLineStyle, &gcv);

    /* register events: ExposureMask for re-drawing, ButtonPressMask
     * to capture mouse button press events */
    XSelectInput(dpy, w, ExposureMask | ButtonPressMask);

    XMapWindow(dpy, w);
    XSync(dpy, False);

    while(run) {
        XEvent xe;
        XNextEvent(dpy, &xe);
        switch (xe.type) {
            case Expose:
                /* whenever we get an expose, draw the rectangles */
                XSetForeground(dpy, gc, WHITE_PIXEL);
                XDrawRectangles(dpy, w, gc, rectangles, 4);
                XFillRectangles(dpy, w, gc, rectangles, 4);
                XSync(dpy, False);
                break;
            case ButtonPress: /* quit if a button is pressed */
                /* note that when using XShapeCombineMask(), i.e.
                 * trying to get a "transparent" background,
                 * no ButtonPress events are ever recognized
                 */
                printf("ButtonPress\n");
                run = 0;
                break;
            default:
                printf("Caught event %i\n", xe.type);
        }
    }

    XDestroyWindow(dpy, w);
    XCloseDisplay(dpy);

    return 0;
}
