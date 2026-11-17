#ifndef AUDITORIA_H
#define AUDITORIA_H

void registrarAuditoria(const char *operacion,
                        const char *detalle,
                        const char *resultado);

/* Devuelve el contenido del log; se libera con g_free. NULL si no hay log. */
char *leerAuditoria(void);

#endif