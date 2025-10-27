//para compilar gcc -Wall main.c funciones.c validaciones.c -o main.exe ; ./main.exe
#include "headers.h"
#include "indice.h"

t_Fecha fechaProcesoGlobal;

int main()
{
    char op;
    bool procesarTxt = true; 
    t_indice miIndice;
    t_reg_indice vecIndice;

    puts("====== SISTEMA VIDECLUB CINEFILIA ======");

    ingresarFecha();

    char nombreArchBin[50];
    sprintf(nombreArchBin, "miembros-VC-%04d%02d%02d.dat", fechaProcesoGlobal.anio, fechaProcesoGlobal.mes, fechaProcesoGlobal.dia);

    FILE* archBin = fopen(nombreArchBin, "rb");

    // 3. Lógica principal
    if (archBin != NULL) 
    {
        fclose(archBin);
        puts("\nYa existe un archivo generado para esta fecha.");
        puts("¿Desea recuperarlo y trabajar con el? (S/N)");
        
        if (ingresoYvalidaOpcion('S', 'N') == 'S')
        {
            puts("Archivo recuperado. No se procesara el archivo de texto.");
            procesarTxt = false; 
        }
        else
        {
            puts("Se procedera a sobreescribir el archivo existente.");
            procesarTxt = true;
        }
    }
    fclose(archBin);

    if (procesarTxt)
    {
        if (!leerYGenerarArchivo())
        {
            printf("FALLO LA CREACION DE ARCHIVOS. El programa se cerrara.\n");
            return 1; 
        }
    }
    

    indice_cargar(nombreArchBin, &miIndice, &vecIndice,sizeof(s_miembros), cmp);

    puts("\n\n====== DESPLEGANDO MENU ======");
    mostrarMenu();
    
    puts("\nIngrese opcion: ");
    op = ingresoYvalidaOpcion('A', 'G');
    switch (op)
    {
        case 'a':
            altaUsuario(nombreArchBin,&miIndice,&vecIndice, sizeof(t_indice),cmp);
            break;
        case 'b':
            bajaUsuario(nombreArchBin, &miIndice, &vecIndice, sizeof(t_indice), cmp);
            break;
        case 'c':
            //modificarUsuario();
            break;
        case 'd':
            //mostrarInfoMiembro();
            break;
        case 'e':
            //listarMiembros();
            break;
        case 'f':
            //listarMiembrosxPlan();
            break;
        case 'g':
            break;
    }

    return 0;
}