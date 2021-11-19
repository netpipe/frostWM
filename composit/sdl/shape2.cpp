/* Please note that this code was written by „moma” at ubuntuforums.org
 */
//  shape2.cpp
//  An example that shows howto create a shaped, borderless window.
//  Uses only XLib and XShape* extension calls.
//
//  This program creates its own shape mask.
//
// Compile it:
// g++ -lX11 -lXext shape2.cpp -o shape2
//
// Run it:
// ./shape2
 
#include <stdlib.h> // For exit()
#include    <iostream>
 
#include    <X11/Xos.h>
#include    <X11/Xlib.h>
#include    <X11/Xutil.h>
#include    <X11/Xatom.h>
#include    <X11/extensions/shape.h>
 
 
using namespace std;
 
 
Display *g_display;
int      g_screen;
Window   g_win;
int      g_disp_width;
int      g_disp_height;
Pixmap   g_bitmap;
 
int     shape_event_base;
int     shape_error_base;
 
// The window size
int WIDTH  = 300;
int HEIGHT = 200;
 
long event_mask = (StructureNotifyMask|ExposureMask| \
        PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyRelease | ButtonPress|ButtonRelease|KeymapStateMask);
 
 
// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
XColor createXColorFromRGB(short red, short green, short blue)
{
    XColor color;
 
    // m_color.red = red * 65535 / 255;
    color.red = (red * 0xFFFF) / 0xFF;
    color.green = (green * 0xFFFF) / 0xFF;
    color.blue = (blue * 0xFFFF) / 0xFF;
    color.flags = DoRed | DoGreen | DoBlue;
 
    if (!XAllocColor(g_display, DefaultColormap(g_display, g_screen), &color))
    {
        std::cerr << "createXColorFromRGB: Cannot create color" << endl;
        exit(-1);
    }
    return color;
}
 
// Create a window
void createShapedWindow()
{
    XSetWindowAttributes wattr;
    XColor black, black_e;
 
    Window root    = DefaultRootWindow(g_display);
    // Visual *visual = DefaultVisual(g_display, g_screen);
 
    // Events for normal windows
    #define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask | \
         PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask | \
         ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask )
 
    // #define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)
    // #define NOT_PROPAGATE_MASK (0)
    //  |  CWDontPropagate
 
    unsigned long mask = CWBackPixmap|CWBackPixel|CWBorderPixel|CWWinGravity|CWBitGravity|
                         CWSaveUnder|CWEventMask|CWOverrideRedirect|CWColormap;
 
    wattr.background_pixmap = None;
    wattr.background_pixel = black.pixel;
 
    wattr.border_pixel=0;
 
    wattr.win_gravity=NorthWestGravity;
    wattr.bit_gravity=ForgetGravity;
 
    wattr.save_under=1;
 
    wattr.event_mask=BASIC_EVENT_MASK;
 
    // wattr.do_not_propagate_mask=NOT_PROPAGATE_MASK;
 
    // This is needed for OpenGL
    wattr.override_redirect=0;
 
    wattr.colormap= DefaultColormap(g_display, g_screen);
 
    XAllocNamedColor(g_display, DefaultColormap(g_display, g_screen), "black", &black, &black_e);
    // You can write:  black = createXColorFromRGB(0, 0, 0) here.
 
    wattr.background_pixel = black.pixel;
 
    g_win = XCreateWindow(g_display, root, 0, 0, WIDTH, HEIGHT, 1, CopyFromParent, InputOutput, CopyFromParent, mask , &wattr);
 
    // We will draw our own mask
 
    // ** g_bitmap = XCreateBitmapFromData (g_display, RootWindow(g_display, g_screen), (char *)myshape_bits, myshape_width, myshape_height);
    // ** XShapeCombineMask(g_display, g_win, ShapeBounding, 0, 0, g_bitmap, ShapeSet);
 
    // We want shape-changed event too
    #define SHAPE_MASK ShapeNotifyMask
    XShapeSelectInput (g_display, g_win, SHAPE_MASK );
 
    // Tell the Window Manager not to draw window borders (frame) or title.
        wattr.override_redirect = 1;
    XChangeWindowAttributes(g_display, g_win, CWOverrideRedirect, &wattr);
 
    // Show the window
    XMapWindow(g_display, g_win);
}
 
 
// Common drawing routine
void commonDraw(Drawable _drawable, GC _gc, int _width, int _height)
{
    // Draw X
    XDrawLine(g_display, _drawable, _gc, 0, 0, _width, _height);
    XDrawLine(g_display, _drawable, _gc, 0, _height, _width, 0);
 
    // Filled circle at the center
    int radius = _height / 4;
    int x = _width  / 2 - radius / 2;
    int y = _height / 2 - radius / 2;
    XFillArc(g_display, _drawable, _gc,  x, y, radius, radius, 0, 360*64);
}
 
// First, draw on the mask. Then draw the same figure on the window.
// The mask has bit depth 1.
void draw()
{
    GC mask_gc;
    GC draw_gc;
    XGCValues gcv;
    XColor red;
    XWindowAttributes attr;
    Pixmap bitmap;
 
    int height, width;
 
#define LINE_WIDTH 4
 
    // Get window attributes
    XGetWindowAttributes(g_display, g_win, &attr);
 
    width  = attr.width;
    height = attr.height;
 
// --------- Draw on mask --------
 
    // Create a simple bitmap mask (depth = 1)
    bitmap = XCreatePixmap(g_display, g_win, width, height, 1);
 
    // Create mask_gc
    mask_gc = XCreateGC(g_display, bitmap, 0, NULL);
    XSetBackground(g_display, mask_gc, 1);
 
    // Clear the mask bits first
    XSetForeground(g_display, mask_gc, 0);
    XFillRectangle(g_display, bitmap, mask_gc, 0, 0, width, height);
 
 
    XSetFillStyle(g_display, mask_gc, FillSolid);
    XSetLineAttributes(g_display, mask_gc, LINE_WIDTH , LineSolid, CapButt, JoinMiter);
 
    XSetForeground(g_display, mask_gc, 1);
    commonDraw(bitmap, mask_gc, width, height);
 
 
// --------- Draw the same thing in the window --------
 
    // Create draw_gc
    draw_gc = XCreateGC(g_display, g_win, 0, NULL);
    XColor blue = createXColorFromRGB(0, 0, 255);
 
    XSetFillStyle(g_display, draw_gc, FillSolid);
    XSetLineAttributes(g_display, draw_gc, LINE_WIDTH , LineSolid, CapButt, JoinMiter);
 
    XSetForeground(g_display, draw_gc, blue.pixel);
 
    // Now draw on the window
    commonDraw(g_win, draw_gc, width, height);
 
    // Set mask
    XShapeCombineMask(g_display, g_win, ShapeBounding, 0, 0, bitmap, ShapeSet);
 
    // Free objects
    XFreePixmap(g_display, bitmap);
    XFreeGC(g_display, mask_gc);
    XFreeGC(g_display, draw_gc);
 
}
 
void moveWindow(int _dx, int _dy)
{
    XWindowAttributes attr;
 
    // Get window attributes
    XGetWindowAttributes(g_display, g_win, &attr);
 
    XMoveWindow(g_display, g_win, attr.x + _dx, attr.y + _dy);
}
 
 
void openDisplay()
{
    g_display = XOpenDisplay(0);
 
    if (!g_display)
    {
        cerr << "Failed to open X display" << endl;
        exit(-1);
    }
 
    g_screen    = DefaultScreen(g_display);
 
    g_disp_width  = DisplayWidth(g_display, g_screen);
    g_disp_height = DisplayHeight(g_display, g_screen);
 
    // Has shape extions?
    if (!XShapeQueryExtension (g_display, &shape_event_base, &shape_error_base))
    {
       cerr << "NO shape extension in your system !" << endl;
       exit (-1);
    }
}
 
 
int main()
{
    openDisplay();
 
    createShapedWindow();
 
    XEvent xevt;
    XExposeEvent *eev;
    XConfigureEvent *cev;
    XKeyEvent *kev;
    XMotionEvent *xmotion;
    KeySym key_symbol;
    XPoint xp;  // Mouse point
 
    while (1)
    {
        XNextEvent(g_display, &xevt);
 
        // Note!  Shaped window generates some special events
        // You got "shape_event_base" from XShapeQueryExtension(...)
        if (xevt.type == shape_event_base + ShapeNotify)
        {
            cout << "Got shape changed event" << endl;
            continue;
        }
 
 
        switch (xevt.type)
        {
        case Expose:
            if (xevt.xexpose.count != 0) continue;
            eev = &xevt.xexpose;
 
            draw();
            break;
 
        case ButtonPress:
            cout << "Got Button press" << endl;
 
            // Quit!
            exit(1);
 
            break;
 
        case ButtonRelease:
            cout << "Got Button release" << endl;
            break;
 
 
        case MotionNotify:
            // cout << "Got Button move" << endl;
 
            xmotion = &xevt.xmotion;
 
            xp.x = xevt.xbutton.x;
            xp.y = xevt.xbutton.y;
            if (xmotion->state & Button1Mask)
            {
                // moveWindow2(xp);
            }
            break;
 
 
        case KeyPress:
            kev = &xevt.xkey;
            int x, y;
            Window the_win;
 
            x = kev->x;
            y = kev->y;
            the_win = kev->window;
 
            // Keycode to a keysymbol.
            key_symbol = XKeycodeToKeysym(g_display, kev->keycode, 0);
            switch (key_symbol)
            {
                case XK_1:
                case XK_KP_1:
                    cout << "Key press: '1'" << endl;
                    break;
 
                case XK_Delete:
                    cout << "Key press: 'delete'" << endl;
                    break;
 
                case XK_Left:
                    moveWindow(-5, 0);
                    break;
 
                case XK_Right:
                    moveWindow(5, 0);
                    break;
 
                case XK_Down:
                    moveWindow(0, 5);
                    break;
 
                case XK_Up:
                    moveWindow(0, -5);
                    break;
 
                default:  /* anything else - check if it is a letter key */
 
                    int ascii_key;
                    if (key_symbol >= XK_A && key_symbol <= XK_Z)
                    {
                        ascii_key = key_symbol - XK_A + 'A';
                        // printf("Key pressed - '%c'\n", ascii_key);
                    }
                    else if (key_symbol >= XK_a && key_symbol <= XK_z)
                    {
                        ascii_key = key_symbol - XK_a + 'a';
                        // printf("Key pressed - '%c'\n", ascii_key);
                    }
 
                    switch (toupper(ascii_key))
                    {
                        case 'U':
                            cout << "Up key" << endl;
                            break;
 
                        case 'N':
                            break;
                    }
            }
            break;
 
        case ConfigureNotify:
            cev = &xevt.xconfigure;
            break;
 
        }
    }
 
    return 0;
}
