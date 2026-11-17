#include <gtk/gtk.h>
#include "../ventana.h"
#include <gio/gio.h>
#include "../../procArchivos.h"

static GtkWidget *contenedorLista;
static GtkWidget *contenedorCuadricula;
static GtkWidget *vistaArchivosStack;
static GtkWidget *etiquetaRuta;
static char *rutaActual = NULL;
static char *rutaSeleccionada = NULL;

static void seleccionarElemento(const char *ruta)
{
  g_free(rutaSeleccionada);
  rutaSeleccionada = g_strdup(ruta);
}

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

static void mostrarImagen(const char *ruta)
{
  GtkWidget *ventana = gtk_window_new();

  gtk_window_set_title(
      GTK_WINDOW(ventana),
      "Visor de imágenes");

  gtk_window_set_default_size(
      GTK_WINDOW(ventana),
      700,
      500);

  GtkWidget *scroll = gtk_scrolled_window_new();

  GtkWidget *imagen =
      gtk_picture_new_for_filename(ruta);

  gtk_picture_set_can_shrink(GTK_PICTURE(imagen), TRUE);

  gtk_picture_set_content_fit(
      GTK_PICTURE(imagen),
      GTK_CONTENT_FIT_CONTAIN);

  gtk_scrolled_window_set_child(
      GTK_SCROLLED_WINDOW(scroll),
      imagen);

  gtk_window_set_child(
      GTK_WINDOW(ventana),
      scroll);

  gtk_window_present(GTK_WINDOW(ventana));
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

    GFile *archivoCuadricula = g_file_get_child(directorio, nombre);
    char *rutaCuadricula = g_file_get_path(archivoCuadricula);

    g_object_set_data_full(
        G_OBJECT(elemento),
        "ruta",
        rutaCuadricula,
        g_free);

    g_object_unref(archivoCuadricula);

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

static void mostrarContenidoArchivo(const char *ruta)
{
  char *contenido = abrirArchivoEnRuta(ruta);

  if (contenido == NULL)
  {
    return;
  }

  GtkWidget *ventana = gtk_window_new();

  gtk_window_set_title(
      GTK_WINDOW(ventana),
      ruta);

  gtk_window_set_default_size(
      GTK_WINDOW(ventana),
      650,
      450);

  GtkWidget *scroll = gtk_scrolled_window_new();

  GtkWidget *texto = gtk_text_view_new();

  gtk_text_view_set_editable(GTK_TEXT_VIEW(texto), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(texto), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(texto), GTK_WRAP_WORD_CHAR);

  GtkTextBuffer *buffer =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(texto));

  gtk_text_buffer_set_text(buffer, contenido, -1);

  gtk_scrolled_window_set_child(
      GTK_SCROLLED_WINDOW(scroll),
      texto);

  gtk_window_set_child(GTK_WINDOW(ventana), scroll);

  gtk_window_present(GTK_WINDOW(ventana));

  free(contenido);
}

static void activarElemento(const char *ruta)
{
  if (ruta == NULL)
  {
    return;
  }

  seleccionarElemento(ruta);

  GFile *archivo = g_file_new_for_path(ruta);

  GFileType tipo = g_file_query_file_type(
      archivo,
      G_FILE_QUERY_INFO_NONE,
      NULL);

  if (tipo == G_FILE_TYPE_DIRECTORY)
  {

    g_free(rutaActual);
    rutaActual = g_strdup(ruta);

    g_free(rutaSeleccionada);
    rutaSeleccionada = NULL;

    gtk_label_set_text(
        GTK_LABEL(etiquetaRuta),
        rutaActual);

    cargarDirectorio(rutaActual);
  }
  else if (tipo == G_FILE_TYPE_REGULAR)
  {

    char *tipoContenido = g_content_type_guess(
        ruta,
        NULL,
        0,
        NULL);

    char *mime = g_content_type_get_mime_type(
        tipoContenido);

    if (mime != NULL && g_str_has_prefix(mime, "text/"))
    {

      mostrarContenidoArchivo(ruta);
    }
    else
    {

      char *uri = g_file_get_uri(archivo);
      GError *error = NULL;

      if (!g_app_info_launch_default_for_uri(
              uri, NULL, &error))
      {

        g_warning(
            "No se pudo abrir el archivo: %s",
            error->message);

        g_clear_error(&error);
      }

      g_free(uri);
    }

    g_free(mime);
    g_free(tipoContenido);
  }

  g_object_unref(archivo);
}

static void alActivarCuadricula(GtkFlowBox *cuadricula, GtkFlowBoxChild *hijo, gpointer datos)
{
  (void)cuadricula;
  (void)datos;

  GtkWidget *elemento =
      gtk_flow_box_child_get_child(hijo);

  const char *ruta = g_object_get_data(
      G_OBJECT(elemento),
      "ruta");

  activarElemento(ruta);
}

static GtkWidget *envolverEnScroll(GtkWidget *hijo)
{
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), hijo);
  gtk_widget_set_vexpand(scroll, TRUE);
  return scroll;
}

static void alActivarFila(GtkListBox *lista, GtkListBoxRow *fila, gpointer datos)
{
  (void)lista;
  (void)datos;

  GtkWidget *contenido =
      gtk_list_box_row_get_child(fila);

  const char *ruta = g_object_get_data(
      G_OBJECT(contenido),
      "ruta");

  activarElemento(ruta);
}

static void alSeleccionarCuadricula(GtkFlowBox *cuadricula, GtkFlowBoxChild *hijo, gpointer datos)
{
  (void)cuadricula;
  (void)datos;

  GtkWidget *elemento = gtk_flow_box_child_get_child(hijo);

  const char *ruta = g_object_get_data(
      G_OBJECT(elemento),
      "ruta");

  if (ruta != NULL)
  {
    seleccionarElemento(ruta);
  }
}
/*
static void alCambiarSeleccionCuadricula(GtkFlowBox *cuadricula, gpointer datos)
{
  (void)datos;

  GList *seleccionados =
      gtk_flow_box_get_selected_children(cuadricula);

  if (seleccionados == NULL)
  {
    return;
  }

  GtkFlowBoxChild *hijo = seleccionados->data;

  GtkWidget *elemento =
      gtk_flow_box_child_get_child(hijo);

  const char *ruta =
      g_object_get_data(G_OBJECT(elemento), "ruta");

  if (ruta != NULL)
  {
    seleccionarElemento(ruta);
  }

  g_list_free(seleccionados);
}
*/
static void alSeleccionarFila(GtkListBox *lista, GtkListBoxRow *fila, gpointer datos)
{
  (void)lista;
  (void)datos;
  if (fila == NULL)
    return;
  const char *ruta = g_object_get_data(
      G_OBJECT(gtk_list_box_row_get_child(fila)), "ruta");
  if (ruta != NULL)
    seleccionarElemento(ruta);
}

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

  g_signal_connect(contenedorLista, "row-selected",
                   G_CALLBACK(alSeleccionarFila), NULL);
  gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(contenedorLista), FALSE);
  return contenedorLista;
}

GtkWidget *crearVistaCuadricula(void)
{
  contenedorCuadricula = gtk_flow_box_new();

  gtk_list_box_set_selection_mode(GTK_LIST_BOX(contenedorCuadricula), GTK_SELECTION_SINGLE);
  g_signal_connect(
      contenedorCuadricula,
      "row-activated",
      G_CALLBACK(alActivarFila),
      NULL);

  g_signal_connect(contenedorCuadricula, "row-selected",
                   G_CALLBACK(alSeleccionarFila), NULL);
  gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(contenedorCuadricula), FALSE);
  return contenedorCuadricula;
}

static void alVolverAtras(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  if (rutaActual == NULL)
  {
    return;
  }

  GFile *directorio = g_file_new_for_path(rutaActual);
  GFile *padre = g_file_get_parent(directorio);

  if (padre == NULL)
  {
    g_object_unref(directorio);
    return;
  }

  char *nuevaRuta = g_file_get_path(padre);

  if (nuevaRuta != NULL)
  {
    g_free(rutaActual);
    rutaActual = nuevaRuta;

    g_free(rutaSeleccionada);
    rutaSeleccionada = NULL;

    gtk_label_set_text(GTK_LABEL(etiquetaRuta), rutaActual);
    cargarDirectorio(rutaActual);
  }

  g_object_unref(padre);
  g_object_unref(directorio);
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
  g_signal_connect(
      botonAtras,
      "clicked",
      G_CALLBACK(alVolverAtras),
      NULL);
  etiquetaRuta = gtk_label_new("E:\\");
  gtk_widget_add_css_class(botonAtras, "boton-volver");

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
  gtk_stack_add_named(GTK_STACK(vistaArchivosStack),
                      envolverEnScroll(crearVistaLista()), "lista");
  gtk_stack_add_named(GTK_STACK(vistaArchivosStack),
                      envolverEnScroll(crearVistaCuadricula()), "cuadricula");
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

static void alConfirmarCrear(GtkDialog *dialogo, int respuesta, gpointer datos)
{
  (void)datos;

  if (respuesta == GTK_RESPONSE_ACCEPT)
  {

    GtkWidget *contenido =
        gtk_dialog_get_content_area(dialogo);

    GtkWidget *entrada =
        gtk_widget_get_first_child(contenido);

    const char *nombre =
        gtk_editable_get_text(GTK_EDITABLE(entrada));

    if (nombre[0] == '\0')
    {
      gtk_window_destroy(GTK_WINDOW(dialogo));
      return;
    }

    char *ruta = g_build_filename(
        rutaActual,
        nombre,
        NULL);

    int resultado = crearArchivosEnRuta(ruta);

    if (resultado == 1)
    {
      cargarDirectorio(rutaActual);
    }

    g_free(ruta);
  }

  gtk_window_destroy(GTK_WINDOW(dialogo));
}

static void alCrearArchivo(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  GtkWidget *dialogo = gtk_dialog_new_with_buttons(
      "Crear archivo",
      NULL,
      GTK_DIALOG_MODAL,
      "Cancelar", GTK_RESPONSE_CANCEL,
      "Crear", GTK_RESPONSE_ACCEPT,
      NULL);

  GtkWidget *contenido =
      gtk_dialog_get_content_area(GTK_DIALOG(dialogo));

  GtkWidget *entrada = gtk_entry_new();

  gtk_entry_set_placeholder_text(
      GTK_ENTRY(entrada),
      "Nombre del archivo");

  gtk_box_append(GTK_BOX(contenido), entrada);

  g_signal_connect(
      dialogo,
      "response",
      G_CALLBACK(alConfirmarCrear),
      NULL);

  gtk_window_present(GTK_WINDOW(dialogo));
}

static void alConfirmarCrearCarpeta(GtkDialog *dialogo, int respuesta, gpointer datos)
{
  (void)datos;

  if (respuesta == GTK_RESPONSE_ACCEPT)
  {
    GtkWidget *contenido =
        gtk_dialog_get_content_area(dialogo);

    GtkWidget *entrada =
        gtk_widget_get_first_child(contenido);

    const char *nombre =
        gtk_editable_get_text(GTK_EDITABLE(entrada));

    if (nombre[0] != '\0')
    {
      char *ruta = g_build_filename(
          rutaActual,
          nombre,
          NULL);

      int resultado = crearCarpetaEnRuta(ruta);

      if (resultado == 1)
      {
        cargarDirectorio(rutaActual);
      }

      g_free(ruta);
    }
  }

  gtk_window_destroy(GTK_WINDOW(dialogo));
}

static void alCrearCarpeta(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  GtkWidget *dialogo = gtk_dialog_new_with_buttons(
      "Crear carpeta",
      NULL,
      GTK_DIALOG_MODAL,
      "Cancelar", GTK_RESPONSE_CANCEL,
      "Crear", GTK_RESPONSE_ACCEPT,
      NULL);

  GtkWidget *contenido =
      gtk_dialog_get_content_area(GTK_DIALOG(dialogo));

  GtkWidget *entrada = gtk_entry_new();

  gtk_entry_set_placeholder_text(
      GTK_ENTRY(entrada),
      "Nombre de la carpeta");

  gtk_box_append(GTK_BOX(contenido), entrada);

  g_signal_connect(
      dialogo,
      "response",
      G_CALLBACK(alConfirmarCrearCarpeta),
      NULL);

  gtk_window_present(GTK_WINDOW(dialogo));
}

static void alConfirmarMover(GtkDialog *dialogo, int respuesta, gpointer datos)
{
  (void)datos;

  if (respuesta == GTK_RESPONSE_ACCEPT)
  {
    GtkWidget *contenido =
        gtk_dialog_get_content_area(dialogo);

    GtkWidget *entrada =
        gtk_widget_get_first_child(contenido);

    const char *destinoDir =
        gtk_editable_get_text(GTK_EDITABLE(entrada));

    if (rutaSeleccionada == NULL || destinoDir[0] == '\0')
    {
      gtk_window_destroy(GTK_WINDOW(dialogo));
      return;
    }

    // Comprobar que el destino sea una carpeta
    GFile *directorio = g_file_new_for_path(destinoDir);

    GFileType tipo = g_file_query_file_type(
        directorio,
        G_FILE_QUERY_INFO_NONE,
        NULL);

    if (tipo != G_FILE_TYPE_DIRECTORY)
    {
      g_warning("La carpeta de destino no existe.");
      g_object_unref(directorio);
      gtk_window_destroy(GTK_WINDOW(dialogo));
      return;
    }

    char *nombre = g_path_get_basename(rutaSeleccionada);

    char *destino = g_build_filename(
        destinoDir,
        nombre,
        NULL);

    int resultado = moverArchivosEnRuta(
        rutaSeleccionada,
        destino);

    if (resultado == 1)
    {
      g_free(rutaSeleccionada);
      rutaSeleccionada = NULL;

      cargarDirectorio(rutaActual);
    }

    g_free(nombre);
    g_free(destino);
    g_object_unref(directorio);
  }

  gtk_window_destroy(GTK_WINDOW(dialogo));
}

static void alMoverElemento(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  if (rutaSeleccionada == NULL)
  {
    g_warning("Seleccioná un archivo o carpeta primero.");
    return;
  }

  GtkWidget *dialogo = gtk_dialog_new_with_buttons(
      "Mover elemento",
      NULL,
      GTK_DIALOG_MODAL,
      "Cancelar", GTK_RESPONSE_CANCEL,
      "Mover", GTK_RESPONSE_ACCEPT,
      NULL);

  GtkWidget *contenido =
      gtk_dialog_get_content_area(GTK_DIALOG(dialogo));

  GtkWidget *entrada = gtk_entry_new();

  gtk_entry_set_placeholder_text(
      GTK_ENTRY(entrada),
      "Ruta de la carpeta de destino");

  gtk_box_append(GTK_BOX(contenido), entrada);

  g_signal_connect(
      dialogo,
      "response",
      G_CALLBACK(alConfirmarMover),
      NULL);

  gtk_window_present(GTK_WINDOW(dialogo));
}

static void alConfirmarRenombrar(GtkDialog *dialogo, int respuesta, gpointer datos)
{
  (void)datos;

  if (respuesta == GTK_RESPONSE_ACCEPT)
  {
    GtkWidget *contenido =
        gtk_dialog_get_content_area(dialogo);

    GtkWidget *entrada =
        gtk_widget_get_first_child(contenido);

    const char *nuevoNombre =
        gtk_editable_get_text(GTK_EDITABLE(entrada));

    if (rutaSeleccionada == NULL || nuevoNombre[0] == '\0')
    {
      gtk_window_destroy(GTK_WINDOW(dialogo));
      return;
    }

    char *directorio = g_path_get_dirname(rutaSeleccionada);

    char *nuevaRuta = g_build_filename(
        directorio,
        nuevoNombre,
        NULL);

    int resultado = renombrarArchivos(
        rutaSeleccionada,
        nuevaRuta);

    if (resultado == 1)
    {
      g_free(rutaSeleccionada);
      rutaSeleccionada = NULL;

      cargarDirectorio(rutaActual);
    }

    g_free(directorio);
    g_free(nuevaRuta);
  }

  gtk_window_destroy(GTK_WINDOW(dialogo));
}

static void alRenombrarElemento(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  if (rutaSeleccionada == NULL)
  {
    g_warning("Seleccioná un archivo o carpeta primero.");
    return;
  }

  GtkWidget *dialogo = gtk_dialog_new_with_buttons(
      "Renombrar elemento",
      NULL,
      GTK_DIALOG_MODAL,
      "Cancelar", GTK_RESPONSE_CANCEL,
      "Renombrar", GTK_RESPONSE_ACCEPT,
      NULL);

  GtkWidget *contenido =
      gtk_dialog_get_content_area(GTK_DIALOG(dialogo));

  GtkWidget *entrada = gtk_entry_new();

  char *nombreActual = g_path_get_basename(rutaSeleccionada);

  gtk_editable_set_text(GTK_EDITABLE(entrada), nombreActual);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entrada), "Nuevo nombre");

  g_free(nombreActual);

  gtk_box_append(GTK_BOX(contenido), entrada);

  g_signal_connect(
      dialogo,
      "response",
      G_CALLBACK(alConfirmarRenombrar),
      NULL);

  gtk_window_present(GTK_WINDOW(dialogo));
}

static void alConfirmarEliminar(GtkDialog *dialogo, int respuesta, gpointer datos)
{
  (void)datos;

  if (respuesta == GTK_RESPONSE_ACCEPT)
  {
    if (rutaSeleccionada == NULL)
    {
      gtk_window_destroy(GTK_WINDOW(dialogo));
      return;
    }

    int resultado = borrarArchivosEnRuta(rutaSeleccionada);

    if (resultado == 1)
    {
      g_free(rutaSeleccionada);
      rutaSeleccionada = NULL;

      cargarDirectorio(rutaActual);
    }
  }

  gtk_window_destroy(GTK_WINDOW(dialogo));
}

static void alEliminarElemento(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  if (rutaSeleccionada == NULL){
    g_warning("Seleccioná un archivo o carpeta primero.");
    return;
  }

  GtkWidget *dialogo = gtk_message_dialog_new(
      NULL,
      GTK_DIALOG_MODAL,
      GTK_MESSAGE_WARNING,
      GTK_BUTTONS_NONE,
      "¿Enviar este elemento a la papelera?");

  gtk_dialog_add_buttons(
      GTK_DIALOG(dialogo),
      "Cancelar", GTK_RESPONSE_CANCEL,
      "Eliminar", GTK_RESPONSE_ACCEPT,
      NULL);

  g_signal_connect(
      dialogo,
      "response",
      G_CALLBACK(alConfirmarEliminar),
      NULL);

  gtk_window_present(GTK_WINDOW(dialogo));
}

static void alVerPapelera(GtkButton *boton, gpointer datos)
{
  (void)boton;
  (void)datos;

  iniciarPapelera();

  g_free(rutaActual);
  rutaActual = g_strdup(".papelera");

  g_free(rutaSeleccionada);
  rutaSeleccionada = NULL;

  gtk_label_set_text(
      GTK_LABEL(etiquetaRuta),
      "Papelera");

  cargarDirectorio(rutaActual);
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


  gtk_widget_add_css_class(inicio, "boton-menu");
  gtk_widget_add_css_class(documentos, "boton-menu");


  gtk_box_append(GTK_BOX(panel), inicio);
  gtk_box_append(GTK_BOX(panel), documentos);


  // div
  GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

  gtk_box_append(GTK_BOX(panel), separador);
  GtkWidget *acciones = gtk_label_new("ACCIONES");
  gtk_widget_add_css_class(acciones, "seccion");

  GtkWidget *btnCrearArchivo = gtk_button_new_with_label("+ Crear archivo");

  g_signal_connect(
      btnCrearArchivo,
      "clicked",
      G_CALLBACK(alCrearArchivo),
      NULL);

  GtkWidget *btnCrearCarpeta =
      gtk_button_new_with_label("+ Crear carpeta");

  g_signal_connect(
      btnCrearCarpeta,
      "clicked",
      G_CALLBACK(alCrearCarpeta),
      NULL);

  GtkWidget *btnMoverArchivo = gtk_button_new_with_label(" Mover Elemento");

  g_signal_connect(
      btnMoverArchivo,
      "clicked",
      G_CALLBACK(alMoverElemento),
      NULL);
  GtkWidget *btnRenombrar =
      gtk_button_new_with_label("Renombrar elemento");

  g_signal_connect(
      btnRenombrar,
      "clicked",
      G_CALLBACK(alRenombrarElemento),
      NULL);
  GtkWidget *btnEliminar = gtk_button_new_with_label("Eliminar elemento");

  g_signal_connect(
      btnEliminar,
      "clicked",
      G_CALLBACK(alEliminarElemento),
      NULL);
  GtkWidget *btnPapelera =
      gtk_button_new_with_label("🗑 Papelera");

  g_signal_connect(
      btnPapelera,
      "clicked",
      G_CALLBACK(alVerPapelera),
      NULL);

  gtk_widget_add_css_class(btnCrearArchivo, "boton-menu");
  gtk_widget_add_css_class(btnCrearCarpeta, "boton-menu");
  gtk_widget_add_css_class(btnMoverArchivo, "boton-menu");
  gtk_widget_add_css_class(btnRenombrar, "boton-menu");
  gtk_widget_add_css_class(btnEliminar, "boton-menu");
  gtk_widget_add_css_class(btnPapelera, "boton-menu");

  gtk_box_append(GTK_BOX(panel), btnCrearArchivo);
  gtk_box_append(GTK_BOX(panel), btnCrearCarpeta);
  gtk_box_append(GTK_BOX(panel), btnMoverArchivo);
  gtk_box_append(GTK_BOX(panel), btnRenombrar);
  gtk_box_append(GTK_BOX(panel), btnEliminar);
  gtk_box_append(GTK_BOX(panel), btnPapelera);

  return panel;
}
