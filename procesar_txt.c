#include "procesar_txt.h"

void generar_nombre_archivo(char *nombre_arch, const char *prefijo, t_fecha fecha, const char *tipo_archivo)
{
    sprintf(nombre_arch, "%s-%04d%02d%02d%s", prefijo, fecha.anio, fecha.mes, fecha.dia, tipo_archivo);
}

void procesar_archivo_texto(t_fecha fecha_proceso, const char *nombre_archivo_bin)
{
    ///DECLARACION Y APERTURA ARCHIVOS
    FILE *arch_txt, *arch_bin, *arch_error;

    arch_txt = fopen(ARCH_MIEMBROS_TXT, "rt");
    if(!arch_txt)
    {
        puts("Error: No se puedo abrir el archivo miembros-VC.txt");
        return;
    }

    arch_bin = fopen (nombre_archivo_bin, "wb");
    if(!arch_bin)
    {
        puts("Error: No se puedo crear el archivo miembros-VC.bin");
        fclose(arch_txt);
        return;
    }

    char nombre_error[TAM_NOM_ARCH];
    generar_nombre_archivo(nombre_error, PREFIJO_ERROR, fecha_proceso, TIPO_ARCH_TXT);
    arch_error = fopen (nombre_error, "wt");
    if(!arch_error)
    {
        puts("Error: No se puedo crear el archivo error-VC.txt");
        fclose(arch_txt);
        fclose(arch_bin);
        return;
    }

    char linea[TAM_LINEA], copia_linea[TAM_LINEA];
    const char *motivo_error;
    t_miembro miembro;

    while(fgets(linea,TAM_LINEA,arch_txt))
    {
        linea[strcspn(linea, "\n")] = 0;
        strcpy(copia_linea,linea);
        if(linea_a_miembro(linea, &miembro))
        {
            normalizar(miembro.apeynom);
            motivo_error = validar_miembro(&miembro, &fecha_proceso);
            if(motivo_error == NULL)
            {
                fwrite(&miembro, sizeof(t_miembro), 1, arch_bin);
            }else
            {
                fprintf(arch_error, "%s|%s\n", motivo_error, copia_linea);
            }
        }
        else
        {
            fprintf(arch_error, "%s|%s\n", copia_linea, "Error: Formato de linea invalido, faltan campos");
        }
    }
    fclose(arch_txt);
    fclose(arch_bin);
    fclose(arch_error);
    return;
}

int linea_a_miembro (char* linea, t_miembro* miembro)
{
    char *token, *delimitador = "|";

    ///DNI
    token = strtok(linea, delimitador);
    if(token == NULL)
        return 0;
    miembro->dni = atol(token);

    ///APELLIDO Y NOMBRE
    token = strtok(NULL, delimitador);
    if(token == NULL)
        return 0;
    strcpy(miembro->apeynom, token);

    ///FECHA NACIMIENTO
    token = strtok(NULL, delimitador);
    if (token == NULL)
        return 0;
    sscanf(token, "%d/%d/%d", &miembro->fecha_nac.dia, &miembro->fecha_nac.mes, &miembro->fecha_nac.anio);

    ///SEXO
    token = strtok(NULL, delimitador);
    if (token == NULL)
        return 0;
    miembro->sexo = token[0];

    ///FECHA AFILIACION
    token = strtok(NULL, delimitador);
    if (token == NULL)
        return 0;
    sscanf(token, "%d/%d/%d", &miembro->fecha_afi.dia, &miembro->fecha_afi.mes, &miembro->fecha_afi.anio);

    ///CATEGORIA
    token = strtok(NULL, delimitador);
    if(token == NULL)
        return 0;
    strcpy(miembro->categoria, token);

    ///FECHA ULTIMA CUOTA
    token = strtok(NULL, delimitador);
    if (token == NULL)
        return 0;
    sscanf(token, "%d/%d/%d", &miembro->fecha_ult_cuo.dia, &miembro->fecha_ult_cuo.mes, &miembro->fecha_ult_cuo.anio);

    ///ESTADO
    token = strtok(NULL, delimitador);
    if (token == NULL)
        return 0;
    miembro->estado = token[0];

    ///PLAN
    token = strtok(NULL, delimitador);
    if(token == NULL)
        return 0;
    strcpy(miembro->plan, token);

    ///MAIL TUTOR
    token = strtok(NULL, delimitador);
    if(token != NULL) // Si el token NO es nulo
        strcpy(miembro->emailTutor, token); // Lo copiamos
    else
        strcpy(miembro->emailTutor, ""); // Si es nulo, guardamos un string vacío

    return 1;
}

int recuperar_archivo(t_fecha fecha_proceso, char *nombre_arch_encontrado)
{
    FILE *arch;
    t_fecha fecha_busqueda = fecha_proceso;
    char nombre_temporal[TAM_NOM_ARCH];

    // Buscamos como máximo en los últimos 365 días
    for (int i = 0; i < 365; i++)
    {
        restar_un_dia(&fecha_busqueda);
        generar_nombre_archivo(nombre_temporal, PREFIJO_BIN, fecha_busqueda, TIPO_ARCH_BIN);
        arch = fopen(nombre_temporal, "rb");
        if (arch != NULL)
        {
            // Lo encontramos
            fclose(arch);
            strcpy(nombre_arch_encontrado, nombre_temporal); // Copiamos el nombre para devolverlo
            return 1;
        }
    }
    return 0;
}
