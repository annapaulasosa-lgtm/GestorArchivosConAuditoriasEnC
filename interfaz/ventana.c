#include <gtk/gtk.h>
#include "ventana.h"
#include <gio/gio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/statvfs.h>
#endif

// creamos la barra lateral
static GtkWidget *crearPanelNavegacion(void)
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

static GtkWidget *contenedorLista;
static GtkWidget *contenedorCuadricula;
static GtkWidget *vistaArchivosStack;
static GtkWidget *etiquetaRuta;
static char *rutaActual = NULL;

// Prototipos de funciones
static GtkWidget *crearVistaLista(void);
static GtkWidget *crearVistaCuadricula(void);
static void cargarDirectorio(const char *ruta);
static void limpiarLista(GtkWidget *lista);
static void limpiarContenedor(GtkWidget *contenedor);

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

// crea los 2 tipos
static GtkWidget *crearVistaLista(void)
{

  contenedorLista = gtk_list_box_new();

  gtk_list_box_set_selection_mode(GTK_LIST_BOX(contenedorLista), GTK_SELECTION_SINGLE);

  return contenedorLista;
}

static GtkWidget *crearVistaCuadricula(void)
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

static GtkWidget *crearExploradorArchivos(void)
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

// vista asistente
static GtkWidget *crearPanelRobot(void)
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
  imagen = gtk_picture_new_for_filename("assets/robot-img.jpeg");
  ;
  gtk_widget_add_css_class(imagen, "robot-imagen");
  gtk_box_append(GTK_BOX(panel), imagen);

  mensaje = gtk_label_new(
      "Hola, soy tu compañero, en que puedo ayudarte?");
  gtk_label_set_wrap(GTK_LABEL(mensaje), TRUE);
  gtk_box_append(GTK_BOX(panel), mensaje);

  return panel;
}

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

static GtkWidget *crearAsistenteFlotante(void)
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

static gboolean obtenerUsoDisco(guint64 *total, guint64 *libre)
{
#ifdef _WIN32

  char ruta[MAX_PATH];
  char unidad[] = "E:\\";

  DWORD longitud = GetModuleFileNameA(
      NULL, ruta, MAX_PATH);

  if (longitud == 0 || longitud >= MAX_PATH)
  {
    g_warning("No se pudo obtener la ruta del ejecutable");
    return FALSE;
  }

  // Extraer la unidad del ejecutable
  unidad[0] = ruta[0];

  ULARGE_INTEGER espacioLibre;
  ULARGE_INTEGER espacioTotal;

  if (!GetDiskFreeSpaceExA(
          unidad,
          NULL,
          &espacioTotal,
          &espacioLibre))
  {
    g_warning("GetDiskFreeSpaceExA fallo para %s. Error: %lu",
              unidad, GetLastError());
    return FALSE;
  }

  *total = espacioTotal.QuadPart;
  *libre = espacioLibre.QuadPart;

  return TRUE;

#else

  struct statvfs datos;

  if (statvfs(".", &datos) != 0)
  {
    return FALSE;
  }

  *total = (guint64)datos.f_blocks * datos.f_frsize;
  *libre = (guint64)datos.f_bavail * datos.f_frsize;

  return TRUE;

#endif
}

// utilizado para probar la funcion de discos
static void probarDisco(void)
{
  ULARGE_INTEGER total;
  ULARGE_INTEGER libre;

  BOOL resultado = GetDiskFreeSpaceExA(
      "E:\\",
      NULL,
      &total,
      &libre);

  if (resultado)
  {
    g_print("DISCO E: CONSULTADO CORRECTAMENTE\n");
    g_print("Total: %s\n", g_format_size(total.QuadPart));
    g_print("Libre: %s\n", g_format_size(libre.QuadPart));
  }
  else
  {
    g_print("ERROR DE WINDOWS: %lu\n", GetLastError());
  }
}

static GtkWidget *crearPanelAlmacenamiento(void)
{

  GtkWidget *panel;
  GtkWidget *titulo;
  GtkWidget *etiquetaTotal;
  GtkWidget *etiquetaUsado;
  GtkWidget *etiquetaLibre;
  GtkWidget *barra;

  guint64 total = 0;
  guint64 libre = 0;

  panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  titulo = gtk_label_new("ALMACENAMIENTO");
  gtk_widget_add_css_class(titulo, "seccion");

  gtk_box_append(GTK_BOX(panel), titulo);

  if (!obtenerUsoDisco(&total, &libre) || total == 0)
  {

    GtkWidget *error = gtk_label_new("No se puede consultar el disco");

    gtk_box_append(GTK_BOX(panel), error);

    return panel;
  }

  guint64 usado = total - libre;

  double porcentaje = (double)usado / (double)total;

  gchar *textoTotal = g_format_size(total);
  gchar *textoUsado = g_format_size(usado);
  gchar *textoLibre = g_format_size(libre);

  etiquetaTotal = gtk_label_new(NULL);
  etiquetaUsado = gtk_label_new(NULL);
  etiquetaLibre = gtk_label_new(NULL);

  gchar *mensajeTotal = g_strdup_printf("capacidadd total: %s", textoTotal);
  gchar *mensajeUsado = g_strdup_printf("Utilizado: %s", textoUsado);
  gchar *mensajeLibre = g_strdup_printf("Libre: %s", textoLibre);

  gtk_label_set_text(GTK_LABEL(etiquetaTotal), mensajeTotal);
  gtk_label_set_text(GTK_LABEL(etiquetaUsado), mensajeUsado);
  gtk_label_set_text(GTK_LABEL(etiquetaLibre), mensajeLibre);

  gtk_label_set_xalign(GTK_LABEL(etiquetaTotal), 0);
  gtk_label_set_xalign(GTK_LABEL(etiquetaUsado), 0);
  gtk_label_set_xalign(GTK_LABEL(etiquetaLibre), 0);

  barra = gtk_progress_bar_new();

  gtk_progress_bar_set_fraction(
      GTK_PROGRESS_BAR(barra), porcentaje);

  gtk_progress_bar_set_show_text(
      GTK_PROGRESS_BAR(barra), TRUE);

  gchar *textoPorcentaje = g_strdup_printf("%.1f%% utilizado", porcentaje * 100);

  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(barra), textoPorcentaje);

  gtk_box_append(GTK_BOX(panel), etiquetaTotal);
  gtk_box_append(GTK_BOX(panel), barra);
  gtk_box_append(GTK_BOX(panel), etiquetaUsado);
  gtk_box_append(GTK_BOX(panel), etiquetaLibre);

  g_free(textoTotal);
  g_free(textoUsado);
  g_free(textoLibre);
  g_free(mensajeTotal);
  g_free(mensajeUsado);
  g_free(mensajeLibre);
  g_free(textoPorcentaje);

  return panel;
}

GtkWidget *crearVentana(GtkApplication *app)
{

  // defino forma del "widget"
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
