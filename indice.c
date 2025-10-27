#include "indice.h"

void indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo)
{
    indice->vindice = malloc(CANTIDAD_ELEMENTOS * tamanyo);
    if (indice->vindice == NULL)
    {
        // Error: No se pudo conseguir memoria.
        indice->cantidad_elementos_actual = 0;
        indice->cantidad_elementos_maxima = 0;
    }
    else
    {
        // Inicializamos la estructura vacía.
        indice->cantidad_elementos_actual = 0;
        indice->cantidad_elementos_maxima = CANTIDAD_ELEMENTOS;
    }
}

void indice_redimensionar(t_indice *indice, size_t nmemb, size_t tamanyo)
{
    size_t nuevo_tam_bytes = nmemb * tamanyo;
    void *nuevo_puntero = realloc(indice->vindice, nuevo_tam_bytes);
    if (nuevo_puntero != NULL)
    {
        // Éxito: Actualizamos el puntero y el tamaño máximo.
        indice->vindice = nuevo_puntero;
        indice->cantidad_elementos_maxima = nmemb;
    }
    // Si falló (nuevo_puntero == NULL)
    // La función que llamó (indice_insertar) fallará al intentar insertar.
}

int indice_vacio(const t_indice *indice)
{
    if (indice->cantidad_elementos_actual == 0)
        return OK; // OK está definido como 1 en indice.h
    else
        return ERROR; // ERROR está definido como 0 en indice.h
}

int indice_lleno(const t_indice *indice)
{
    if (indice->cantidad_elementos_actual == indice->cantidad_elementos_maxima)
        return OK; // OK está definido como 1 en indice.h
    else
        return ERROR; // ERROR está definido como 0 en indice.h
}

void indice_vaciar(t_indice* indice)
{
    indice->cantidad_elementos_actual = 0;
}

int indice_buscar(const t_indice *indice, const void *registro, size_t nmemb, size_t tamanyo, int (*cmp) (const void *, const void *))
{
    int izq = 0;
    int der = indice->cantidad_elementos_actual - 1;
    char *p_base = (char*)indice->vindice;
    while (izq <= der)
    {
        // 1. Calculamos la posición media
        int medio = izq + (der - izq) / 2;
        // 2. Obtenemos un puntero al elemento del medio
        void *elem_medio = p_base + (medio * tamanyo);
        // 3. Comparamos el elemento del medio con el que buscamos
        int res_cmp = cmp(elem_medio, registro);
        if (res_cmp == 0) // ENCONTRADO
            return medio;
        else if (res_cmp > 0) // elem_medio es MAYOR que registro, buscamos en la mitad izquierda
            der = medio - 1;
        else // elem_medio es MENOR que registro, buscamos en la mitad derecha
            izq = medio + 1;
    }
    return NO_EXISTE; // Definido como -1 en indice.h
}

int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp)(const void *, const void *))
{
    char *p_base = (char*)indice->vindice;
    int pos = indice_buscar(indice, registro, 0, tamanyo, cmp);
    if (pos == NO_EXISTE)
        return ERROR;
    int elementos_a_mover = indice->cantidad_elementos_actual - pos - 1;
    if (elementos_a_mover > 0)
    {
        // Calculamos la dirección de "destino" (la posición 'pos' que borramos)
        void *destino = p_base + (pos * tamanyo);
        // Calculamos la dirección de "origen" (la posición 'pos + 1')
        void *origen = p_base + ((pos + 1) * tamanyo);
        // Calculamos cuántos bytes totales hay que mover
        size_t bytes_a_mover = elementos_a_mover * tamanyo;

        memmove(destino, origen, bytes_a_mover);
    }
    indice->cantidad_elementos_actual--;
    return OK;
}

int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo, int (*cmp) (const void *, const void *))
{
    // Verificar si el array está lleno y redimensionar si es necesario ---
    if (indice_lleno(indice))
    {
        unsigned nueva_maxima = (unsigned)(indice->cantidad_elementos_maxima * INCREMENTO);
        // Si el índice estaba vacío (0 * 1.3 = 0), le damos el tamaño base
        if (nueva_maxima == 0)
            nueva_maxima = CANTIDAD_ELEMENTOS;

        indice_redimensionar(indice, nueva_maxima, tamanyo);
        // Si después de intentar redimensionar sigue lleno, realloc() falló.
        if (indice_lleno(indice))
            return ERROR; // No hay más memoria, no se puede insertar.
    }

    // Buscar la posición de inserción
    char *p_base = (char*)indice->vindice;
    int pos = 0; // Posición de inserción
    // Buscamos la posición 'pos' donde el elemento en esa posición sea MAYOR que el registro que queremos insertar.
    while (pos < indice->cantidad_elementos_actual)
    {
        void *elem_actual = p_base + (pos * tamanyo);
        // elem_actual > registro
        if (cmp(elem_actual, registro) > 0)
            break; // Encontramos la posición. Hay que insertar aquí.
        pos++;
    }
    // Si el bucle termina sin 'break', 'pos' será 'cantidad_elementos_actual', lo que significa que el nuevo registro va al final (es el más grande).
    // Mover elementos para "hacer el hueco"
    int elementos_a_mover = indice->cantidad_elementos_actual - pos;
    if (elementos_a_mover > 0)
    {
        void *origen = p_base + (pos * tamanyo);
        void *destino = p_base + ((pos + 1) * tamanyo);
        size_t bytes_a_mover = elementos_a_mover * tamanyo;
        memmove(destino, origen, bytes_a_mover);
    }
    void *pos_insercion = p_base + (pos * tamanyo);
    memcpy(pos_insercion, registro, tamanyo);
    indice->cantidad_elementos_actual++;
    return OK;
}

int indice_construir_desde_dat(t_indice *indice, const char *path_archivo_dat, size_t tamanyo_reg_indice, int (*cmp)(const void*, const void*))
{
    FILE *arch_dat = fopen(path_archivo_dat, "rb");
    if (arch_dat == NULL)
    {
        printf("Error: No se pudo abrir el archivo .dat para construir el indice.\n");
        return ERROR;
    }
    t_miembro miembro_leido;
    t_reg_indice nuevo_reg_indice;
    unsigned nro_reg = 0;
    // Leemos el archivo .dat de principio a fin
    while (fread(&miembro_leido, sizeof(t_miembro), 1, arch_dat))
    {
        // Solo indexamos registros activos (Estado 'A')
        if (miembro_leido.estado == 'A')
        {
            // Creamos el registro de índice
            nuevo_reg_indice.dni = miembro_leido.dni;
            nuevo_reg_indice.nro_reg = nro_reg;
            // Lo insertamos en orden en el TDA
            if (indice_insertar(indice, &nuevo_reg_indice, tamanyo_reg_indice, cmp) == ERROR)
            {
                printf("Error: Fallo al insertar en el indice (posiblemente sin memoria).\n");
                fclose(arch_dat);
                return ERROR;
            }
        }
        nro_reg++;
    }
    fclose(arch_dat);
    return OK;
}

int indice_cargar(const char* path, t_indice* indice, void *vreg_ind, size_t tamanyo, int (*cmp) (const void *, const void *))
{
    FILE *arch_idx = fopen(path, "rb");
    if (arch_idx == NULL)
        return ERROR;
    // Calculamos cuántos registros hay en el archivo
    fseek(arch_idx, 0, SEEK_END); // Vamos al final del archivo
    long tam_archivo_bytes = ftell(arch_idx); // Obtenemos el tamaño en bytes
    rewind(arch_idx); // Volvemos al principio

    unsigned cant_registros = (unsigned)(tam_archivo_bytes / tamanyo);
    if (cant_registros == 0)
    {
        // El archivo está vacío, no hay nada que cargar.
        fclose(arch_idx);
        return OK;
    }
    // Verificamos si tenemos suficiente memoria en el TDA
    if (indice->cantidad_elementos_maxima < cant_registros)
    {
        // Usamos realloc() para ajustar la memoria exactamente al tamaño necesario
        void *nuevo_puntero = realloc(indice->vindice, tam_archivo_bytes);
        if (nuevo_puntero == NULL)
        {
            fclose(arch_idx);
            return ERROR;
        }
        indice->vindice = nuevo_puntero;
        indice->cantidad_elementos_maxima = cant_registros;
    }

    // Leemos TODOS los registros del archivo
    size_t leidos = fread(indice->vindice, tamanyo, cant_registros, arch_idx);
    fclose(arch_idx);
    // 5. Verificamos si la lectura fue exitosa
    if (leidos != cant_registros)
    {
        indice->cantidad_elementos_actual = 0;
        return ERROR;
    }

    // Actualizamos el contador del TDA
    indice->cantidad_elementos_actual = cant_registros;
    return OK;
}

int cmp_indice_dni(const void *a, const void *b)
{
    // Convertimos los punteros void* a los punteros de nuestro tipo
    t_reg_indice *reg_a = (t_reg_indice*)a;
    t_reg_indice *reg_b = (t_reg_indice*)b;

    if (reg_a->dni < reg_b->dni)
        return -1;
    if (reg_a->dni > reg_b->dni)
        return 1;
    return 0;
}
