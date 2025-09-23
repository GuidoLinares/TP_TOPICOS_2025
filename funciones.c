#include "headers.h"

void mostrarMenu()
{
    char*menu[] = {
        "a. Alta",
        "b. Baja",
        "c. Modificacion",
        "d. Mostrar informacion de un miembro",
        "e. Listado de miembros ordenados por DNI",
        "f. Listado de todos los miembros agrupados por plan",
        "g. Salir"
    };

    int tama = sizeof(menu)/sizeof(menu[0]);
    for (int i = 0; i < tama; i++)
    {
        printf("\n%s", menu[i]);
    }
}

void normalizarCadena(char* cadena )
{
    char* lector = cadena;
    char* escritor = cadena;
    bool primera_letra = true;
    bool ya_puse_coma = false;

    while (isspace(*lector))
        lector++;

    while (*lector != '\0')
    {
        if (isalpha(*lector))
        {
            if (primera_letra)
            {
                *escritor = toupper(*lector);
                primera_letra = false;
            }
            else
            {
                *escritor = tolower(*lector);
            }
            lector++;
            escritor++;
        }
        else if (isspace(*lector))
        {
            while (isspace(*lector))
                lector++;

            if (*lector != '\0') 
            {
                if (!ya_puse_coma) 
                {
                    *escritor = ',';
                    ya_puse_coma = true;
                }
                else           
                    *escritor = ' ';
                
                escritor++;
                primera_letra = true;
            }
        }
        else
        {
            lector++;
        }
    }

    if (escritor > cadena && (*(escritor - 1) == ',' || *(escritor - 1) == ' '))
        escritor--;

    *escritor = '\0';
}

void ingresarFecha()
{   
    t_Fecha fecha;

    puts("INGRESE FECHA (dd/mm/aaaa): ");
    do
    {
        scanf("%d",&fecha.dia);
        scanf("%d",&fecha.mes);
        scanf("%d",&fecha.anio);
        if (!validarFecha(fecha))
            puts("Fecha invalida, ingrese nuevamente: ");
        
    } while (!validarFecha(fecha));
    
    printf("\n FECHA INGRESADA: %d/%d/%d",fecha.dia,fecha.mes,fecha.anio);
}

bool leerYGenerarArchivo()
{

    FILE* bin, *txt;
    
    txt = fopen(arch_miembros_txt, "rt");

    if (!txt)
        printf("No se pudo abrir el archivo %s",arch_miembros_txt);
    
    bin = fopen (arch_miembros,"wb");




}


