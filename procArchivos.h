#ifndef PROC_ARCHIVOS_H
#define PROC_ARCHIVOS_H

void quitarSalto(char texto[]);
void iniciarPapelera(void);


//Funciones para consolas
void crearArchivo(void);
void borrarArchivo(void);
void renombrarArchivo(void);
void moverArchivo(void);
void abrirArchivo(void);

// funciones para interfaz
int crearArchivosEnRuta(const char *ruta);
int borrarArchivosEnRuta(const char *ruta);
int renombrarArchivos(const char *rutaActual, const char *nuevaRuta);
int moverArchivosEnRuta(const char *origen, const char *destino);
char *abrirArchivoEnRuta(const char *ruta);
int crearCarpetaEnRuta(const char *ruta);
#endif