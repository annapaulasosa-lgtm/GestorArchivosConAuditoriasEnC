#include <gtk/gtk.h>
#include "../ventana.h"
#include <gio/gio.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/statvfs.h>
#endif

static void mostrarRobot(GtkButton *boton, gpointer datos)
{
  (void)boton;
  GtkPopover *popover = GTK_POPOVER(datos);
  if (gtk_widget_get_visible(GTK_WIDGET(popover)))
  {
    gtk_popover_popdown(popover);
  }
  else
  {
    gtk_popover_popup(popover);
  }
}

GtkWidget *crearAsistenteFlotante(void)
{
  GtkWidget *ancla;
  GtkWidget *popover;
  GtkWidget *contenido;
  GtkWidget *mensaje;

  ancla = gtk_button_new_with_label("asistente");
  gtk_widget_add_css_class(ancla, "boton-robot");

  popover = gtk_popover_new();
  gtk_popover_set_autohide(GTK_POPOVER(popover), TRUE);

  contenido = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_size_request(contenido, 220, 130);

  mensaje = gtk_label_new("hola, en que puedo ayudarte?");
  gtk_label_set_wrap(GTK_LABEL(mensaje), TRUE);

  gtk_box_append(GTK_BOX(contenido), mensaje);
  gtk_popover_set_child(GTK_POPOVER(popover), contenido);

  gtk_widget_set_parent(popover, ancla);

  g_signal_connect(ancla, "clicked",
                   G_CALLBACK(mostrarRobot), popover);

  return ancla;
}

GtkWidget *crearPanelRobot(void)
{
  GtkWidget *panel;
  GtkWidget *titulo;
  GtkWidget *imagen;
  GtkWidget *mensaje;

  panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_add_css_class(panel, "robot-panel");

  titulo = gtk_label_new("ASISTENTE");
  gtk_widget_add_css_class(titulo, "seccion");
  gtk_box_append(GTK_BOX(panel), titulo);

  /// pixel art
  imagen = gtk_picture_new_for_filename("assets/happy.png");
  ;
  gtk_widget_add_css_class(imagen, "robot-imagen");
  gtk_box_append(GTK_BOX(panel), imagen);

  mensaje = gtk_label_new(
      "Hola, soy tu compañero, en que puedo ayudarte?");
  gtk_label_set_wrap(GTK_LABEL(mensaje), TRUE);
  gtk_box_append(GTK_BOX(panel), mensaje);

  return panel;
}