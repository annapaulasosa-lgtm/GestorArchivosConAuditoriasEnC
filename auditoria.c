#include <stdio.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "auditoria.h"

#define ARCHIVO_LOG "auditoria.log"

/* Reemplaza caracteres que romperían el formato del log */
static char *limpiarTexto(const char *texto)
{
    char *copia = g_strdup(texto != NULL ? texto : "");
    for (char *p = copia; *p != '\0'; p++) {
        if (*p == '|')
            *p = '/';
        else if (*p == '\n' || *p == '\r')
            *p = ' ';
    }
    return copia;
}

void registrarAuditoria(const char *operacion,
                        const char *detalle,
                        const char *resultado)
{
    FILE *archivo = g_fopen(ARCHIVO_LOG, "a");
    if (archivo == NULL) {
        perror("Error al abrir auditoria.log");
        return;
    }

    char fechaTexto[32] = "fecha_desconocida";
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);
    if (fecha != NULL &&
        strftime(fechaTexto, sizeof fechaTexto, "%Y-%m-%d %H:%M:%S", fecha) == 0)
        strcpy(fechaTexto, "fecha_desconocida");

    char *op  = limpiarTexto(operacion);
    char *det = limpiarTexto(detalle);
    char *res = limpiarTexto(resultado);

    fprintf(archivo, "%s | Usuario: %s | %s | %s | %s\n",
            fechaTexto, g_get_user_name(), op, det, res);

    g_free(op);
    g_free(det);
    g_free(res);

    if (fclose(archivo) != 0)
        perror("Error al cerrar auditoria.log");
}

/* Devuelve todo el log (liberar con g_free) o NULL si no existe todavia */
char *leerAuditoria(void)
{
    char *contenido = NULL;
    if (!g_file_get_contents(ARCHIVO_LOG, &contenido, NULL, NULL))
        return NULL;
    return contenido;
}