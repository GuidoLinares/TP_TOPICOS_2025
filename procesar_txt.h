#ifndef PROCESAR_TXT_H_INCLUDED
#define PROCESAR_TXT_H_INCLUDED

#include "fecha.h"
#include "miembros.h"
#include <string.h>
#include <stdlib.h>
#include "validacion.h"
#include <stdio.h>

#define ARCH_MIEMBROS_TXT "miembros-VC.txt"
#define TAM_NOM_ARCH 19
#define PREFIJO_BIN "miembros-VC"
#define PREFIJO_ERROR "error-VC"
#define TIPO_ARCH_BIN ".dat"
#define TIPO_ARCH_TXT ".txt"
#define TAM_LINEA 155

void procesar_archivo_texto(t_fecha fecha_proceso, const char *nombre_archivo_bin);
void generar_nombre_archivo(char *nombre_arch, const char *prefijo, t_fecha fecha, const char *tipo_archivo);
int linea_a_miembro (char* linea, t_miembro* miembro);
int recuperar_archivo(t_fecha fecha_proceso, char *nombre_arch_encontrado);

#endif // PROCESAR_TXT_H_INCLUDED
