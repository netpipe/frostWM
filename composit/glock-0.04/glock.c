/*
Glock - a simple analog clock using GTK+
$Id: glock.c 17 2006-02-21 00:08:49Z eric $
Copyright 2005, 2006 Eric L. Smith <eric@brouhaha.com>

Glock is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.  Note that I am not
granting permission to redistribute or modify glock under the
terms of any later version of the General Public License.

Glock is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program (in the file "COPYING"); if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111, USA.
*/

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gtk/gtk.h>


static GtkWidget *main_window;
static GtkWidget *menubar;  /* actually a popup menu in transparency/shape mode */
static GtkItemFactory *item_factory;


typedef struct
{
  int width;
  int height;
  double radius;
  GtkWidget *drawing_area;
  struct tm tm;
#ifdef __GDK_CAIRO_H__
  cairo_t *cr;
#else
  GdkGC *background_gc;
  GdkGC *frame_gc;
  GdkGC *hour_hand_gc;
  GdkGC *minute_hand_gc;
  GdkGC *second_hand_gc;
#endif
} glock_t;

glock_t glock;


#define QMAKESTR(x) #x
#define MAKESTR(x) QMAKESTR(x)


char *progname;  /* set to argv[0] in main */


void usage (FILE *f)
{
  fprintf (f, "Glock %s:  analog clock\n",
	   MAKESTR(GLOCK_RELEASE));
  fprintf (f, "Copyright 2005 Eric L. Smith\n");
  fprintf (f, "http://www.brouhaha.com/~eric/software/glock/\n");
  fprintf (f, "\n");
  fprintf (f, "usage: %s [options...]\n", progname);
  fprintf (f, "options:\n");
  fprintf (f, "   --size n         sets size in pixels\n");
}


/* generate fatal error message to stderr, doesn't return */

void fatal (int ret, char *format, ...)
{
  va_list ap;

  fprintf (stderr, "fatal error: ");
  va_start (ap, format);
  vfprintf (stderr, format, ap);
  va_end (ap);
  if (ret == 1)
    usage (stderr);
  exit (ret);
}


void *alloc (size_t size)
{
  void *p;

  p = calloc (1, size);
  if (! p)
    fatal (2, "Memory allocation failed\n");
  return (p);
}


char *newstr (char *orig)
{
  int len;
  char *r;

  len = strlen (orig);
  r = (char *) alloc (len + 1);
  memcpy (r, orig, len + 1);
  return (r);
}


static void quit_callback (GtkWidget *widget, gpointer data)
{
  gtk_main_quit ();
}


static void edit_copy (GtkWidget *widget, gpointer data)
{
  /* $$$ not yet implemented */
}


static void help_about (GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog;
  char buf [200];

  dialog = gtk_dialog_new_with_buttons ("About glock",
					GTK_WINDOW (main_window),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK,
					GTK_RESPONSE_NONE,
					NULL);

  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), TRUE);

  sprintf (buf, "glock release %s", MAKESTR(GLOCK_RELEASE));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
		     gtk_label_new (buf));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
		     gtk_label_new ("Analog clock\n"
				    "Copyright 2005 Eric L. Smith\n"
				    "http://www.brouhaha.com/~eric/software/glock/"));
  gtk_widget_show_all (dialog);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}


static GtkItemFactoryEntry menu_items [] =
  {
    { "/_File",         NULL,         NULL,          0, "<Branch>" },
    { "/File/_Quit",    "<CTRL>Q",    gtk_main_quit, 0, "<StockItem>", GTK_STOCK_QUIT },
    { "/_Edit",         NULL,         NULL,          0, "<Branch>" },
    { "/Edit/_Copy",    "<control>C", edit_copy,     0, "<StockItem>", GTK_STOCK_COPY },
    { "/_Help",         NULL,         NULL,          0, "<LastBranch>" },
    { "/_Help/About",   NULL,         help_about,    0, "<Item>" }
  };

static gint nmenu_items = sizeof (menu_items) / sizeof (GtkItemFactoryEntry);


static GtkWidget *create_menus (GtkWidget *window,
				GtkType container_type)
{
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();
  item_factory = gtk_item_factory_new (container_type,
				       "<main>",
				       accel_group);
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
  return (gtk_item_factory_get_widget (item_factory, "<main>"));
}


gboolean window_click_callback (GtkWidget *widget, GdkEventButton *event)
{
  if (event->type == GDK_BUTTON_PRESS)
    {
      switch (event->button)
	{
	case 1:  /* left button */
	  gtk_window_begin_move_drag (GTK_WINDOW (main_window),
				      event->button,
				      event->x_root,
				      event->y_root,
				      event->time);
	  break;
	case 3:  /* right button */
	  gtk_menu_popup (GTK_MENU (menubar),
			  NULL,  /* parent_menu_shell */
			  NULL,  /* parent_menu_item */
			  NULL,  /* func */
			  NULL,  /* data */
			  event->button,
			  event->time);
	  break;
	}
    }
  return FALSE;
}


gboolean clock_tick (gpointer data)
{
  GdkRectangle rect = { 0, 0, 0, 0 };

  rect.width = glock.width;
  rect.height = glock.height;

  gdk_window_invalidate_rect (glock.drawing_area->window,
			      & rect,
			      FALSE);

  return TRUE;
}


#ifdef __GDK_CAIRO_H__

static void draw_mask (GdkDrawable *bitmap)
{
  cairo_t *cr;

  cr = gdk_cairo_create (bitmap);

  // fill window_shape_bitmap with black
  cairo_save (cr);
  cairo_rectangle (cr, 0, 0, glock.width, glock.height);
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_fill (cr);
  cairo_restore (cr);

  // draw white filled circle into window_shape_bitmap
  cairo_arc (cr, glock.width / 2, glock.height / 2, glock.radius, 0, 2 * M_PI);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_fill (cr);

  cairo_destroy (cr);
}

static void init_graphics_context (void)
{
  //glock.cr = gdk_cairo_create (glock.drawing_area->window);
  //cairo_rectangle (glock.cr, 0, 0, glock.width, glock.height);
  //cairo_clip (glock.cr);
}

static void draw_line (int red, int green, int blue,
		       double line_width,
		       double inner_radius, double outer_radius,
		       double angle_60ths)
{
  double h = sin (angle_60ths * M_PI / 30);
  double v = -cos (angle_60ths * M_PI / 30);

  cairo_save (glock.cr);
  cairo_set_source_rgb (glock.cr, red, green, blue);
  cairo_set_line_width (glock.cr, line_width * cairo_get_line_width (glock.cr));
  cairo_move_to (glock.cr,
		 glock.width  / 2 + inner_radius * glock.radius * h,
		 glock.height / 2 + inner_radius * glock.radius * v);
  cairo_line_to (glock.cr,
		 glock.width  / 2 + outer_radius * glock.radius * h,
		 glock.height / 2 + outer_radius * glock.radius * v);
  cairo_stroke (glock.cr);
  cairo_restore (glock.cr);
}

static void draw_face (void)
{
  int i;

  glock.cr = gdk_cairo_create (glock.drawing_area->window);

  cairo_arc (glock.cr, glock.width / 2, glock.height / 2, glock.radius, 0, 2 * M_PI);
  cairo_set_source_rgb (glock.cr, 1, 1, 1);
  cairo_fill_preserve (glock.cr);

  cairo_set_source_rgb (glock.cr, 0, 0, 0);
  cairo_stroke (glock.cr);

  for (i = 0; i < 12; i++)
    {
      double inset, width;
      if (i % 3)
	{
	  inset = 0.1;
	  width = 0.5;
	}
      else
	{
	  inset = 0.2;
	  width = 1.0;
	}
      draw_line (0, 0, 0, width, 1.0 - inset, 1.0, i * 5);
    }

  draw_line (0, 0, 0, 2.5, 0, 0.5,
	     glock.tm.tm_hour * 5 + glock.tm.tm_min / 12.0);
  draw_line (0, 0, 0, 1.0, 0, 0.75,
	     glock.tm.tm_min + glock.tm.tm_sec / 60.0);
  draw_line (1, 0, 0, 1.0, 0, 0.7,
	     glock.tm.tm_sec);

  cairo_destroy (glock.cr);
  glock.cr = NULL;
}

#else

static void draw_mask (GdkDrawable *bitmap)
{
  GdkColormap *colormap;
  GdkColor black;
  GdkColor white;
  GdkGC *gc;

  colormap = gdk_colormap_get_system ();
  gdk_color_black (colormap, & black);
  gdk_color_white (colormap, & white);

  gc = gdk_gc_new (bitmap);
  gdk_gc_set_foreground (gc, & black);
  gdk_gc_set_background (gc, & white);

  // fill window_shape_bitmap with black

  gdk_draw_rectangle (bitmap,
		      gc,
		      TRUE,  // filled
		      0,     // x
		      0,     // y
		      glock.width,
		      glock.height);

  gdk_gc_set_foreground (gc, & white);
  gdk_gc_set_background (gc, & black);

  // draw white filled circle into window_shape_bitmap
  gdk_draw_arc (bitmap,
		gc,
		TRUE,     // filled
		0,        // x
		0,        // y
		glock.width,
		glock.height,
		0,        // angle 1
		360*64);  // angle 2: full circle
}

static void init_graphics_context (void)
{
  glock.frame_gc = glock.drawing_area->style->fg_gc [GTK_WIDGET_STATE (glock.drawing_area)];
  glock.background_gc = glock.drawing_area->style->bg_gc [GTK_WIDGET_STATE (glock.drawing_area)];
  // ideally we'd set up colors here.
  glock.hour_hand_gc = glock.frame_gc;
  glock.minute_hand_gc = glock.frame_gc;
  glock.second_hand_gc = glock.frame_gc;
}

static void draw_line (GdkGC *gc,
		       double inner_radius, double outer_radius,
		       double angle_60ths)
{
  double h = sin (angle_60ths * M_PI / 30);
  double v = -cos (angle_60ths * M_PI / 30);

  gdk_draw_line (glock.drawing_area->window,
		 gc,
		 glock.width  / 2 + inner_radius * glock.radius * h,
		 glock.height / 2 + inner_radius * glock.radius * v,
		 glock.width  / 2 + outer_radius * glock.radius * h,
		 glock.height / 2 + outer_radius * glock.radius * v);
		 
}

static void draw_face (void)
{
  int i;

  gdk_draw_arc (glock.drawing_area->window,
		glock.frame_gc,
		TRUE,
		0,
		0,
		glock.width,
		glock.height,
		0,
		64 * 360);
  gdk_draw_arc (glock.drawing_area->window,
		glock.background_gc,
		TRUE,
		glock.width / 16,
		glock.height / 16,
		(glock.width * 7) / 8,
		(glock.height * 7) / 8,
		0,
		64 * 360);

  for (i = 0; i < 12; i++)
    draw_line (glock.frame_gc, 0.75, 1.0, i * 5);

  draw_line (glock.hour_hand_gc, 0.0, 0.5,
	     glock.tm.tm_hour * 5 + glock.tm.tm_min / 12.0);
  draw_line (glock.minute_hand_gc, 0.0, 0.75,
	     glock.tm.tm_min + glock.tm.tm_min / 60.0);
  draw_line (glock.second_hand_gc, 0.0, 0.7,
	     glock.tm.tm_sec);
}

#endif


gboolean expose_callback (GtkWidget *widget,
			  GdkEventExpose *event,
			  gpointer data)
{
  time_t t;

  t = time (NULL);
  localtime_r (& t, & glock.tm);

  draw_face ();

  return TRUE;
}


int main (int argc, char *argv[])
{
  int width = 200;
  int height = 200;

  GdkDrawable *window_shape_bitmap;
  GtkWidget *event_box;

  progname = newstr (argv [0]);

  gtk_init (& argc, & argv);

  while (--argc)
    {
      argv++;
      if (*argv [0] == '-')
	{
	  if (strcasecmp (argv [0], "--size") == 0)
	    {
	      if ((! --argc) || (! isdigit (*argv [1])))
		fatal (1, "size option requires numeric argument\n");
	      width = height = atoi (argv [1]);
	      argv++;
	    }
	  else if (strcasecmp (argv [0], "--help") == 0)
	    {
	      usage (stderr);
	      exit (0);
	    }
	  else
	    fatal (1, "unrecognized option '%s'\n", argv [0]);
	}
      else
	fatal (1, "unrecognized option '%s'\n", argv [0]);
    }

  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  glock.width = width;
  glock.height = height;
  glock.radius = width / 2.0;

  glock.drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (glock.drawing_area, width, height);

  window_shape_bitmap = (GdkDrawable *) gdk_pixmap_new (NULL,
							width,
							height,
							1);

  draw_mask (window_shape_bitmap);

  gtk_widget_shape_combine_mask (main_window,
				 window_shape_bitmap,
				 0,
				 0);

  gtk_window_set_resizable (GTK_WINDOW (main_window), FALSE);

  gtk_window_set_title (GTK_WINDOW (main_window),
			progname);

  menubar = create_menus (main_window, GTK_TYPE_MENU);

  g_signal_connect (G_OBJECT (glock.drawing_area),
		    "expose_event",
		    G_CALLBACK (expose_callback),
		    NULL);

  event_box = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (event_box), glock.drawing_area);

  gtk_container_add (GTK_CONTAINER (main_window), event_box);

  gtk_window_set_decorated (GTK_WINDOW (main_window), FALSE);

  // Have to show everything again, now that we've done display_init()
  // and combined the shape mask.
  gtk_widget_show_all (main_window);

  init_graphics_context ();

  // Mouse click to drag
  g_signal_connect (G_OBJECT (main_window),
		    "button_press_event",
		    G_CALLBACK (window_click_callback),
		    NULL);

  g_signal_connect (G_OBJECT (main_window),
		    "destroy",
		    G_CALLBACK (quit_callback),
		    NULL);

  g_timeout_add (1000, clock_tick, NULL);

  gtk_main ();

  exit (0);
}
