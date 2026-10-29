#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auditoria.h"
#include "procArchivos.h"

#define MAX_NOMBRE 255
void quitarSalto(char texto[]){
    texto[strcspn(texto, "\n")] ='\0';
}

//funcionan :D
void abrirArchivo(void) {
    char nombre[MAX_NOMBRE];
    char linea[1024];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        return;
    }

    quitarSalto(nombre);

    FILE *archivo = fopen(nombre, "r");

    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        registrarAuditoria("ABRIR", nombre, "ERROR");

        return;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        printf("%s", linea);
    }

    fclose(archivo);

}
void crearArchivo(void) {
    char nombre[MAX_NOMBRE];

    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        return;
    }

    quitarSalto(nombre);

    FILE *archivo = fopen(nombre, "r"); //wx escribe o ejecuta
    if (archivo != NULL) {
        fclose(archivo);
        printf("el archivo ya existe.\n");
        return;
    }

    archivo = fopen(nombre,"w");

    if (archivo == NULL) {
        perror("No se pudo crear el archivo");

        registrarAuditoria(
            "CREAR",nombre, "ERROR"
        );
        return;
    }

    if (fclose(archivo) != 0){
        perror("error al cerrar el archivo");
        return; 
    }

    registrarAuditoria("CREAR", nombre, "CORRECTAMENTE");

    printf("Archivo creado correctamente: %s\n", nombre);
    return;
}


//no borra. lo mueve a la papelera por 15 dias 
//(el programa no se necesita mantener abierto, cuando se vuelva a abrir verifica el tiempo)
void borrarArchivo(void){

    char nombre[MAX_NOMBRE];
    //variable de papelera(destino) y id(id del archivo a mover)
    char destino[MAX_NOMBRE + 257];
    char id[32];
    
    printf("Nombre del archivo: ");

    if (fgets(nombre, sizeof(nombre), stdin) == NULL){
        return;
    }
    
    quitarSalto(nombre);

    if (nombre[0] == '\0') {
        printf("El nombre no puede estar vacio.\n");
        return;
    }

    //guarda el dia y hora del borrado
    time_t tiempoActual = time(NULL);
    struct tm *fecha = localtime(&tiempoActual);
  

    //en caso de que no se pueda obtener localtime
    if(fecha == NULL){
        printf ("no se puede obtener la fecha.\n");
        return;
    }

    if (strftime(id, sizeof(id), "%Y%m%d_%H%M%S",fecha)== 0){
        printf("No se pudo generar el id");
        return;
    }

    int resultado = snprintf(destino, sizeof(destino), ".papelera/%s_%s",id,nombre);

    if (resultado < 0 || (size_t)resultado >= sizeof(destino)) {
        printf("La ruta de destino es demasiado largo.\n");
        return;
    }

    //por si no se puede mover el archivo
    if (rename(nombre,destino) != 0){
        perror("No puede moverse el archivo");
        registrarAuditoria(
            "BORRADO",nombre, "ERROR"
        );
        return;
    }

    FILE *registro = fopen(".papelera/registro.txt", "a");

    //por si no se exite el archivo dentro de la papelera o no registra fecha
    if (registro==NULL) {
        perror("no se pudo abrir el registro");
        printf("Archivo en papelera pero no registro fecha.\n");
        registrarAuditoria(
            "BORRADO",nombre, "ERROR"
        );
        return;
    }

    fprintf(registro, "%s|%s|%lld\n", nombre, destino,(long long)tiempoActual);

    if (fclose(registro) != 0){
        perror("error al cerrar le registro");
        return;
    }

    registrarAuditoria(
            "BORRADO",nombre, "CORRECTAMENTE"
        );
    printf("archivo movido a la papelera por 15 dias");
}

//no renombra
void renombrarArchivo(void){
    char nombre[MAX_NOMBRE];
    char nuevoNombre[MAX_NOMBRE];

    printf("Nombre Actual: ");
    if (fgets(nuevoNombre, sizeof(nuevoNombre), stdin) == NULL) return;

    quitarSalto(nombre);

    printf("Nuevo Nombre:");

    if(fgets(nuevoNombre, sizeof(nuevoNombre), stdin) == NULL){
        return;
    }

    quitarSalto(nombre);

    if( nombre[0] == '\0' || nuevoNombre[0]== '\0'){
        printf("los nombres no pueden estar vacios.\n");
        return;
    }
    
    if (rename(nombre,nuevoNombre) == 0){
        registrarAuditoria("RENOMBRAR",nombre, "CORRECTAMENTE");
        printf("archivo renombrado correctamente.\n");

    } else {
        perror("No se pudo renombrar le archivo");
        registrarAuditoria("RENOMBRAR",nombre, "ERROR");
    }
}

//no mueve
void moverArchivo(void){

    char destino[MAX_NOMBRE];
    char origen[MAX_NOMBRE];

    printf("Archivo a mover: ");

    if(fgets(origen, sizeof(origen), stdin) == NULL) return;

    quitarSalto(origen);

    printf("ruta destino: ");

    if(fgets(destino, sizeof(destino), stdin) == NULL) return;

    quitarSalto(origen);
    
    if(origen[0] == '\0'|| destino[0]=='\0'){
        printf("origen ni destino puede estar vacio.\n");
        return;
    }

    if(rename(origen, destino) != 0){
        perror("El archivo no pudo moverse");
        registrarAuditoria("MOVER", origen, "ERROR");
    } 

    registrarAuditoria("MOVER", origen, "CORRECTAMENTE");
    printf("Archivo movido correctamente.\n");
    
}



//void copiarArchivo(void){} //o duplicar
//void crearCarpeta (void){}

//void listarArchivos(void){}
//void listarCarpetas(void){}

//podriamos hacer que cree backups
// y que muestre la auditorias hasta ese momento