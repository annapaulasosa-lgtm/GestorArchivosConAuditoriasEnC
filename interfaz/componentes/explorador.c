#include <gtk/gtk.h>
#include "../ventana.h"
#include <gio/gio.h>

static GtkWidget *contenedorLista;
static GtkWidget *contenedorCuadricula;
static GtkWidget *vistaArchivosStack;
static GtkWidget *etiquetaRuta;
static char *rutaActual = NULL;

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/statvfs.h>
#endif
static void limpiarContenedor(GtkWidget *contenedor)
{
  GtkWidget *hijo = gtk_widget_get_first_child(contenedor);

  while (hijo != NULL)
  {

    GtkWidget *siguiente = gtk_widget_get_next_sibling(hijo);
    gtk_flow_box_remove(GTK_FLOW_BOX(contenedor), hijo);
    hijo = siguiente;
  }
}

static void limpiarLista(GtkWidget *lista)
{

  GtkWidget *hijo = gtk_widget_get_first_child(lista);

  while (hijo != NULL)
  {
    GtkWidget *siguiente = gtk_widget_get_next_sibling(hijo);
    gtk_list_box_remove(GTK_LIST_BOX(lista), hijo);
    hijo = siguiente;
  }
}

static void cargarDirectorio(const char *ruta)
{
  limpiarLista(contenedorLista);
  limpiarContenedor(contenedorCuadricula);

  GFile *directorio = g_file_new_for_path(ruta);
  GError *error = NULL;

  GFileEnumerator *enumerador = g_file_enumerate_children(
      directorio, "standard::name,standard::type",
      G_FILE_QUERY_INFO_NONE, NULL, &error);

  if (enumerador == NULL)
  {
    g_warning("No se pudo abrir el directorio: %s", error->message);

    g_clear_error(&error);
    g_object_unref(directorio);
    return;
  }

  GFileInfo *info;

  while ((info = g_file_enumerator_next_file(enumerador, NULL, &error)) != NULL)
  {

    const char *nombre = g_file_info_get_name(info);

    gboolean esCarpeta = g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY;

    // ----- VISTA LISTA -----

    GtkWidget *fila =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    GFile *archivo = g_file_get_child(directorio, nombre);
    char *rutaCompleta = g_file_get_path(archivo);

    g_object_set_data_full(
        G_OBJECT(fila),
        "ruta",
        rutaCompleta,
        g_free);

    g_object_unref(archivo);

    GtkWidget *icono =
        gtk_label_new(esCarpeta ? "📁" : "📄");

    GtkWidget *etiqueta =
        gtk_label_new(nombre);

    gtk_label_set_xalign(GTK_LABEL(etiqueta), 0);

    gtk_widget_set_hexpand(etiqueta, TRUE);

    gtk_box_append(GTK_BOX(fila), icono);
    gtk_box_append(GTK_BOX(fila), etiqueta);

    gtk_list_box_append(GTK_LIST_BOX(contenedorLista), fila);

    // ----- VISTA CUADRICULA -----

    GtkWidget *elemento = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_size_request(elemento, 110, 95);

    GtkWidget *iconoGrande = gtk_label_new(esCarpeta ? "📁" : "📄");

    gtk_widget_add_css_class(iconoGrande, "icono-archivo");

    GtkWidget *nombreGrande = gtk_label_new(nombre);

    gtk_label_set_ellipsize(GTK_LABEL(nombreGrande), PANGO_ELLIPSIZE_END);

    gtk_box_append(GTK_BOX(elemento), iconoGrande);
    gtk_box_append(GTK_BOX(elemento), nombreGrande);

    gtk_flow_box_append(GTK_FLOW_BOX(contenedorCuadricula), elemento);

    g_object_unref(info);
  }

  if (error != NULL)
  {
    g_warning("Error leyendo directorio: %s", error->message);
    g_clear_error(&error);
  }

  g_file_enumerator_close(enumerador, NULL, NULL);

  g_object_unref(enumerador);
  g_object_unref(directorio);
}

static void alActivarFila(GtkListBox *lista, GtkListBoxRow *fila, gpointer datos)
{
  (void)lista;
  (void)datos;

  GtkWidget *contenido = gtk_list_box_row_get_child(fila);

  const char *ruta = g_object_get_data(
      G_OBJECT(contenido), "ruta");

  if (ruta == NULL)
    return;

  GFile *archivo = g_file_new_for_path(ruta);
  GFileType tipo = g_file_query_file_type(
      archivo,
      G_FILE_QUERY_INFO_NONE,
      NULL);

  if (tipo == G_FILE_TYPE_DIRECTORY)
  {
    g_free(rutaActual);
    rutaActual = g_strdup(ruta);

    gtk_label_set_text(GTK_LABEL(etiquetaRuta), rutaActual);
    cargarDirectorio(rutaActual);
  }

  g_object_unref(archivo);
}

// tipos dev ista

static void mostrarLista(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  gtk_stack_set_visible_child_name(
      GTK_STACK(vistaArchivosStack), "lista");
}

static void mostrarCuadricula(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  gtk_stack_set_visible_child_name(
      GTK_STACK(vistaArchivosStack), "cuadricula");
}

GtkWidget *crearVistaLista(void)
{
  contenedorLista = gtk_list_box_new();

  gtk_list_box_set_selection_mode(GTK_LIST_BOX(contenedorLista), GTK_SELECTION_SINGLE);
  g_signal_connect(
      contenedorLista,
      "row-activated",
      G_CALLBACK(alActivarFila),
      NULL);
  return contenedorLista;
}

GtkWidget *crearVistaCuadricula(void)
{

  contenedorCuadricula = gtk_flow_box_new();

  gtk_flow_box_set_selection_mode(
      GTK_FLOW_BOX(contenedorCuadricula),
      GTK_SELECTION_SINGLE);

  gtk_flow_box_set_min_children_per_line(
      GTK_FLOW_BOX(contenedorCuadricula), 3);

  gtk_flow_box_set_max_children_per_line(
      GTK_FLOW_BOX(contenedorCuadricula), 5);

  gtk_flow_box_set_row_spacing(
      GTK_FLOW_BOX(contenedorCuadricula), 12);

  gtk_flow_box_set_column_spacing(
      GTK_FLOW_BOX(contenedorCuadricula), 12);

  return contenedorCuadricula;
}

GtkWidget *crearExploradorArchivos(void)
{

  GtkWidget *contenedor;
  GtkWidget *barra;
  GtkWidget *botonLista;
  GtkWidget *botonCuadricula;
  GtkWidget *busqueda;
  GtkWidget *botonAtras;

  contenedor = gtk_box_new(
      GTK_ORIENTATION_VERTICAL, 12);

  gtk_widget_add_css_class(contenedor, "archivos");

  // Barra de ubicación
  barra = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

  botonAtras = gtk_button_new_with_label("←");
  etiquetaRuta = gtk_label_new("E:\\");

  gtk_label_set_xalign(GTK_LABEL(etiquetaRuta), 0);
  gtk_widget_set_hexpand(etiquetaRuta, TRUE);

  gtk_box_append(GTK_BOX(barra), botonAtras);
  gtk_box_append(GTK_BOX(barra), etiquetaRuta);

  gtk_box_append(GTK_BOX(contenedor), barra);

  // Buscador
  busqueda = gtk_search_entry_new();

  gtk_search_entry_set_placeholder_text(
      GTK_SEARCH_ENTRY(busqueda),
      "Buscar archivos...");

  gtk_box_append(GTK_BOX(contenedor), busqueda);

  // Selector de vistas
  barra = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

  botonLista = gtk_button_new_with_label("☷ Lista");
  botonCuadricula = gtk_button_new_with_label("▦ Cuadrícula");

  gtk_widget_add_css_class(botonLista, "boton-vista");
  gtk_widget_add_css_class(botonCuadricula, "boton-vista");

  g_signal_connect(
      botonLista, "clicked",
      G_CALLBACK(mostrarLista), NULL);

  g_signal_connect(
      botonCuadricula, "clicked",
      G_CALLBACK(mostrarCuadricula), NULL);

  gtk_box_append(GTK_BOX(barra), botonLista);
  gtk_box_append(GTK_BOX(barra), botonCuadricula);

  gtk_box_append(GTK_BOX(contenedor), barra);

  // Stack
  vistaArchivosStack = gtk_stack_new();

  gtk_stack_set_transition_type(
      GTK_STACK(vistaArchivosStack),
      GTK_STACK_TRANSITION_TYPE_CROSSFADE);

  gtk_stack_add_named(
      GTK_STACK(vistaArchivosStack),
      crearVistaLista(),
      "lista");

  gtk_stack_add_named(
      GTK_STACK(vistaArchivosStack),
      crearVistaCuadricula(),
      "cuadricula");

  gtk_widget_set_vexpand(vistaArchivosStack, TRUE);
  gtk_widget_set_hexpand(vistaArchivosStack, TRUE);

  gtk_box_append(
      GTK_BOX(contenedor), vistaArchivosStack);

  // Directorio inicial
#ifdef _WIN32
  rutaActual = g_strdup("E:\\");
#else
  rutaActual = g_strdup(g_get_home_dir());
#endif

  cargarDirectorio(rutaActual);

  return contenedor;
}

GtkWidget *crearPanelNavegacion(void)
{

  GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

  gtk_widget_add_css_class(panel, "lateral");
  gtk_widget_set_size_request(panel, 210, -1);

  GtkWidget *titulo = gtk_label_new("EXPLORADOR");

  gtk_widget_add_css_class(titulo, "seccion");
  gtk_box_append(GTK_BOX(panel), titulo);

  // ubis

  GtkWidget *inicio = gtk_button_new_with_label(" Inicio");
  GtkWidget *documentos = gtk_button_new_with_label(" documentos");
  GtkWidget *papelera = gtk_button_new_with_label(" papelera");

  gtk_widget_add_css_class(inicio, "boton-menu");
  gtk_widget_add_css_class(documentos, "boton-menu");
  gtk_widget_add_css_class(papelera, "boton-menu");

  gtk_box_append(GTK_BOX(panel), inicio);
  gtk_box_append(GTK_BOX(panel), documentos);
  gtk_box_append(GTK_BOX(panel), papelera);

  // div
  GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_box_append(GTK_BOX(panel), separador);
  GtkWidget *acciones = gtk_label_new("ACCIONES");
  gtk_widget_add_css_class(acciones, "seccion");

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
