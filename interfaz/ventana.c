#include <gtk/gtk.h>
#include "ventana.h"
#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/statvfs.h>
#endif

//creamos la barra lateral
static GtkWidget *crearPanelNavegacion(void){

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

  gtk_widget_add_css_class(inicio, "boton-menu");
  gtk_widget_add_css_class(documentos, "boton-menu");
  gtk_widget_add_css_class(papelera, "boton-menu");

  gtk_box_append(GTK_BOX(panel), inicio);
  gtk_box_append(GTK_BOX(panel), documentos);
  gtk_box_append(GTK_BOX(panel), papelera);

  //div
  GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_box_append(GTK_BOX(panel), separador);
  GtkWidget *acciones = gtk_label_new("ACCIONES");
  gtk_widget_add_css_class(acciones,"seccion");

  GtkWidget *btnCrearArchivo =
    gtk_button_new_with_label("+ Crear archivo");

  GtkWidget *btnCrearCarpeta =
    gtk_button_new_with_label("+ Crear carpeta");

  gtk_widget_add_css_class(btnCrearArchivo, "boton-menu");
  gtk_widget_add_css_class(btnCrearCarpeta, "boton-menu");

  gtk_box_append(GTK_BOX(panel), btnCrearArchivo);
  gtk_box_append(GTK_BOX(panel), btnCrearCarpeta);
  return panel;
}

static GtkWidget *vistaArchivosStack;
//tipos dev ista
static void mostrarLista(GtkButton *boton, gpointer datos){
  (void)boton;
  (void)datos;

  gtk_stack_set_visible_child_name(
    GTK_STACK(vistaArchivosStack), "lista"
  );
}

static void mostrarCuadricula(GtkButton *boton, gpointer datos){
  (void)boton;
  (void)datos;

  gtk_stack_set_visible_child_name(
    GTK_STACK(vistaArchivosStack), "cuadricula"
  );
}

//crea los 2 tipos
static GtkWidget *crearVistaLista(void){
  GtkWidget *lista;
  GtkWidget *fila;
  GtkWidget *nombre;
  GtkWidget *tipo;
  GtkWidget *tamanio;

  lista = gtk_list_box_new();
  gtk_list_box_set_selection_mode(
    GTK_LIST_BOX(lista), GTK_SELECTION_SINGLE
  );

  //de ejemplo 
  const char *nombres[] = {
    "Proyectos", "informe.txt", "main.c", "auditoria.c"
  };

  const char *tipos[] = {
    "Carpeta", "Texto", "Código C", "Código C"
  };

  const char *tamanios[] = {
    "—", "12 KB", "4 KB", "8 KB"
  };


  for (int i = 0; i < 4; i++)
  {
    GtkWidget *contenedor = gtk_box_new(
      GTK_ORIENTATION_HORIZONTAL, 12
    );

    nombre = gtk_label_new(nombres[i]);
    tipo = gtk_label_new(tipos[i]);
    tamanio = gtk_label_new(tamanios[i]);

    gtk_widget_set_hexpand(nombre,TRUE);
    gtk_label_set_xalign(GTK_LABEL(nombre),0.0);
    gtk_widget_set_size_request(tipo,100,-1);
    gtk_widget_set_size_request(tamanio,70,-1);
    
    gtk_box_append(GTK_BOX(contenedor),nombre);
    gtk_box_append(GTK_BOX(contenedor),tipo);
    gtk_box_append(GTK_BOX(contenedor),tamanio);

    fila = gtk_list_box_row_new();
    gtk_list_box_row_set_child(
      GTK_LIST_BOX_ROW(fila),contenedor
    );

    gtk_list_box_append(GTK_LIST_BOX(lista), fila);

  }
  return lista;
}

static GtkWidget *crearVistaCuadricula(void){
  GtkWidget *cuadricula;
  GtkWidget *elemento;
  GtkWidget *etiqueta;

  cuadricula = gtk_flow_box_new();
  gtk_flow_box_set_selection_mode(
    GTK_FLOW_BOX(cuadricula), GTK_SELECTION_SINGLE
  );

  gtk_flow_box_set_min_children_per_line(
    GTK_FLOW_BOX(cuadricula), 3
  );

  gtk_flow_box_set_max_children_per_line(
    GTK_FLOW_BOX(cuadricula), 5
  );

  gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(cuadricula), 12);
  gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(cuadricula), 12);

  const char *nombres[] = {
    "Proyectos",
    "informe.txt",
    "main.c",
    "auditoria.c"
  };

  const char *iconos[] = {
    "📁", "📄", "📄", "📄"
  };

  for (int i = 0; i< 4; i++){
    elemento= gtk_box_new(GTK_ORIENTATION_VERTICAL,6);
    gtk_widget_set_size_request(elemento, 110,95);
    gtk_widget_set_halign(elemento, GTK_ALIGN_CENTER);

    etiqueta = gtk_label_new(iconos[i]);
    gtk_widget_add_css_class(etiqueta, "icono-archivo");
    gtk_box_append(GTK_BOX(elemento), etiqueta);

    etiqueta = gtk_label_new(nombres[i]);
    gtk_box_append(GTK_BOX(elemento),etiqueta);

    gtk_flow_box_append(GTK_FLOW_BOX(cuadricula), elemento);
  }
  return cuadricula;
}

//explorador de archivos
static GtkWidget *crearExploradorArchivos(void){

  GtkWidget *contenedor;
  GtkWidget *barra;
  GtkWidget *titulo;
  GtkWidget *botonLista;
  GtkWidget *botonCuadricula;
  GtkWidget *busqueda;

  contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_add_css_class(contenedor, "archivos");

  //ubicacion actual
  titulo = gtk_label_new("inicio/documentos");
  gtk_label_set_xalign(GTK_LABEL(titulo), 0.0);
  gtk_box_append(GTK_BOX(contenedor), titulo);

  //buscador
  busqueda = gtk_search_entry_new();
  gtk_search_entry_set_placeholder_text(
    GTK_SEARCH_ENTRY(busqueda), "Buscar Archivos"
  );
  gtk_box_append(GTK_BOX(contenedor), busqueda);

  //selector
  barra = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  botonLista = gtk_button_new_with_label("☷ lista");
  botonCuadricula = gtk_button_new_with_label("▦ Cuadrícula");
  gtk_widget_add_css_class(botonLista, "boton-vista");
  gtk_widget_add_css_class(botonCuadricula, "boton-vista");

  g_signal_connect(
    botonLista, "clicked",
    G_CALLBACK(mostrarLista), NULL
  );

  g_signal_connect(
    botonCuadricula, "clicked",
    G_CALLBACK(mostrarCuadricula), NULL
  );

  gtk_box_append(GTK_BOX(barra), botonLista);
  gtk_box_append(GTK_BOX(barra), botonCuadricula);
  gtk_box_append(GTK_BOX(contenedor), barra);

  vistaArchivosStack = gtk_stack_new();
  gtk_stack_set_transition_type(
    GTK_STACK(vistaArchivosStack),
    GTK_STACK_TRANSITION_TYPE_CROSSFADE
  );

  gtk_stack_add_named(
    GTK_STACK(vistaArchivosStack),
    crearVistaLista(),
    "lista"
  );
  gtk_stack_add_named(
    GTK_STACK(vistaArchivosStack),
    crearVistaCuadricula(),
    "cuadricula"
  );
  gtk_widget_set_vexpand(vistaArchivosStack, TRUE);
  gtk_widget_set_hexpand(vistaArchivosStack, TRUE);

  gtk_box_append(GTK_BOX(contenedor), vistaArchivosStack);

  return contenedor;
}

//vista asistente
static GtkWidget *crearPanelRobot(void){
  GtkWidget *panel;
  GtkWidget *titulo;
  GtkWidget *imagen;
  GtkWidget *mensaje;

  panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_add_css_class(panel, "robot-panel");

  titulo =gtk_label_new("ASISTENTE");
  gtk_widget_add_css_class(titulo, "seccion");
  gtk_box_append(GTK_BOX(panel), titulo);

  ///pixel art
  imagen = gtk_label_new("🤖");
  gtk_widget_add_css_class(imagen, "robot-imagen");
  gtk_box_append(GTK_BOX(panel), imagen);

  mensaje = gtk_label_new(
    "Hola, soy tu compañero, en que puedo ayudarte?"
  );
  gtk_label_set_wrap(GTK_LABEL(mensaje), TRUE);
  gtk_box_append(GTK_BOX(panel), mensaje);

  return panel;

}

static void mostrarRobot(GtkButton *boton, gpointer datos){
  (void) boton;
  GtkPopover *popover = GTK_POPOVER(datos);
  if( gtk_widget_get_visible(GTK_WIDGET(popover))){
    gtk_popover_popdown(popover);
  }else{
    gtk_popover_popup(popover);
  }
}

static GtkWidget *crearAsistenteFlotante(void){
  GtkWidget *ancla;
  GtkWidget *popover;
  GtkWidget *contenido;
  GtkWidget *mensaje;

  ancla = gtk_button_new_with_label("asistente");
  gtk_widget_add_css_class(ancla,"boton-robot");
   
  popover = gtk_popover_new();
  gtk_popover_set_autohide(GTK_POPOVER(popover), TRUE);

  contenido = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_size_request(contenido, 220,130);

  mensaje = gtk_label_new("hola, en que puedo ayudarte?");
  gtk_label_set_wrap(GTK_LABEL(mensaje), TRUE);

  gtk_box_append(GTK_BOX(contenido), mensaje);
  gtk_popover_set_child(GTK_POPOVER(popover), contenido);

  gtk_widget_set_parent(popover, ancla);

  g_signal_connect(ancla,"clicked",
    G_CALLBACK(mostrarRobot), popover
  );

  return ancla;
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

  lateral = crearPanelNavegacion();
  archivos = crearExploradorArchivos();
  derecha = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_size_request(derecha, 270,-1);
  gtk_widget_add_css_class(derecha, "derecha");

  /* Robot integrado */
  gtk_box_append(GTK_BOX(derecha), crearPanelRobot());
  /* Botón para abrir el asistente flotante */
  gtk_box_append(GTK_BOX(derecha), crearAsistenteFlotante());
  gtk_box_append(GTK_BOX(derecha), gtk_label_new("ALMACENAMIENTO"));

 //union de las columnas
  gtk_box_append(GTK_BOX(contenido), lateral);
  gtk_box_append(GTK_BOX(contenido), archivos);
  gtk_box_append(GTK_BOX(contenido), derecha);

  gtk_box_append(GTK_BOX(principal), encabezado);
  gtk_box_append(GTK_BOX(principal), contenido);

  gtk_window_set_child(GTK_WINDOW(ventana), principal);

  return ventana;

}