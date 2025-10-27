#ifndef VALIDACION_H_INCLUDED
#define VALIDACION_H_INCLUDED

#include "fecha.h"
#include "miembros.h"
#include "menu.h"
#include <ctype.h>
#include <string.h>

void limpiar_buffer();
void normalizar(char *cad);
char* validar_miembro(const t_miembro *miembro, const t_fecha *fecha_proceso);
int email_valido(const char *email);

#endif // VALIDACION_H_INCLUDED
