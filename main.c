#include <stdio.h>
#include <stdlib.h>

#include "fecha.h"
#include "miembros.h"
#include "validacion.h"
#include "menu.h"
#include "procesar_txt.h"
#include "indice.h"

int main()
{
    /// DECLARACIONES
    t_fecha fecha_proceso;
    t_indice indice;

    puts("=== VIDEOCLUB CINEFILIA ===");
    puts("Sistema de Gestion de Miembros\n");

    /// INGRESO FECHA
    ingresarFechaValida(&fecha_proceso);

    /// MENU ARCHIVOS
    const char *menu_archivos[] =
    {
        "\t1. Recuperar archivo binario existente",
        "\t2. Procesar archivo de texto\n"
    };
    int tam_menu_archivos = sizeof(menu_archivos) / sizeof(menu_archivos[0]);

    mostrar_menu(menu_archivos, tam_menu_archivos);
    char opcion_menu_archivos = capturar_opcion('1', '2');

    char arch_bin[TAM_NOM_ARCH];
    int exito_carga = 0;

    switch (opcion_menu_archivos)
    {
    case '1':
        puts("\n>>> Recuperando archivo binario existente...\n");
        if (recuperar_archivo(fecha_proceso, arch_bin))
        {
            printf("Archivo encontrado: %s\n", arch_bin);
            exito_carga = 1;
        }
        else
        {
            puts("No se encontraron archivos anteriores. Abortando.\n");
            return 1;
        }
        break;

    case '2':
        puts("\n>>> Procesando archivo de texto...\n");
        generar_nombre_archivo(arch_bin, PREFIJO_BIN, fecha_proceso, TIPO_ARCH_BIN);
        procesar_archivo_texto(fecha_proceso, arch_bin);
        printf("Archivo generado: %s\n", arch_bin);
        exito_carga = 1;
        break;
    }

    if (!exito_carga)
    {
        puts("Error: No se pudo cargar o crear un archivo de miembros.\n");
        return 1;
    }

    printf("\nTrabajando con el archivo: %s\n", arch_bin);
    puts("Creando indice en memoria...\n");

    indice_crear(&indice, 0, sizeof(t_reg_indice));

    if (indice_construir_desde_dat(&indice, arch_bin, sizeof(t_reg_indice), cmp_indice_dni) == ERROR)
    {
        puts("Error: No se pudo construir el indice en memoria.\n");
        indice_vaciar(&indice);
        return 1;
    }

    printf("Indice cargado con %u elementos.\n\n", indice.cantidad_elementos_actual);

    /// MENU PROCESAMIENTO
    const char *menu_binario[] =
    {
        "\tA) Alta",
        "\tB) Baja",
        "\tC) Modificacion",
        "\tD) Mostrar informacion de un miembro",
        "\tE) Listado de miembros ordenados por DNI",
        "\tF) Listado de todos los miembros agrupados por plan",
        "\tG) Listado de todos los miembros inactivos (dados de baja)",
        "\tH) Buscar miembros por rangos de edad",
        "\tI) Mostrar Estadísticas Simples",
        "\tJ) Salir"
    };

    int tam_menu_binario = sizeof(menu_binario) / sizeof(menu_binario[0]);
    char opcion_menu_binario;

    do
    {
        system("cls");  /// Limpia pantalla cada vez que vuelve al men� principal
        mostrar_menu(menu_binario, tam_menu_binario);
        opcion_menu_binario = capturar_opcion('A', 'J');

        switch (opcion_menu_binario)
        {
        case 'A':
            system("cls");
            alta_miembro(arch_bin, &indice, &fecha_proceso, cmp_indice_dni);
            break;

        case 'B':
            system("cls");
            baja_miembro(arch_bin, &indice, cmp_indice_dni);
            break;

        case 'C':
            system("cls");
            modificar_miembro(arch_bin, &indice, &fecha_proceso, cmp_indice_dni);
            break;

        case 'D':
            system("cls");
            mostrar_miembro(&indice, arch_bin, cmp_indice_dni);
            break;

        case 'E':
            system("cls");
            listar_miembros_dni(&indice, arch_bin);
            break;

        case 'F':
            system("cls");
            listar_miembros_plan(&indice, arch_bin);
            break;

        case 'G':
            system("cls");
            //mostrar_miembros_inactivos(arch_bin);
            break;

        case 'H':
            system("cls");
            //mostrar_miembros_rango_edad(&indice, arch_bin);
            break;

        case 'I':
            system("cls");
            mostrar_estadisticas(arch_bin);
            break;


        case 'J':
            system("cls");
            puts("\nSaliendo del programa...\n");
            break;


        default:
            puts("\nOpcion invalida. Intente nuevamente.\n");
        }

        if (opcion_menu_binario != 'J')
        {
            puts("\nPresione DOBLE ENTER para volver al menu principal...");
            limpiar_buffer();
            getchar();
        }

    } while (opcion_menu_binario != 'J');

    puts("Liberando memoria del indice...\n");
    free(indice.vindice);

    return 0;
}

