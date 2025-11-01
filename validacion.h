#ifndef VALIDACION_H_INCLUDED
#define VALIDACION_H_INCLUDED

#include "fecha.h"
#include "miembros.h"
#include "menu.h"
#include "indice.h"
#include <ctype.h>
#include <string.h>

void limpiar_buffer();
void normalizar(char *cad);
int validar_dni_unico(t_indice *indice, long dni, int (*cmp)(const void*, const void*));
int validar_nombre(const char *apeynom);
int validar_fecha_nacimiento(t_fecha *fecha_nac, t_fecha *fecha_actual);
int validar_fecha_afiliacion(t_fecha *fecha_afi, t_fecha *fecha_nac, t_fecha *fecha_actual);
int validar_sexo(char sexo);
int validar_categoria(const char *categoria, t_fecha *fecha_nac, t_fecha *fecha_actual);
int validar_fecha_ultima_cuota(t_fecha *fecha_ult_cuo, t_fecha *fecha_afi, t_fecha *fecha_actual);
int validar_plan(const char *plan);
int validar_email_tutor(const char *emailTutor, int es_menor);
int email_valido(const char *email);
char* validar_miembro(const t_miembro *miembro, const t_fecha *fecha_proceso);


#endif // VALIDACION_H_INCLUDED
