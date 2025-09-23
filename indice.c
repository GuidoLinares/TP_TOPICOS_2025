#include "indice.h"


void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo)
{
  indice->vindice = malloc(nmemb * tamanyo);

  if (indice->vindice == NULL)
  {
    indice->cantidad_elementos_actual = 0;
    indice->cantidad_elementos_maxima = 0;
  }
  else
  {
    indice->cantidad_elementos_maxima = nmemb;
    indice->cantidad_elementos_actual = 0;
  }

}

void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo)
{
    unsigned nuevaCantidadMax = (unsigned)(indice->cantidad_elementos_maxima * INCREMENTO);

    void* aux = realloc(indice->vindice, nuevaCantidadMax * tamanyo);
    if (aux == NULL)
        puts("fallo al redimensionar el indice");

    indice->vindice = aux;
    indice->cantidad_elementos_maxima = nuevaCantidadMax;
}

int indice_insertar (t_indice *indice, const void *registro, size_t tamanyo,int (*cmp)(const void *, const void *))
{

}

int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int(*cmp)(const void *, const void *))
{

}

int indice_buscar (const t_indice *indice, const void *registro, size_t nmemb,size_t tamanyo, int (*cmp)(const void *, const void *))
{

    

}

int indice_vacio(const t_indice *indice)
{

}

int indice_lleno(const t_indice *indice)
{

}

void indice_vaciar(t_indice* indice)
{

}

int indice_cargar(const char* path, t_indice* indice, void *vreg_ind, size_t tamanyo, int (*cmp)(const void *, const void *))
{

}


