#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOMBRE 255

//void crearArchivo(void){
void crearArchivo(void) {
    char nombre[MAX_NOMBRE];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        return 1;
    }

    FILE *archivo = fopen(nombre, "wx"); //wx escribe o ejecuta

    if (archivo == NULL) {
        perror("No se pudo crear el archivo");
        return 1;
    }

    fclose(archivo);

    printf("Archivo creado correctamente: %s\n");

    return 0;
}

void borrarArchivo(void){ // lo mueve a la papelera por 15 dias (el programa no se necesita mantener abierto, cuando se vuelva a abrir verifica el tiempo)

    char nombre[MAX_NOMBRE];
    //variable de papelera(destino) y id(id del archivo a mover)
    char destino[MAX_NOMBRE + 32];
    char id[32];
    
    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL){
        return;
    }
    
    //guarda el dia y hora del borrado
    time_t tiempoActual = time(NULL);
    struct tm *fecha = localtime(&tiempoActual);

    //en caso de que no se pueda obtener localtime
    if(fecha == NULL){
        printf ("no se puede obtener la fecha");
        return 1;
    }

    strftime(id, sizeof(id), "%Y%m%d_%H%M%S",fecha);

    snprintf(destino, sizeof(destino), ".papelera/%s_%s",id,nombre);

    //por si no se puede mover el archivo
    if (rename(nombre,destino) != 0){
        perror("No puede moverse el archivo");
        return;
    }

    FILE *registro = fopen(".papelera/registro.txt", "a");

    //por si no se exite el archivo dentro de la papelera o no registra fecha
    if (registro==NULL) {
        perror("no se pudo abrir el registro");
        printf("Archivo en papelera pero no registro fecha.\n");
        return;
    }

    fprintf(registro, "%s|%s|%lld\n", nombre, destino,(long long)tiempoActual);

    fclose(registro);

    printf("archivo movido a la papelera por 15 dias");
}

void renombrarArchivo(void){
    char nombre[MAX_NOMBRE];
    char nuevoNombre[MAX_NOMBRE];

    printf("NombreActual: ");
    if (fgets(nuevoNombre, sizeof(nuevoNombre), stdin) == NULL) return 1;
    if( nombre[0] == '\0' || nuevoNombre[0]== '\0'){
        printf("los nombres no pueden estar vacios.\n");
        return 1;
    }

    if (rename(nombre,nuevoNombre) == 0){
        printf("archivo renombrado correctamente.\n");
    } else {
        perror("No se pudo renombrar le archivo");
    }
}

void moverArchivo(void){

    char nombre[MAX_NOMBRE];
    char destino[MAX_NOMBRE];
    char origen[MAX_NOMBRE];

    printf("Archivo a mover: ");
    if(fgets(origen, sizeof(origen), stdin) == NULL) return 1;

    printf("ruta destino: ");
    if(fgets(destino, sizeof(destino), stdin) == NULL) return 1;
    
    if(origen[0] == '\0'|| destino[0]=='\0'){
        printf("origen ni destino puede estar vacio.\n");
        return 1;
    }

    if(rename(origen, destino) == 0){
        printf("Archivo movido correctamente.\n");
    } else {
        perror("El archivo no pudo moverse");
    }
}

// no me termina de convencer por que mostraria su contenido por consola
void abrirArchivo(void) {
    char nombre[MAX_NOMBRE];
    char linea[1024];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        return;
    }


    FILE *archivo = fopen(nombre, "r");

    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        printf("%s", linea);
    }

    fclose(archivo);

}

//void copiarArchivo(void){} //o duplicar
//void listarArchivos (void){}
//void crearCarpeta (void){}

//void listarArchivos(void){}
//void listarCarpetas(void){}

//podriamos hacer que cree backups
// y que muestre la auditorias hasta ese momento