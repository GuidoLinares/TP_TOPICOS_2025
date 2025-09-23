#ifndef INDICE_H_INCLUDED
#define INDICE_H_INCLUDED

#include "headers.h"

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


/**************************************************************************
Descripción: toma memoria para 100 elementos e inicializa la estructura vacía.
Parámetros: indice: TDA índice.
nmemb: cantidad de elementos del índice.
tamanyo: el espacio en bytes ocupado por cada elemento.
Retorno: n/a.
Observaciones:
**************************************************************************/

void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo);


/**************************************************************************
Descripción: redimensiona el tamaño del índice.
Parámetros: indice: TDA índice.
nmemb: cantidad de elementos del índice.
tamanyo: el espacio en bytes ocupado por cada elemento.
Retorno: n/a.
Observaciones: Debe proporcionar el nmemb incrementado en un 30%
**************************************************************************/
void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo);


/**************************************************************************
Descripción: inserta en orden según la clave.
Parámetros: indice: TDA índice.
registro: el nuevo elemento a insertar en el índice.
tamanyo: el espacio en bytes ocupado por el elemento a insertar.
cmp: función de comparación provista.
Retorno: OK si la operación fue exitosa y ERROR en caso contrario.
Observaciones: Si el array está lleno, toma un 30 % más de memoria.
**************************************************************************/
int indice_insertar (t_indice *indice, const void *registro, size_t tamanyo,
int (*cmp)(const void *, const void *));


/**************************************************************************
Descripción: elimina el registro del índice.
Parámetros: indice: TDA índice.
registro: el elemento a eliminar.
tamanyo: el espacio en bytes ocupado por el elemento a insertar.
cmp: función de comparación provista.
Retorno: OK si la operación fue exitosa y ERROR en caso contrario.
Observaciones: -
**************************************************************************/
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int
(*cmp)(const void *, const void *));


/**************************************************************************
Descripción: si la clave existe deja el registro en registro.
Parámetros: indice: TDA índice.
registro: el elemento a buscar.
nmemb: cantidad de elementos del índice.
tamanyo: espacio en bytes ocupado por el elemento a insertar.
cmp: función de comparación provista.
Retorno: NO_EXISTE si no existe o si existe, la posición ocupada dentro
del array.
Observaciones: -
**************************************************************************/
int indice_buscar (const t_indice *indice, const void *registro, size_t nmemb,
size_t tamanyo, int (*cmp)(const void *, const void *));


/**************************************************************************
Descripción: determina si el índice contiene 0 (cero) elementos.
Parámetros: indice: TDA índice.
Retorno: OK si está vacío, cualquier otro valor si no lo está.
Observaciones: -
**************************************************************************/
int indice_vacio(const t_indice *indice);

/**************************************************************************
descripción: determina si el índice contiene el tamaño máximo posible.
Parámetros: indice: TDA índice.
Retorno: OK si está lleno, cualquier otro valor si no lo está.
Observaciones: -
**************************************************************************/
int indice_lleno(const t_indice *indice);

/**************************************************************************
Descripción: deja el índice vacío.
Parámetros: indice: TDA indice.
Retorno: No posee.
Observaciones: -
**************************************************************************/
void indice_vaciar(t_indice* indice);

/**************************************************************************
Descripción: Carga el array desde un archivo ordenado.
Parámetros: path: la ruta al archivo binario.
indice: TDA índice.
vreg_ind: vector de elementos dentro del índice.
tamanyo: el espacio en bytes ocupado por el elemento a insertar.
cmp: función de comparación provista.
Retorno: OK si la operación fue exitosa y ERROR en caso contrario.
Observaciones: -
**************************************************************************/
int indice_cargar(const char* path, t_indice* indice, void *vreg_ind, size_t
tamanyo, int (*cmp)(const void *, const void *));



#endif // INDICE_H_INCLUDED