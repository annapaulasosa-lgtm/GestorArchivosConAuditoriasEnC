//#include <time.h>
//#include <sys/stat.h>  //revisar
//#include <unistd.h> //revisar
//#include <errno.h> //revisar
//
//#include <procArchivos.c>
//#include <auditor>
//#include <ventana>

/*void inciarPapelera(void){
    if (mkdir(".papelera") == -1 && errno != EEXIST) {
        perror("No se pudo crear la papelera");
    }
}
//tiene que guardar la ruta vieja del archivo y hacer la funcion .remove despues de los 15dias
*/

#include <stdio.h>
#include "procArchivos.h"

int main(void){
    int opcion;
    char entrada[20];

    do{
        printf("\n====== GESTOR DE ARCHIVOS =======\n");        
        printf("1. Crear archivo\n");
        printf("2. abrir archivo\n");
        printf("3. borrar archivo\n");
        printf("4. mover archivo\n");
        printf("5. renombrar archivo\n");


        if (fgets(entrada, sizeof(entrada), stdin) == NULL){
            break;
        }
        if (sscanf(entrada, "%d", &opcion) != 1){
            printf("Ingrese una opcion valida.\n");
            continue;
        }
        switch (opcion){
            case 1:
                crearArchivo();
                break;
            
            case 2:
                abrirArchivo();
                break;
            
            case 3:
                borrarArchivo();
                break;
            
            case 4:
                moverArchivo();
                break;
            
            case 5:
                renombrarArchivo();
                break;
            
            case 0:
                printf("Cerrando el gestor....\n");
                break;
            default:
                printf("Opcion inexistente.\n");
        }
    } 
    
    while (opcion != 0);
    return 0;
}