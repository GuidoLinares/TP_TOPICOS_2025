#include "indice.h"


void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo)
{
    
}

void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo)
{

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


