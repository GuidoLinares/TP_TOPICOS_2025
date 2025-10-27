#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "validacion.h"
#include <ctype.h>
#include "indice.h"

void mostrar_menu (const char *menu[], int tam);
char capturar_opcion(char li, char ls);
void alta_miembro(const char *nombre_archivo_bin, t_indice *indice, t_fecha *fecha, int (*cmp)(const void*, const void*));
void baja_miembro(const char *nombre_archivo_bin, t_indice *indice, int (*cmp)(const void*, const void*));
void mostrar_miembro(const t_indice *indice, const char *nombre_archivo_bin, int (*cmp)(const void*, const void*));
void listar_miembros_dni(const t_indice *indice, const char *nombre_archivo_bin);
void listar_miembros_plan(const t_indice *indice, const char *nombre_archivo_bin);
int cmp_plan_dni(const void *a, const void *b);
void modificar_miembro(const char *arch_bin, t_indice *pIndice, t_fecha *fecha_proceso, int (*cmp)(const void*, const void*));


#endif // MENU_H_INCLUDED
