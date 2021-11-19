#include <stdio.h>
#include <gtk/gtk.h>
#include <time.h>

#define TIMESTEP (1.0 / 100.0)

double x = 200, y = 100;

static gboolean
time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);

  y += 1; 
  return TRUE;
}


gboolean
draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  //printf("Draw callback called\n");

  guint width, height;

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  cairo_set_source_rgb (cr, 255, 255, 255);
  cairo_paint (cr);

  cairo_set_source_rgb(cr, 0, 0, 255);

  cairo_arc (cr,
             x, y,
             50,
             0, 2 * G_PI);

  cairo_stroke (cr);
  return FALSE;

}  


int
main (int argc, char *argv[]) {


  GtkWidget *window;

  GtkWidget *frame;
  GtkWidget *da;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW(window), 400, 400);
  gtk_window_move(GTK_WINDOW(window), 100, 100);                                          

  g_signal_connect (GTK_WINDOW(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

  //gtk_window_present (GTK_WINDOW(window));

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (window), frame);

  da = gtk_drawing_area_new ();
  /* set a minimum size */
  gtk_widget_set_size_request (da, 100, 100);

  gtk_container_add (GTK_CONTAINER (frame), da);

  /* Signals used to handle the backing surface */
  g_signal_connect (da, "draw",
            G_CALLBACK (draw_cb), NULL);

  g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);

  gtk_widget_show_all (window);  

  gtk_main ();

  return 0;
}
