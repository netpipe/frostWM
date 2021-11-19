#ifdef WIN32
#include <windows.h>
#else
#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include <X11/extensions/shape.h>
#endif
#include "SDL.h"
#include "SDL_syswm.h"
/*
Create window shape from 8bit mask surface

All pixels >= mask_cutoff are considered visible;
*/
int setWindowShape (SDL_Surface *mask, Uint8 mask_cutoff)
{
int x,y, xstart;
Uint8 *curpos;
#ifdef WIN32
HRGN MaskRegion;
HRGN TempRegion;
HWND WindowHwnd;
SDL_SysWMinfo wmi;
#else
int shape_event_base, shape_error_base;
GC shapeGC;
Pixmap shape_mask;
SDL_SysWMinfo wmi;
Display *display;
Window window;
Window parent;
XGCValues xgcv;
#endif
#ifdef WIN32
/* Win32 - Create and apply shape polygonal region */

/* Get the window handle */
SDL_VERSION(&wmi.version);
if (!SDL_GetWMInfo(&wmi)) {
return;
} else {
WindowHwnd=wmi.window;
}

/* Start with empty region */
MaskRegion=CreateRectRgn(0,0,0,0);

/* Scan through the mask bytemap creating small 1 line regions */
curpos=(Uint8 *)mask->pixels;
for (y=0; y<mask->h; y++) {
for (x=0, xstart=-1; x<mask->w; x++) {
/* Check for non-background pixel */
if (*curpos >= mask_cutoff) {
if (xstart<0) xstart=x;
} else {
/* Create region and add it to global region */
if (xstart>=0) {
TempRegion=CreateRectRgn(xstart,y,x,y+1);
CombineRgn(MaskRegion, TempRegion, MaskRegion, RGN_OR);
DeleteObject(TempRegion);
xstart=-1;
}
}
curpos++;
}
/* Create region and add it to global region */
if (xstart>=0) {
TempRegion=CreateRectRgn(xstart,y,x,y+1);
CombineRgn(MaskRegion, TempRegion, MaskRegion, RGN_OR);
DeleteObject(TempRegion);
xstart=-1;
}
}

/* Set the new region mask for the window */
SetWindowRgn (WindowHwnd, MaskRegion, TRUE);
/* Clean up mask */
DeleteObject(MaskRegion);
#else
/* X11 - Create and apply shape pixmap */

/* Get the X window handle */
SDL_VERSION(&wmi.version);
if (!SDL_GetWMInfo(&wmi)) {
return;
} else {
display=wmi.info.x11.display;
window=wmi.info.x11.wmwindow;
}
if (!XShapeQueryExtension (display, &shape_event_base, &shape_error_base)) {
return (-1);
}

/* Create empty pixmap */
shape_mask = XCreatePixmap (display, window, mask->w, mask->h, 1);
shapeGC = XCreateGC (display, shape_mask, 0, &xgcv);
XSetForeground (display, shapeGC, 0);
XFillRectangle (display, shape_mask, shapeGC, 0, 0, mask->w, mask->h);
XSetForeground (display, shapeGC, 1);

/* Scan through the mask bytemap and draw pixmap */
curpos=(Uint8 *)mask->pixels;
for (y=0; y<mask->h; y++) {
for (x=0, xstart=-1; x<mask->w; x++) {
/* Check for non-background pixel */
if (*curpos >= mask_cutoff) {
if (xstart<0) xstart=x;
} else {
/* Draw mask line */
if (xstart>=0) {
XDrawLine(display, shape_mask, shapeGC, xstart, y, x-1, y);
xstart=-1;
}
}
curpos++;
}
/* Draw mask line */
if (xstart>=0) {
XDrawLine(display, shape_mask, shapeGC, xstart, y, x-1, y);
xstart=-1;
}
}

/* Apply shape to window */
XShapeCombineMask (display, window, ShapeBounding, 0, 0, shape_mask, ShapeSet);
XSync (display, False);
/* Clean up pixmap */
XFreePixmap (display, shape_mask);
#endif /* WIN32 */

return (0);
} 
