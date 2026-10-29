#ifndef AUDITORIA_H
#define AUDITORIA_H

const char *obtenerUsuario(void);

void registrarAuditoria(    //que guarda la auditoria??
    const char *operacion,     // la operación
    const char *detalle,       // el detale(hora,usuario)
    const char *resultado      // si fallo o fue un exito 
);

#endif