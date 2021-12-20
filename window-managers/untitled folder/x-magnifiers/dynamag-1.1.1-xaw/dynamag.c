/*
 * DynaMag 1.1.1
 * Bas Cancrinus <bas@cipherware.com> & Fabian Groffen 2002
 *
 * Change log:
 *  5 Oct 2002 - Bas: Removed fancy cursor stuff from main() and
 * 	  inclusion of bitmap files.
 *  6 Oct 2002 - Fabian: Compiler warnings solutions by explicit casts
 *    a little touch up of the code.
 *  8 Oct 2002 - Bas: Cleaned up code (removed unused variables,
 *    simplified functions).
 *  9 Oct 2002 - Bas: modified repaint_image() to capture the area
 *    around the mouse pointer instead of the upper-right area.
 */

/*
        UnWindows 1.1
        dynamag.c

        Richard L. Kline
	Rensselaer Polytechnic Institute
        16 May 1994

	This is a major rewrite of the xmag program distributed by
	MIT with X11R5.  It features three modes of magnification.  The
	magnifier can be made to follow the mouse pointer around,
	displaying a magnified image either in a window that is
	"sticky" to the pointer, or in a stationary window.
	The magnifier can also be `anchored' to continually magnify a fixed
	area of the screen.

	The copyright notice of the original program appears below,
	though, admittedly, there isn't much of it left.

	---
	Modification history:
	1.1 16 May 94 - added new follow mode without sticky window
			fixed bug in window resizing code
			eliminated nasty lag in response of command
			  buttons when magnified area was large
*/

/*
 *
 * xmag - simple magnifying glass application
 *
 * $XConsortium: xmag.c,v 1.29 89/12/10 17:23:58 rws Exp $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>

/* constants ****************************************************************/
#define DEFAULT_BORDER_SIZE      2
#define DEFAULT_CURSOR_SIZE     64
#define DEFAULT_PIX_FONT       "fixed"

#define DEFAULT_MAG_HEIGHT     150
#define DEFAULT_MAG_WIDTH      470
#define DEFAULT_MAGNIFICATION    5
#define DEFAULT_UPDATE           0

#define MOTION_THRESH            4


/* global variables *********************************************************/
int       Argc;                            /* copy of argc */
char    **Argv;                            /* copy of argv */
char     *ProgramName;                     /* name of program being executed */

int       display_height;
int       display_width;
int       screen;                          /* screen being used */
int       pixmap_format;                   /* XYPixmap or ZPixmap */
int       ncolormapentries;                /* size of colormap being used */

Colormap     cmap;                         /* colormap to use for drawing */
Window       root;                         /* root window of screen */
XGCValues    gcv;                          /* for creating GC */
XtAppContext appCtx;
XtIntervalId next_timeout;

Window    t          = (Window)NULL;
Display  *dpy        = (Display *)NULL;    /* X server being used */
GC        fillGC     = (GC)NULL;           /* for drawing enlargement */
char     *pix_font   = (char *)NULL;       /* for displaying pixel value */

int       mag_toggle = False;
int       following  = False;
int       rightnow   = False;
int       mag_height = DEFAULT_MAG_HEIGHT;
int       mag_width  = DEFAULT_MAG_WIDTH;
int       mag        = DEFAULT_MAGNIFICATION;
int       update_int = DEFAULT_UPDATE;
int       src_height = DEFAULT_CURSOR_SIZE;
int       src_width  = DEFAULT_CURSOR_SIZE;

int       old_x      = 9999;               /* pointer's previous position */
int       old_y      = 9999;
int       root_x, root_y;                  /* pointer's current screen coordinates */
int       anchor_x, anchor_y;              /* lower left corner of screen area being magnified */

char      mag_specs_file[128];

String fallback_resources[] = {
  "*font:         -b&h-lucidatypewriter-bold-r-normal-sans-*-350-*-*-*-*-iso8859-*",
  "*buttons*skipAdjust: True",
  "*Paned*showGrip: False",
  NULL,
};


Widget toplevel, bigview, follow, onoff, setshell;
Widget mag_label, mag_scroll, upd_label, upd_scroll, stick_x;
float mag_scroll_pos, upd_scroll_pos;
int sticky = TRUE;

float upd_scroll_values[10] = {
  999.0, 0.25, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 10.0, 20.0 };
char upd_scroll_labels[10][9] = {
  "disabled", "0.25 sec", "0.5 sec", "1 sec", "2 sec", "3 sec",
  "4 sec", "5 sec", "10 sec", "20 sec" };

/* functions ****************************************************************/
void                setup_window();
void                magnify();
void                repaint_image();
Bool                do_magnify();

XtCallbackProc      Quit();
XtCallbackProc      Follow();
XtCallbackProc      MagnifyOnOff();
XtCallbackProc      Settings();
XtCallbackProc      KillSettings();
XtCallbackProc      SaveSettings();
XtCallbackProc      StickyOnOff();
XtTimerCallbackProc UpdateMagnifier();

void                LoadSettings();
void                UpdateScroll();
void                NewMagLevel();


/*
 * Exit procedure.
 */

XtCallbackProc Quit ()
{
  if (dpy)
    {
      XCloseDisplay (dpy);
      dpy = NULL;
    }
  (void) exit (0);
} /* Quit */


/*
 * Usage - print out usage message and quit.
 */

int Usage ()
{
    static char *help_message[] = {
"where options include:",
"    -display displaystring        X server to contact",
"",
"It may be abbreviated to -d.",
"",
NULL
    }, **cpp;

    fprintf (stderr, "usage:\n        %s [-option ...]\n\n", ProgramName);
    for (cpp = help_message; *cpp; cpp++) {
	fprintf (stderr, "%s\n", *cpp);
    }
    Quit (1);
} /* Usage */


/*
 * Follow - make the window follow the cursor around the screen.
 *          Callback function for first command button.
 */

XtCallbackProc Follow (w)
  Widget w;
{
  Position x, y;
  Window  top = XtWindow (toplevel);
  Window  parent, root;
  Window* children;
  unsigned numChildren;
  XWindowAttributes parentAttributes;
  char geom[12];
  XtIntervalId new;

  /* Start following the cursor around. */
  if ( !following ) {
    following = True;

    /* If we're being sticky, hide the application window. */
    if (sticky) {

      XtUnrealizeWidget (toplevel);
      t = (Window) NULL;

    /* If not, take over the pointer so we can capture the button
       press that turns off this mode. */
    } else {

      XtGrabPointer (toplevel, False,
		 PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
		 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
      XtAddRawEventHandler (toplevel, ButtonPressMask|ButtonReleaseMask,
			    True, (XtEventHandler)Follow, NULL);

    }

    /* Timeout updates do very bad things while following is on. */
    if (next_timeout) {
      XtRemoveTimeOut (next_timeout);
      next_timeout = (XtIntervalId) 0;

    rightnow = True;
    repaint_image (0);

    }

  /* Stop following the cursor and get back to widgets. */
  } else {

    following = False;

    /* If in sticky mode, we must bring back the application widgets. */
    if (sticky) {

      sprintf (geom, "+%d+%d", root_x, root_y);
      XtVaSetValues (toplevel, XtNgeometry, geom, NULL);
      XtRealizeWidget (toplevel);

    /* If not, we must return the pointer which we stole while following. */
    } else {

      XtUngrabPointer(toplevel, CurrentTime);
      XtRemoveRawEventHandler(toplevel, ButtonPressMask|ButtonReleaseMask,
			      True, (XtEventHandler)Follow, NULL);

    }

    XSync (dpy, False);
    rightnow = True;
    repaint_image (0);

  /* Create a new timeout if update and magnifier are currently on. */
  if (update_int && mag_toggle)
    new =
      XtAppAddTimeOut (appCtx, (int)(upd_scroll_values[update_int]*1000),
			   (XtTimerCallbackProc)UpdateMagnifier, (XtPointer)NULL);
  else
    new = (XtIntervalId) 0;

  /* If there's already a timeout pending, get rid of it.*/
  if (next_timeout)
    XtRemoveTimeOut (next_timeout);

  next_timeout = new;
  }

} /* Follow */



/*
 * MagnifyOnOff - toggle the magnifier active/inactive.
 *         Callback function for second command button.
 */

XtCallbackProc MagnifyOnOff ()
{

  if (mag_toggle) {

    XtVaSetValues (onoff, XtNlabel, "Resume", NULL);

    /* If there's already a timeout pending, get rid of it.*/
    if (next_timeout)
      XtRemoveTimeOut (next_timeout);
    next_timeout = (XtIntervalId) 0;

  } else {

    /* Create a new timeout if update is currently on. */
    if (update_int)
      next_timeout =
	XtAppAddTimeOut (appCtx, (int)(upd_scroll_values[update_int]*1000),
			 (XtTimerCallbackProc)UpdateMagnifier, (XtPointer)NULL);

    XtVaSetValues (onoff, XtNlabel, " Stop ", NULL);
    rightnow = True;
    repaint_image (0);
  }
  mag_toggle = !mag_toggle;

} /* MagnifyOnOff */



/*
 * Settings - Pop up the settings window.
 */

XtCallbackProc Settings ()
{
  char ch[20];
  Position x, y;

  sprintf (ch, "Magnify: X %d", mag);
  XtVaSetValues (mag_label, XtNlabel, ch, NULL);
  mag_scroll_pos = (mag - 2) * 0.125;
  XawScrollbarSetThumb (mag_scroll, mag_scroll_pos, 0.1);

  XtVaSetValues (upd_label, XtNlabel, "Update: disabled", NULL);
  sprintf (ch, "Update: %s", upd_scroll_labels[update_int]);
  upd_scroll_pos = update_int * 0.1;
  XawScrollbarSetThumb (upd_scroll, upd_scroll_pos, 0.1);

  if (sticky)
    XtVaSetValues (stick_x, XtNlabel, "Yes", NULL);
  else
    XtVaSetValues (stick_x, XtNlabel, "No", NULL);

  /* Turn off timeouts while this window is up. */
  if (next_timeout)
    XtRemoveTimeOut (next_timeout);

  XtTranslateCoords (follow, (Position)0, (Position)0, &x, &y);
  XtVaSetValues (setshell, XtNx, x, XtNy, y, NULL);
  XtPopup (setshell, XtGrabNonexclusive);
  XtVaSetValues (upd_label, XtNlabel, ch, NULL);
}


/*
 * NewMagLevel - The user has just fiddled with the mag level scrollbar.
 */

void NewMagLevel (w, client_data, position)
     Widget w;
     XtPointer client_data;
     XtPointer position;
{
  char ch[20];

  if ((int)client_data == 1) { /* left or right button pressed */
    if ((int)position > 0) { /* left button was pressed */
      if (mag_scroll_pos >= 0.125)
	mag_scroll_pos = (int)(mag_scroll_pos * 8) / 8.0 - 0.125;
      else
	mag_scroll_pos = 0.0;
    } else { /* right button was pressed */
      if (mag_scroll_pos < 0.875)
	mag_scroll_pos = (int)(mag_scroll_pos * 8) / 8.0 + 0.125;
      else
	mag_scroll_pos = 0.875;
    }
  } else {
    mag_scroll_pos = *((float *) position);
  }
  XawScrollbarSetThumb (mag_scroll, mag_scroll_pos, 0.1);
  mag = (int)(mag_scroll_pos * 8) + 2;
  src_height = mag_height / mag;
  src_width  = mag_width  / mag;
  rightnow = True;
  repaint_image (0);
  sprintf (ch, "Magnify: X %d", mag);
  XtVaSetValues (mag_label, XtNlabel, ch, NULL);
}


/*
 * UpdateScroll - The user has just fiddled with the update scrollbar.
 */

void UpdateScroll (w, client_data, position)
     Widget w;
     XtPointer client_data;
     XtPointer position;
{
  char ch[20];

  if ((int)client_data == 1) { /* left or right button pressed */
    if ((int)position > 0) { /* left button was pressed */
      if (upd_scroll_pos >= 0.1)
	upd_scroll_pos = (int)(upd_scroll_pos * 10) / 10.0 - 0.1;
      else
	upd_scroll_pos = 0.0;
    } else { /* right button was pressed */
      if (upd_scroll_pos < 0.899)
	upd_scroll_pos = (int)(upd_scroll_pos * 10) / 10.0 + 0.1;
      else
	upd_scroll_pos = 0.9;
    }
  } else {
    upd_scroll_pos = *((float *) position);
  }
  XawScrollbarSetThumb (upd_scroll, upd_scroll_pos, 0.1);
  update_int = (int)(upd_scroll_pos * 10);
  sprintf (ch, "Update: %s", upd_scroll_labels[update_int]);
  XtVaSetValues (upd_label, XtNlabel, ch, NULL);

}

XtTimerCallbackProc UpdateMagnifier (x)
     XtPointer x;
{
  XtIntervalId new;

  if (mag_toggle) {
    rightnow = True;
    repaint_image (0);

    /* Create a new timeout if update is currently on. */
    if (update_int)
      new = XtAppAddTimeOut (appCtx, (int)(upd_scroll_values[update_int]*1000),
		     (XtTimerCallbackProc)UpdateMagnifier, (XtPointer)NULL);
    else
      new = (XtIntervalId) 0;

    /* If there's already a timeout pending, get rid of it.*/
    if (next_timeout)
      XtRemoveTimeOut (next_timeout);
    next_timeout = new;
  }

}


/*
 * KillSettings - User's done with the settings window.
 */

XtCallbackProc KillSettings ()
{
  XtIntervalId new;

  /* Create a new timeout if update and magnifier are currently on. */
  if (update_int && mag_toggle)
    new = XtAppAddTimeOut (appCtx, (int)(upd_scroll_values[update_int]*1000),
			   (XtTimerCallbackProc)UpdateMagnifier, (XtPointer)NULL);
  else
    new = (XtIntervalId) 0;

  /* If there's already a timeout pending, get rid of it.*/
/*  if (next_timeout)
    XtRemoveTimeOut (next_timeout);
  next_timeout = new;
*/
  XtPopdown (setshell);
}


/*
 * LoadSettings - Read in the user's preferences from the appropriate
 *                file.  Set defaults if the file is incomplete, has
 *                bad values, or is altogether missing.
 */

void LoadSettings ()
{
  FILE * fp;
  char line[80];

  strcpy ( mag_specs_file, getenv ( "HOME" ) );
  strcat ( mag_specs_file, "/.dynamag" );
  fp = fopen (mag_specs_file, "r");
  if (fp) {
    fscanf (fp, "%s %d", line, &mag_height);
    fscanf (fp, "%s %d", line, &mag_width);
    fscanf (fp, "%s %d", line, &mag);
    fscanf (fp, "%s %d", line, &update_int);
    fscanf (fp, "%s %d", line, &sticky);
  } else {
    mag_height = DEFAULT_MAG_HEIGHT;
    mag_width  = DEFAULT_MAG_WIDTH;
    mag    = DEFAULT_MAGNIFICATION;
    update_int = DEFAULT_UPDATE;
    sticky = FALSE;
  }

  src_height = mag_height / (mag);
  src_width  = mag_width / (mag);

}


/*
 * SaveSettings - Save the current configuration into the user's
 *                preferences file.
 */

XtCallbackProc SaveSettings ()
{
  FILE * fp = fopen (mag_specs_file, "w");

  if (fp) {
    fprintf (fp, "magnifier_height	%d\n", mag_height);
    fprintf (fp, "magnifier_width	%d\n", mag_width);
    fprintf (fp, "magnification		%d\n", mag);
    fprintf (fp, "update_interval	%d\n", update_int);
    fprintf (fp, "sticky		%d\n", sticky);
  } else {
    fprintf (stderr, "Unable to save settings to '%s'!\n", mag_specs_file);
  }

}


/*
 * StickyOnOff - Switches behavior of mobile mode between mobile and
 *               stationary magnification window
 */

XtCallbackProc StickyOnOff ()
{

  if (sticky) {
    sticky = FALSE;
    XtVaSetValues (stick_x, XtNlabel, "No", NULL);
  } else {
    sticky = TRUE;
    XtVaSetValues (stick_x, XtNlabel, "Yes", NULL);
  }

}


main (argc, argv)
    int argc;
    char **argv;
{

    int i;				/* iterator variable */
    char *displayname = NULL;		/* name of X server to contact */
    char *source_geometry = NULL;	/* size of square tracking pointer */
    char *geometry = NULL;		/* size and location of enlarged */
    int magnification = 0;		/* enlarged image will be m by m */
    char *border_color = NULL;		/* border color name */
    char *back_color = NULL;		/* background of enlargement */
    int border_width = -1;		/* border width in pixels */
    Position x, y;

    Arg     arg[10];

    ProgramName = argv[0];
    Argc = argc;
    Argv = argv;

    for (i = 1; i < argc; i++) {
	char *arg = argv[i];

	if (arg[0] == '-') {		/* process command line switches */
	    switch (arg[1]) {
	      case 'd':			/* -display displaystring */
		if (++i >= argc) Usage ();
		displayname = argv[i];
		continue;
	      default:
		Usage ();
		/* NOTREACHED */
	    }				/* end switch */
	} else {			/* any command line parameters */
	    Usage ();
	    /* NOTREACHED */
	}				/* end if else */
    }					/* end for */

    toplevel = XtAppInitialize (&appCtx, "Dynamag", NULL, 0,
				&argc, argv, fallback_resources, NULL, 0);

    dpy = XtDisplay (toplevel);

    if (!dpy) {
      fprintf (stderr, "%s:  unable to open display \"%s\".\n",
	       ProgramName, XDisplayName (displayname));
      Quit (1);
    }

    display_height = DisplayHeight (dpy, DefaultScreen(dpy) );
    display_width = DisplayWidth (dpy, DefaultScreen(dpy) );

    root = DefaultRootWindow (dpy);
    screen = DefaultScreen (dpy);
    cmap = DefaultColormap (dpy, screen);
    ncolormapentries = DisplayCells (dpy, screen);
    pixmap_format = (DisplayPlanes (dpy, screen) > 1 ? ZPixmap : XYPixmap);

    XSelectInput (dpy, root,
		  PointerMotionMask|KeyPressMask|StructureNotifyMask);

    LoadSettings ();
    setup_window (mag_height, mag_width, mag);
    XtRealizeWidget (toplevel);

    /* Turn it on */
    mag_toggle = False;
    MagnifyOnOff ();

    /*
     * get defaults (would be nice to use the resource manager instead)
     */

    pix_font = XGetDefault (dpy, ProgramName, "font");
    pix_font = XGetDefault (dpy, ProgramName, "Font");

    source_geometry = XGetDefault (dpy, ProgramName, "source");
    source_geometry = XGetDefault (dpy, ProgramName, "Source");

    {
    	char *tmp = XGetDefault (dpy, ProgramName, "magnification");
    	if (!tmp)
		tmp = XGetDefault (dpy, ProgramName, "Magnification");
    }

    {
    	char *tmp = XGetDefault (dpy, ProgramName, "borderWidth");
    	if (!tmp)
		tmp = XGetDefault (dpy, ProgramName, "BorderWidth");
    	border_width = tmp ? atoi (tmp) : DEFAULT_BORDER_SIZE;
    }

    border_color = XGetDefault (dpy, ProgramName, "borderColor");
    border_color = XGetDefault (dpy, ProgramName, "BorderColor");

    back_color = XGetDefault (dpy, ProgramName, "background");
    back_color = XGetDefault (dpy, ProgramName, "Background");

    XSync (dpy, False);
    do_magnify();

    Quit (0);
}


/*
 * Set up all the fine widgets that make up the application.
 * Figure out the initial settings for the magnifier.
 */

void setup_window (height, width, mag)
     int height, width, mag;
{
  /* Parent window stuff */
  Widget mom, temp;

  /* Pop-up window stuff */
  Dimension ht;
  Widget setform;


  mom = XtVaCreateManagedWidget ("the_form", panedWidgetClass,
				 toplevel, NULL);

  bigview = XtVaCreateManagedWidget ("bigview", widgetClass, mom,
				      XtNheight, height, XtNwidth, width,
				      XtNbackground, BlackPixel(dpy,screen),
				      NULL);

  mom = XtVaCreateManagedWidget ("buttons", formWidgetClass, mom, NULL);

  follow = XtVaCreateManagedWidget ("Follow", commandWidgetClass, mom, NULL);
  XtAddCallback (follow, XtNcallback, (XtCallbackProc)Follow, (XtPointer)NULL);

  onoff = XtVaCreateManagedWidget ("onoff", commandWidgetClass, mom,
			       XtNfromHoriz, follow, XtNlabel, " Stop ", NULL);
  XtAddCallback (onoff, XtNcallback, (XtCallbackProc)MagnifyOnOff, (XtPointer)NULL);

  temp = XtVaCreateManagedWidget ("Settings", commandWidgetClass, mom,
				  XtNfromHoriz, onoff, NULL);
  XtAddCallback (temp, XtNcallback, (XtCallbackProc)Settings, (XtPointer)NULL);

  temp = XtVaCreateManagedWidget ("Quit", commandWidgetClass, mom,
				  XtNfromHoriz, temp, NULL);
  XtAddCallback (temp, XtNcallback, (XtCallbackProc)Quit, (XtPointer)NULL);

  /* Set up the settings pop-up window */

  if (!setshell) {
    setshell = XtVaCreatePopupShell ("setshell", transientShellWidgetClass,
				 toplevel, XtNtitle, "dynamag settings", NULL);

    setform = XtVaCreateManagedWidget ("setform", formWidgetClass, setshell,
				       NULL);

    mag_label = XtVaCreateManagedWidget ("mag_label", labelWidgetClass,
					 setform, XtNlabel, "Magnify: X 2",
					 XtNborderWidth, 0, NULL);

    upd_label = XtVaCreateManagedWidget ("upd_label", labelWidgetClass,
					 setform, XtNlabel, "Update: disabled",
					 XtNjustify, XtJustifyLeft,
					 XtNfromVert, mag_label,
					 XtNborderWidth, 0, NULL);

    /* Get height of label (which was automatically chosen based on the
       label font size) to use in setting height of companion widgets */
    XtVaGetValues (mag_label, XtNheight, &ht, NULL);

    mag_scroll = XtVaCreateManagedWidget ("mag_scroll", scrollbarWidgetClass,
				     setform, XtNorientation,
				     XtorientHorizontal,
				     XtNfromHoriz, upd_label, XtNheight, ht,
				     XtNwidth, 300, XtNminimumThumb, 40, NULL);
    XtAddCallback (mag_scroll, XtNscrollProc, NewMagLevel, (XtPointer) 1);
    XtAddCallback (mag_scroll, XtNjumpProc, NewMagLevel, (XtPointer) 2);

    upd_scroll = XtVaCreateManagedWidget ("upd_scroll", scrollbarWidgetClass,
					  setform, XtNorientation,
				    XtorientHorizontal, XtNfromVert, mag_label,
				     XtNfromHoriz, upd_label, XtNheight, ht,
				     XtNwidth, 300, XtNminimumThumb, 40, NULL);
    XtAddCallback (upd_scroll, XtNscrollProc, UpdateScroll, (XtPointer) 1);
    XtAddCallback (upd_scroll, XtNjumpProc, UpdateScroll, (XtPointer) 2);

    temp = XtVaCreateManagedWidget ("stick_label", labelWidgetClass, setform,
				    XtNlabel, "Sticky window", XtNjustify,
				    XtJustifyLeft, XtNfromVert, upd_label,
				    XtNborderWidth, 0, NULL);

    stick_x = XtVaCreateManagedWidget ("stick_x", commandWidgetClass, setform,
				    XtNlabel, "Yes", XtNfromVert, upd_label,
				    XtNfromHoriz, upd_label, XtNresize, False,
				    NULL);
    XtAddCallback (stick_x, XtNcallback, (XtCallbackProc)StickyOnOff, (XtPointer)NULL);

    temp = XtVaCreateManagedWidget ("setsave", commandWidgetClass, setform,
				    XtNfromVert, stick_x, XtNlabel,
				    "Save settings", NULL);
    XtAddCallback (temp, XtNcallback, (XtCallbackProc)SaveSettings, (XtPointer)NULL);

    temp = XtVaCreateManagedWidget ("setdone", commandWidgetClass, setform,
				    XtNfromVert, stick_x, XtNfromHoriz, temp,
				    XtNlabel, "Done", NULL);
    XtAddCallback (temp, XtNcallback, (XtCallbackProc)KillSettings, (XtPointer)NULL);

  }


} /* setup_window */


/*
	Display the magnifying glass and follow the pointer.
*/
Bool do_magnify ()
{
  XGCValues xgcv;
  GC	  myGC;
  XColor  white;
  unsigned long	  border_pixel;
  int	  i = 1;
  int window_width = 0, window_height = 0;
  Position x, y;

  XParseColor (dpy, cmap, "White", &white);
  XAllocColor (dpy, cmap, &white);
  xgcv.background = white.pixel;
  border_pixel = BlackPixel (dpy, screen);

  anchor_x = 0;
  anchor_y = src_height;

  while (1) {			/* do forever */
    XEvent event;
    XEvent peek;
    XMotionEvent xme;

    XtAppNextEvent (appCtx, &event);
    XtDispatchEvent (&event);

    /* End pointer-following mode if mouse button is pressed
       inside magnifier window. */

    if (following && event.type == ButtonPress)
      {
	XDestroyWindow (dpy, t);
	t = (Window) NULL;
	Follow (t);

	/* remember where the window ended up, since the magnifier
	   is still on */
	anchor_x = root_x; anchor_y = root_y;
	old_x = old_y = 9999;

      }

    /* Figure out new dimensions for magnifier if window has
       been resized. */
    if (event.type == ConfigureNotify)
      {
	XConfigureEvent *xce = &(event.xconfigure);
	if (xce->width != window_width || xce->height != window_height) {
	  XtVaGetValues (bigview, XtNwidth, &x, XtNheight, &y, NULL);
	  mag_width = x;
	  mag_height = y;
	  src_width = mag_width / mag;
	  src_height = mag_height / mag;
	  window_width = xce->width;
	  window_height = xce->height;
	}
      }

    /* We can get up to five of these at once, depending on how many
       of the command buttons are uncovered as well as the image area.
       But we only want to redraw once. */
    if (event.type == Expose) {
      repaint_image (0);
      if (QLength (dpy))
	{
	  XPeekEvent (dpy, &peek);
	  while (QLength (dpy) && peek.type == Expose)
	    {
	      XtAppNextEvent (appCtx, &event);
	      XtDispatchEvent (&event);
	  /* If queue is empty, XPeekEvent will block.  We don't want this. */
	      if (QLength (dpy))
		XPeekEvent (dpy, &peek);
	    }
	}
    }

    /* Redisplay if pointer is moved */
    if (event.type == MotionNotify) {

      /* Look through events to find the most recent MotionNotify event,
	 so we can use the coordinates given there as the most current
	 pointer position.  We can gloss over all event types except
	 the ones above, ButtonPress, Expose, and ConfigureNotify. */

      xme = event.xmotion;
/*      x = event.xmotion.x_root;
      y = event.xmotion.y_root;*/
      if (QLength (dpy))
	{
	  XPeekEvent (dpy, &peek);
	  while (QLength (dpy) > 0 && peek.type != Expose &&
		 peek.type !=ConfigureNotify && peek.type != ButtonPress)
	    {
	      XtAppNextEvent (appCtx, &event);
	      XtDispatchEvent (&event);
	      if (event.type == MotionNotify)
		xme = event.xmotion;
	      /*	    {
			    x = event.xmotion.x_root;
			    y = event.xmotion.y_root;
			    }
			    */
	  /* If queue is empty, XPeekEvent will block.  We don't want this. */
	      if (QLength (dpy))
		XPeekEvent (dpy, &peek);
	    }
	}

      /* redraw the magnifying glass if:

	 1 - it is toggled on
	 2 - the pointer moved more than MOTION_THRESH pixels on
	 either axis
	 */
      if ((mag_toggle || following) &&
	  (abs (xme.x_root - old_x) > MOTION_THRESH ||
	   abs (xme.y_root - old_y) > MOTION_THRESH))
	{
	  repaint_image (xme);
	}

    }
  } /* while (1) */

} /* do_magnify */



/*
	copy the appropriate screen area and fill in the magnifying glass

	NOTE: I tried XDrawLine in place of XFillRectangle, but it was
	no faster and produced some flickering and some incorrect drawing.
*/
void repaint_image (XMotionEvent xme) {
    XImage *image;
    int cap_x, cap_y; /* Zero-coordinates of area to capture. */
    int r_x, r_y, r_width, r_height;
    XGCValues gcv;
    int row, column;
    int maxrow, maxcolumn;
    unsigned long pixel;
    unsigned long prev_pixel;

  XSetWindowAttributes attr;

  attr.save_under = True;
  attr.override_redirect = True;
  attr.border_pixel = BlackPixel (dpy, screen);
  attr.background_pixel = WhitePixel (dpy, screen);

    /* One-shot conditions ("rightnow" is TRUE):
       - "follow" has just been turned on
       - magnify level has been changed
       - update time-out has just occurred */

    if (following) {
      if (rightnow) {
	rightnow = False;
	root_x = anchor_x;
	root_y = anchor_y;
      } else {
	old_x = root_x = xme.x_root;
	old_y = root_y = xme.y_root;

	if (sticky && t) XDestroyWindow (dpy, t);
      }
    }

    if (root_x <= 0) root_x = 0;
    if (root_y <= 0) root_y = 0;

	/* Get a symmetric area around the pointer. */

    /* Calculate the X-coordinate */
    if (root_x - (src_width / 2) < 0) {
		cap_x = 0;
	} else {
		if (root_x + (src_width / 2) > display_width) {
			cap_x = display_width - src_width;
		} else {
			cap_x = root_x - (src_width / 2);
		}
	}

    /* Calculate the Y-coordinate */
    if (root_y - (src_height / 2) < 0) {
		cap_y = 0;
	} else {
		if (root_y + (src_height / 2) > display_height) {
			cap_y = display_height - src_height;
		} else {
			cap_y = root_y - (src_height / 2);
		}
	}

	image = XGetImage (dpy, root, cap_x, cap_y,
			src_width, src_height, AllPlanes, pixmap_format);

    /* If the magnifier is currently following the cursor,
       it is its own unmanaged window and must be created.
       If not, then the "bigdraw" box widget is where the
       magnified image will be drawn. */
    if (following && sticky) {
      r_x = src_width * mag;
      r_y = src_height * mag;
      if (root_y - mag_height <= 0)
	if (root_x + mag_width - MOTION_THRESH > display_width)
	  t = XCreateWindow (dpy, root,
			     display_width - r_x, 0,
			     r_x, r_y,
			     2, 8, InputOutput, CopyFromParent,
			     CWSaveUnder | CWBorderPixel |
			     CWOverrideRedirect, &attr);
	else
	  t = XCreateWindow (dpy, root,
			     root_x-MOTION_THRESH, 0,
			     r_x, r_y,
			     2, 8, InputOutput, CopyFromParent,
			     CWSaveUnder | CWBorderPixel |
			     CWOverrideRedirect, &attr);
      else
	if (root_x + r_x - MOTION_THRESH> display_width)
	  t = XCreateWindow (dpy, root,
			     display_width - r_x,
			     root_y - r_y+MOTION_THRESH,
			     r_x, r_y,
			     2, 8, InputOutput, CopyFromParent,
			     CWSaveUnder | CWBorderPixel |
			     CWOverrideRedirect, &attr);
	else
	  t = XCreateWindow (dpy, root,
			     root_x-MOTION_THRESH,
			     root_y - r_y+MOTION_THRESH,
			     r_x, r_y,
			     2, 8, InputOutput, CopyFromParent,
			     CWSaveUnder | CWBorderPixel |
			     CWOverrideRedirect, &attr);
      XSelectInput (dpy, t, ButtonPressMask);
      XMapWindow (dpy, t);
    } else {
      t = XtWindow (bigview);
    }

    gcv.function = GXcopy;
    gcv.plane_mask = AllPlanes;
    gcv.fill_style = FillSolid;

    fillGC = XCreateGC (dpy, t,
			(GCFunction | GCPlaneMask | GCFillStyle),
			&gcv);

    gcv.foreground = prev_pixel = BlackPixel (dpy, screen);
    XChangeGC (dpy, fillGC, GCForeground, &gcv);

    maxrow = ((mag_height + mag - 1) / mag) - 1;
    maxcolumn = ((mag_width + mag - 1) / mag) - 1;

    /*
     * clip to image size
     */

    if (maxcolumn >= image->width) maxcolumn = image->width - 1;
    if (maxrow >= image->height) maxrow = image->height - 1;

    /*
     * Repaint the image; be somewhat clever by only drawing when the pixel
     * changes or when we run out of lines.
     */

    r_y = 0;	/* to start */
    r_height = mag;		/* always doing just 1 line */

    /*
     * iterate over scanlines (rows)
     */

    for (row = 0; row <= maxrow; row++) {
	r_x = 0;			/* start at beginning */
	r_width = mag;

	/*
	 * iterate over pixels in scanline (columns)
	 */
	gcv.foreground = XGetPixel ( image, 0, row );
	for ( column = 1; column <= maxcolumn; column++ ) {
	    pixel = XGetPixel ( image, column, row );
	    if ( pixel != gcv.foreground ) {
		XChangeGC ( dpy, fillGC, GCForeground, &gcv );
		XFillRectangle ( dpy, t, fillGC, r_x, r_y, r_width, r_height );
		gcv.foreground = pixel;
		r_x += r_width;
		r_width = 0;
	    }
	    r_width += mag;
	} /* end for */
	XChangeGC ( dpy, fillGC, GCForeground, &gcv );
	XFillRectangle ( dpy, t, fillGC, r_x, r_y, r_width, r_height );
	r_y += mag; /* advance to next scanline */
    } /* end for */

    XDestroyImage (image);

    return;
}
