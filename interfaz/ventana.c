#include <gtk/gtk.h>
#include "ventana.h"
#include "componentes/explorador.h"
#include "componentes/robot.h"
#include "componentes/almacenamiento.h"
#include <gtk/gtk.h>


#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/statvfs.h>
#endif



GtkWidget *crearVentana(GtkApplication *app)
{

  // defino forma del "widget"
  GtkWidget *ventana;
  GtkWidget *principal;
  GtkWidget *encabezado;
  GtkWidget *contenido;

  //defino las tres columnas del gestor(tres columnas por que asi es mi modelo)

  GtkWidget *lateral;
  GtkWidget *archivos;
  GtkWidget *derecha;

  ventana = gtk_application_window_new(app);
  // probarDisco(); lo use para consultar el funcionamiento de la funcion
  // defino titulo de la ventana
  gtk_window_set_title(GTK_WINDOW(ventana), "Gestor de archivos");

  // defino tamaño incial/default
  gtk_window_set_default_size(GTK_WINDOW(ventana), 1200, 750);

  principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  encabezado = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_widget_add_css_class(encabezado, "encabezado");

  GtkWidget *titulo = gtk_label_new("Gestor de archivos");

  gtk_widget_add_css_class(titulo, "titulo");

  gtk_box_append(GTK_BOX(encabezado), titulo);

  contenido = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  lateral = crearPanelNavegacion();
  archivos = crearExploradorArchivos();
  derecha = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_size_request(derecha, 270, -1);
  gtk_widget_add_css_class(derecha, "derecha");

  /* Robot integrado */
  gtk_box_append(GTK_BOX(derecha), crearPanelRobot());
  /* Botón para abrir el asistente flotante */
  gtk_box_append(GTK_BOX(derecha), crearAsistenteFlotante());
  gtk_box_append(GTK_BOX(derecha), crearPanelAlmacenamiento());

  // union de las columnas
  gtk_box_append(GTK_BOX(contenido), lateral);
  gtk_box_append(GTK_BOX(contenido), archivos);
  gtk_box_append(GTK_BOX(contenido), derecha);

  gtk_box_append(GTK_BOX(principal), encabezado);
  gtk_box_append(GTK_BOX(principal), contenido);

  gtk_window_set_child(GTK_WINDOW(ventana), principal);

  return ventana;
}
