#ifndef INDICE_H_INCLUDED
#define INDICE_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "miembros.h"

#define CANTIDAD_ELEMENTOS 100
#define INCREMENTO 1.3
#define OK 1
#define ERROR 0
#define NO_EXISTE -1

typedef struct
{
   unsigned nro_reg;
   long dni;
}t_reg_indice;

typedef struct
{
void *vindice;
unsigned cantidad_elementos_actual;
unsigned cantidad_elementos_maxima;
}t_indice;


typedef struct 
{
   unsigned nro_reg;
   char nombreApe [50];
}t_reg_indice_apeynom;



void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo);
void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo);
int indice_insertar (t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void *, const void *));
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void *, const void *));
int indice_buscar (const t_indice *indice, const void *registro, size_t nmemb, size_t tamanyo, int (*cmp)(const void *, const void *));
int indice_vacio(const t_indice *indice);
int indice_lleno(const t_indice *indice);
void indice_vaciar(t_indice* indice);
int indice_cargar(const char* path, t_indice* indice, void *vreg_ind, size_t tamanyo, int (*cmp)(const void *, const void *));
int indice_construir_desde_dat(t_indice *indice, const char *path_archivo_dat, size_t tamanyo_reg_indice, int (*cmp)(const void*, const void*));
int cmp_indice_dni(const void *a, const void *b);

//NUEVAS MODIFICACIONES
void* cmp_indice_nomape(const void*, const void*);
int indice_construir_apeynom_desde_dat(t_indice *indice, const char *path_archivo_dat);



#endif // INDICE_H_INCLUDED
