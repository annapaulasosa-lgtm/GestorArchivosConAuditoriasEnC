#include <gtk/gtk.h>
#include "../ventana.h"
#include <gio/gio.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/statvfs.h>
#endif

// utilizado para probar la funcion de discos
/*static void probarDisco(void)
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
*/

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

GtkWidget *crearPanelAlmacenamiento(void)
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
