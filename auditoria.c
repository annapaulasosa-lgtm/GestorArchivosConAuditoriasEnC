#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auditoria.h"

#ifndef _WIN32

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

const char *obtenerUsuario(void){
    const char *usuario = getenv("USERNAME");

    if (usuario == NULL || usuario[0] == '\0'){
        usuario = getenv("USER");
    }
    if( usuario != NULL && usuario[0] == '\0'){
        return usuario;
    }
    
    #ifndef _WIN32
        struct passwd *datos = getpwuid(getuid());

        if(datos != NULL && datos ->pw_name != NULL) {
            return datos->pw_name;
        };
    #endif

    return "usuario_desconocido";
}

void registrarAuditoria(
    const char *operacion,      
    const char *detalle,           
    const char *resultado)
{
    FILE *archivo = fopen("auditoria.log", "a");

    if(archivo == NULL) {
        perror("error al abrir auditoria.log");
        return;
    }

    time_t horaActual= time(NULL);
    struct tm *fecha = localtime(&horaActual);
    char fechaTexto[32] = "fecha_desconocida";

    if (fecha != NULL){
        if ( strftime(fechaTexto,sizeof(fechaTexto), "%Y-%m-%d  %H:%M:%S", fecha) == 0){
            strcpy(fechaTexto, "fecha_desconocida");
        }
    }

    fprintf(archivo, "%s | Usuario: %s | %s | %s | %s\n",
        fechaTexto, obtenerUsuario(),
        operacion, detalle, 
        resultado);
    
    if (fclose(archivo) != 0){
        perror("Error al cerrar auditoria.log");
    }      
}