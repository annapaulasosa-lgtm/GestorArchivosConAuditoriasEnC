#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auditoria.h"
#include "procArchivos.h"
#include <errno.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(ruta) _mkdir(ruta)
#else
#include <sys/stat.h>
#define MKDIR(ruta) mkdir(ruta, 0755)
#endif

// variable comun
#define MAX_NOMBRE 255

// proc para mejor ejecución
void quitarSalto(char texto[])
{
    texto[strcspn(texto, "\n")] = '\0';
}

void iniciarPapelera(void)
{
    if (MKDIR(".papelera") != 0 && errno != EEXIST)
    {
        perror("No se pudo crear la papelera");
    }
}

// reutilizacion de las funciones de consola para usar con interfaz
int crearArchivosEnRuta(const char *ruta)
{
    if (ruta == NULL || ruta[0] == '\0')
    {
        return -1;
    }

    FILE *archivo = fopen(ruta, "wx");

    if (archivo == NULL)
    {
        if (errno == EEXIST)
        {
            return 0;
        }
        perror("No se pudo crear el archivo");
        registrarAuditoria("CREAR", ruta, "ERROR");

        return -1;
    }
    if (fclose(archivo) != 0)
    {

        perror("Error al cerrar al archivo");

        registrarAuditoria(
            "CREAR", ruta, "ERROR");
        return -1;
    }
    registrarAuditoria("CREAR", ruta, "CORRECTAMENTE");

    return 1;
}

int borrarArchivosEnRuta(const char *ruta)
{
    if (ruta == NULL || ruta[0] == '\0')
    {
        return -1;
    }

    iniciarPapelera();

    time_t tiempoActual = time(NULL);
    struct tm *fecha = localtime(&tiempoActual);

    if (fecha == NULL)
    {
        return -1;
    }

    char id[32];
    char destino[MAX_NOMBRE + 300];

    if (strftime(id, sizeof(id), "%d%m%Y_%H%M%S", fecha) == 0)
    {
        return -1;
    }

    const char *nombre = strrchr(ruta, '/');
    const char *nombreWindows = strrchr(ruta, '\\');

    if (nombreWindows != NULL &&
        (nombre == NULL || nombreWindows > nombre))
    {
        nombre = nombreWindows;
    }

    nombre = (nombre != NULL) ? nombre + 1 : ruta;

    int resultado = snprintf(
        destino,
        sizeof(destino),
        ".papelera/%s_%s",
        id,
        nombre);

    if (resultado < 0 || (size_t)resultado >= sizeof(destino))
    {
        return -1;
    }

    if (rename(ruta, destino) != 0)
    {
        perror("No se pudo mover el archivo a la papelera");
        registrarAuditoria("BORRADO", ruta, "ERROR");
        return -1;
    }

    FILE *registro = fopen(".papelera/registro.txt", "a");

    if (registro == NULL)
    {
        perror("No se pudo abrir el registro");
        return -1;
    }

    fprintf(
        registro,
        "%s|%s|%lld\n",
        ruta,
        destino,
        (long long)tiempoActual);

    fclose(registro);

    registrarAuditoria("BORRADO", ruta, "CORRECTAMENTE");

    return 1;
}

int renombrarArchivos(const char *rutaActual, const char *nuevaRuta)
{
    if (rutaActual == NULL || nuevaRuta == NULL ||
        rutaActual[0] == '\0' || nuevaRuta[0] == '\0')
    {
        return -1;
    }

    if (rename(rutaActual, nuevaRuta) != 0)
    {
        perror("No se pudo renombrar el archivo");
        registrarAuditoria("RENOMBRAR", rutaActual, "ERROR");
        return -1;
    }

    registrarAuditoria("RENOMBRAR", rutaActual, "CORRECTAMENTE");
    return 1;
}

int moverArchivosEnRuta(const char *origen, const char *destino)
{
    if (origen == NULL || destino == NULL ||
        origen[0] == '\0' || destino[0] == '\0')
    {
        return -1;
    }

    if (rename(origen, destino) != 0)
    {
        perror("No se pudo mover el archivo");
        registrarAuditoria("MOVER", origen, "ERROR");
        return -1;
    }

    registrarAuditoria("MOVER", origen, "CORRECTAMENTE");
    return 1;
}

char *abrirArchivoEnRuta(const char *ruta)
{
    if (ruta == NULL || ruta[0] == '\0')
    {
        return NULL;
    }

    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL)
    {
        perror("No se pudo abrir el archivo");
        registrarAuditoria("ABRIR", ruta, "ERROR");
        return NULL;
    }

    if (fseek(archivo, 0, SEEK_END) != 0)
    {
        fclose(archivo);
        return NULL;
    }

    long tamanio = ftell(archivo);

    if (tamanio < 0 || fseek(archivo, 0, SEEK_SET) != 0)
    {
        fclose(archivo);
        return NULL;
    }

    char *contenido = malloc((size_t)tamanio + 1);

    if (contenido == NULL)
    {
        fclose(archivo);
        return NULL;
    }

    size_t leidos = fread(contenido, 1, (size_t)tamanio, archivo);

    if (ferror(archivo))
    {
        free(contenido);
        fclose(archivo);
        return NULL;
    }

    contenido[leidos] = '\0';

    fclose(archivo);

    return contenido;
}

int crearCarpetaEnRuta(const char *ruta)
{
    if (ruta == NULL || ruta[0] == '\0')
    {
        return -1;
    }

    int resultado = MKDIR(ruta);

    if (resultado != 0)
    {
        if (errno == EEXIST)
        {
            return 0;
        }

        perror("No se pudo crear la carpeta");
        registrarAuditoria("CREAR CARPETA", ruta, "ERROR");
        return -1;
    }

    registrarAuditoria("CREAR CARPETA", ruta, "CORRECTAMENTE");

    return 1;
}
// fuinciones de consolas
void abrirArchivo(void)
{
    char nombre[MAX_NOMBRE];
    char linea[1024];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL)
    {
        return;
    }

    quitarSalto(nombre);

    FILE *archivo = fopen(nombre, "r");

    if (archivo == NULL)
    {
        perror("No se pudo abrir el archivo");
        registrarAuditoria("ABRIR", nombre, "ERROR");

        return;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        printf("%s", linea);
    }

    fclose(archivo);
}

void crearArchivo(void)
{
    char nombre[MAX_NOMBRE];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL)
    {
        return;
    }

    quitarSalto(nombre);

    int resultado = crearArchivosEnRuta(nombre);

    switch (resultado)
    {

    case 1:
        printf("Archivo creado correctamente: %s\n", nombre);
        break;

    case 0:
        printf("El archivo ya existe.\n");
        break;

    case -1:
        printf("No se pudo crear el archivo.\n");
        break;
    }
}

//(el programa no se necesita mantener abierto, cuando se vuelva a abrir verifica el tiempo)
void borrarArchivo(void)
{

    char nombre[MAX_NOMBRE];
    // variable de papelera(destino) y id(id del archivo a mover)
    char destino[MAX_NOMBRE + 257];
    char id[32];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL)
    {
        return;
    }

    quitarSalto(nombre);

    if (nombre[0] == '\0')
    {
        printf("El nombre no puede estar vacio.\n");
        return;
    }

    // guarda el dia y hora del borrado
    time_t tiempoActual = time(NULL);
    struct tm *fecha = localtime(&tiempoActual);

    // en caso de que no se pueda obtener localtime
    if (fecha == NULL)
    {
        printf("no se puede obtener la fecha.\n");
        return;
    }

    if (strftime(id, sizeof(id), "%d%m%Y_%H%M%S", fecha) == 0)
    {
        printf("No se pudo generar el id");
        return;
    }

    int resultado = snprintf(destino, sizeof(destino), ".papelera/%s_%s", id, nombre);

    if (resultado < 0 || (size_t)resultado >= sizeof(destino))
    {
        printf("La ruta de destino es demasiado largo.\n");
        return;
    }

    // por si no se puede mover el archivo
    if (rename(nombre, destino) != 0)
    {
        perror("No puede moverse el archivo");
        registrarAuditoria(
            "BORRADO", nombre, "ERROR");
        return;
    }

    FILE *registro = fopen(".papelera/registro.txt", "a");

    // por si no se exite el archivo dentro de la papelera o no registra fecha
    if (registro == NULL)
    {
        perror("no se pudo abrir el registro");
        printf("Archivo en papelera pero no registro fecha.\n");
        registrarAuditoria(
            "BORRADO", nombre, "ERROR");
        return;
    }

    fprintf(registro, "%s|%s|%lld\n", nombre, destino, (long long)tiempoActual);

    if (fclose(registro) != 0)
    {
        perror("error al cerrar le registro");
        return;
    }

    registrarAuditoria(
        "BORRADO", nombre, "CORRECTAMENTE");
    printf("archivo movido a la papelera por 15 dias.\n");
}

// no renombra
void renombrarArchivo(void)
{
    char nombre[MAX_NOMBRE];
    char nuevoNombre[MAX_NOMBRE];

    printf("Nombre Actual: ");
    if (fgets(nombre, sizeof(nombre), stdin) == NULL)
        return;

    quitarSalto(nombre);

    printf("Nuevo Nombre:");

    if (fgets(nuevoNombre, sizeof(nuevoNombre), stdin) == NULL)
        return;

    quitarSalto(nuevoNombre);

    if (nombre[0] == '\0' || nuevoNombre[0] == '\0')
    {
        printf("los nombres no pueden estar vacios.\n");
        return;
    }

    if (rename(nombre, nuevoNombre) == 0)
    {
        registrarAuditoria("RENOMBRAR", nombre, "CORRECTAMENTE");
        printf("archivo renombrado correctamente.\n");
    }
    else
    {
        perror("No se pudo renombrar le archivo");
        registrarAuditoria("RENOMBRAR", nombre, "ERROR");
    }
}

// no mueve
void moverArchivo(void)
{

    char destino[MAX_NOMBRE];
    char origen[MAX_NOMBRE];

    printf("Archivo a mover: ");

    if (fgets(origen, sizeof(origen), stdin) == NULL)
        return;

    quitarSalto(origen);

    printf("ruta destino: ");

    if (fgets(destino, sizeof(destino), stdin) == NULL)
        return;

    quitarSalto(destino);

    if (origen[0] == '\0' || destino[0] == '\0')
    {
        printf("origen ni destino puede estar vacio.\n");
        return;
    }

    if (rename(origen, destino) != 0)
    {
        perror("El archivo no pudo moverse");
        registrarAuditoria("MOVER", origen, "ERROR");
        return;
    }

    registrarAuditoria("MOVER", origen, "CORRECTAMENTE");
    printf("Archivo movido correctamente.\n");
}
