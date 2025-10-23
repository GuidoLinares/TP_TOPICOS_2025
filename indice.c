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
        //compruebo si hay espacio
        
        //si no hay, llamo a indice_redimensionar
        //busco pos
        //muevo todo desde pos hasta el final con memmove
        //copio registro con memcpy en pos
        //incremento contador

    }
    

    int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int(*cmp)(const void *, const void *))
    {
    int pos = indice_buscar(indice, registro, indice->cantidad_elementos_actual, tamanyo, cmp);
    
    if(pos == NO_EXISTE )
        return ERROR;
    
    void* destino = (char*) indice->vindice + pos * tamanyo;
    const void* origen = (const char*) destino + tamanyo;
    
    unsigned elementos_a_mover = indice->cantidad_elementos_actual - 1 - pos;

    if (elementos_a_mover > 0 )
        memmove(destino, origen, elementos_a_mover * tamanyo);
    
    
    indice->cantidad_elementos_actual --;

    return OK;
    }

    int indice_buscar (const t_indice *indice, const void *registro, size_t nmemb,size_t tamanyo, int (*cmp)(const void *, const void *))
    {
        int inicio = 0;
        int fin = indice->cantidad_elementos_actual - 1;
        int medio;

        while (inicio <= fin)
        { 
            medio = (inicio + fin) / 2;

            void* elemento_medio = (char*)indice->vindice + (medio*tamanyo);

            int resultado = cmp(registro,elemento_medio);

            if (resultado == 0)
            return medio;
            else if (resultado < 0)
            fin = medio - 1;
            else
            inicio  = medio + 1; 
        }
        
        return NO_EXISTE;
    }

    int indice_vacio(const t_indice *indice)
    {
    if ((unsigned)indice->cantidad_elementos_actual == 0)
        return OK;
    else
        return 2;
    }

    int indice_lleno(const t_indice *indice)
    {
    if ((unsigned)indice->cantidad_elementos_actual >= (unsigned)indice->cantidad_elementos_maxima)
        return OK;
    else
        return 2;
    }

    void indice_vaciar(t_indice* indice)
    {
    while ((unsigned)indice->cantidad_elementos_actual != 0)
    {
        free(indice->vindice);
        (unsigned)indice->cantidad_elementos_actual -- ;
    }
    }

    /*
    int indice_cargar(const char* path, t_indice* indice, void *vreg_ind, size_t tamanyo, int (*cmp)(const void *, const void *))
    {

    }
    */
