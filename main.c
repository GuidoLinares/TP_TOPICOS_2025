//para compilar gcc -Wall main.c funciones.c validaciones.c -o main.exe ; ./main.exe
#include "headers.h"

int main()
{
    char op;

    puts("====== SISTEMA VIDECLUB CINEFILIA ======");    

    ingresarFecha();
    
    if(!leerYGenerarArchivo())
        printf("FALLO LA CREACION DE ARCHIVOS");
    else
    {
        puts("\n\n====== DESPLEGANDO MENU ======");
    
        mostrarMenu();
        
        puts("Ingrese opcion: ");
        op = ingresoYvalidaOpcion('A','G');

        switch (op)
        {
        case 'a':
            //altaUsuario();
            break;
        case 'b':
            //bajaUsuario();
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

    }

    
    return 0;
}












