#include <gtk/gtk.h>
#include "ventana.h"

//creamos la barra lateral
static GtkWidget *creaPanelNav(void){

  GtkWidget *panel = gtk_box_new( GTK_ORIENTATION_VERTICAL,12);

  gtk_widget_add_css_class(panel, "lateral");
  gtk_widget_set_size_request(panel,210,-1);

  GtkWidget *titulo = gtk_label_new("EXPLORADOR");

  gtk_widget_add_css_class(titulo, "seccion");
  gtk_box_append(GTK_BOX(panel), titulo);

  //ubis

  GtkWidget *inicio = gtk_button_new_with_label(" Inicio");
  GtkWidget *documentos = gtk_button_new_with_label(" documentos");
  GtkWidget *papelera = gtk_button_new_with_label(" papelera");

  Gtk_box_append(GTK_BOX(panel), inicio);
  Gtk_box_append(GTK_BOX(panel), documentos);
  Gtk_box_append(GTK_BOX(panel), papelera);

  //div
  GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_box_append(GTK_BOX(panel), separador);
  GtkWidget *acciones = gtk_label_new("ACCIONES");
  gtk_widget_add_css_class(acciones,"seccion");
  
gtk_box_append(GTK_BOX(panel), acciones);
gtk_box_append(GTK_BOX(panel), gtk_button_with_label("+ Crear Archivo"));
gtk_box_append(GTK_BOX(panel), gtk_button_with_label("+ Crear carpeta"));


return panel;
}


GtkWidget *crearVentana(GtkApplication *app){

 //defino forma del "widget"
   GtkWidget *ventana;     
   GtkWidget *principal; 
   GtkWidget *encabezado; 
   GtkWidget *contenido; 

 /*defino las tres columnas del gestor
   (tres columnas por que asi es mi modelo)*/

   GtkWidget *lateral; 
   GtkWidget *archivos; 
   GtkWidget *derecha;     
   
   ventana = gtk_application_window_new(app);

 //defino titulo de la ventana
   gtk_window_set_title(GTK_WINDOW(ventana), "Gestor de archivos");

 //defino tamaño incial/default
   gtk_window_set_default_size(GTK_WINDOW(ventana),1200,750);

   principal = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

   encabezado = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
   gtk_widget_add_css_class(encabezado, "encabezado");

   GtkWidget *titulo = gtk_label_new("Gestor de archivos");

   gtk_widget_add_css_class(titulo, "titulo");

   gtk_box_append(GTK_BOX(encabezado), titulo);

   contenido = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

   lateral = gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
   gtk_widget_set_size_request(lateral,210,-1);
   gtk_widget_add_css_class(lateral, "lateral");   

   //subpartes columna

   gtk_box_append(GTK_BOX(lateral), gtk_label_new("EXPLORADOR"));
    
   gtk_box_append(GTK_BOX(lateral),gtk_label_new("inicio"));

   gtk_box_append(GTK_BOX(lateral),gtk_label_new("Documentos"));

   gtk_box_append(GTK_BOX(lateral),gtk_label_new("Papelera"));


 //segrunda columna
   archivos = gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
   gtk_widget_set_hexpand(archivos, TRUE);
   gtk_widget_add_css_class(archivos, "archivos"); 

   //subpartes columna

   gtk_box_append(GTK_BOX(archivos), gtk_label_new("Contenido en las carpetas"));
    
 //tercera columna
   derecha = gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
   gtk_widget_set_size_request(derecha,270,-1);
   gtk_widget_add_css_class(derecha, "derecha"); 

   //subpartes columna
   gtk_box_append(GTK_BOX(derecha), gtk_label_new("asistente"));

   gtk_box_append(GTK_BOX(derecha), gtk_label_new("almacenamiento"));

 //union de las columnas
   gtk_box_append(GTK_BOX(contenido), lateral);
   gtk_box_append(GTK_BOX(contenido), archivos);
   gtk_box_append(GTK_BOX(contenido), derecha);

   gtk_box_append(GTK_BOX(principal), encabezado);
   gtk_box_append(GTK_BOX(principal), contenido);

   gtk_window_set_child(GTK_WINDOW(ventana), principal);

   return ventana;

}