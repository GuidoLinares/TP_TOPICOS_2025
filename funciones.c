#include "headers.h"
#include "indice.h"

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

    puts("INGRESE FECHA (dd/mm/aaaa): ");
    do
    {
        scanf("%d",&fechaProcesoGlobal.dia);
        scanf("%d",&fechaProcesoGlobal.mes);
        scanf("%d",&fechaProcesoGlobal.anio);
        if (!validarFecha(fechaProcesoGlobal))
            puts("Fecha invalida, ingrese nuevamente: ");
        
    } while (!validarFecha(fechaProcesoGlobal));
    
    printf("\n FECHA INGRESADA: %d/%d/%d",fechaProcesoGlobal.dia,fechaProcesoGlobal.mes,fechaProcesoGlobal.anio);
}

bool grabarArchError(const char* lineaError,char* motivoError, FILE*archError)
{
    return fprintf(archError, "%s;%s\n", lineaError, motivoError) >= 0 ;
}

bool grabarArchBin(const s_miembros *miembros, FILE* archBin)
{
    return fwrite(miembros, sizeof(s_miembros),1,archBin) == 1;
}

bool parsearLineas(s_miembros *miembroTemp, char* buffer)
{
    char* token = strtok(buffer, ";\n") ; //dni
    if (!token)
        return false;
    
    miembroTemp->DNI = atol(token);

    token = strtok(NULL, ";\n") ; //apellido y nombre
    if (!token)
        return false;
    
    strcpy(miembroTemp->apellidoYnombre, token); 
    
    token = strtok(NULL, ";\n") ; //fecha nacimiento
    if (!token)
        return false;

    sscanf(token,"%d/%d/%d",&miembroTemp->fecha_nacimiento.dia,&miembroTemp->fecha_nacimiento.mes,&miembroTemp->fecha_nacimiento.anio);

    token = strtok(NULL, ";\n") ; //sexo
    if (!token)
        return false;

    miembroTemp->sexo = token[0];

    token = strtok(NULL, ";\n") ; //fecha de afiliacion
    if (!token)
        return false;

    sscanf(token,"%d/%d/%d",&miembroTemp->fecha_afiliacion.dia,&miembroTemp->fecha_afiliacion.mes,&miembroTemp->fecha_afiliacion.anio);

    token = strtok(NULL, ";\n") ; //fecha de ultima cuota
    if (!token)
        return false;

    sscanf(token,"%d/%d/%d",&miembroTemp->fecha_ultima_cuota_paga.dia,&miembroTemp->fecha_ultima_cuota_paga.mes,&miembroTemp->fecha_ultima_cuota_paga.anio);

    token = strtok(NULL, ";\n") ; //plan
    if (!token)
        return false;

    strcpy(miembroTemp->plan,token);

    token = strtok(NULL, ";\n") ; //email tutor 
    if (token)
        strcpy(miembroTemp->email_tutor,token);
    else
        strcpy(miembroTemp->email_tutor, "");

    return true;
}

bool leerYGenerarArchivo()
{
    char nombreArchivoBin[50];
    char nombreArchivoError[50];

    sprintf(nombreArchivoBin, "miembros-VC-%04d%02d%02d.dat", fechaProcesoGlobal.anio, fechaProcesoGlobal.mes, fechaProcesoGlobal.dia);
    sprintf(nombreArchivoError, "error-VC-%04d%02d%02d.txt", fechaProcesoGlobal.anio, fechaProcesoGlobal.mes, fechaProcesoGlobal.dia);

    char buffer[512];
    FILE* bin, *txt, *error;
    s_miembros miembroTemp;
    txt = fopen(arch_miembros_txt, "rt");
    if (!txt)
        printf("No se pudo abrir el archivo %s",arch_miembros_txt);

    bin = fopen (nombreArchivoBin,"wb");
    if (!bin)
    {               
        fclose(txt);
        printf("No se pudo crear el archivo %s",nombreArchivoBin);
    }    

    error = fopen(nombreArchivoError, "wt");
    if (!error)
    {
        fclose(txt);
        fclose(bin);
        printf("No se pudo crear el archivo %s",nombreArchivoError);
    }
    
   
    while (fgets(buffer, sizeof(buffer), txt) != NULL)
    {  
        //procesamiento de la linea del archivo texto
        char lineaLogError[512]; 
        strcpy(lineaLogError, buffer);//linea para el log de errores 

        if (buffer[0] == '#' || buffer[0] == '\n')
            continue;
        

        char motivoError[100];
        //PARSEAR LINEAS Y CARGAR UNA STRUCT TEMPORAL CON LOS REGISTROS
        if(parsearLineas(&miembroTemp, buffer))
        {
            //ponemos todos los registros con el estado A
            miembroTemp.estado = 'A';

            strcpy(miembroTemp.categoria, validarCategoria(miembroTemp.fecha_nacimiento, fechaProcesoGlobal));

             //VALIDAR REGISTRO COMPLETO Y MANEJO DE ERRORES DE ESCRITUR
            if(!validarRegistros(&miembroTemp,&fechaProcesoGlobal,motivoError))
            {    
                if(!grabarArchError(lineaLogError, motivoError, error))
                    printf("Error al grabar el archivo %s",nombreArchivoError);
            }
            else
            {
                if (!grabarArchBin(&miembroTemp, bin))
                    printf("Error al grabar el archivo %s",nombreArchivoBin);
            }
        }
        else
            if(!grabarArchError(lineaLogError, "Error de formato de linea", error))   
                puts("ERROR al graba el archivo de errores\n");

        
    }

    printf("\nProceso de migracion finalizado.\n");
    fclose(txt);
    fclose(bin);
    fclose(error);

    return true;
}
    
int cmp(const void *dato1, const void *dato2)
{
    
    t_reg_indice* dat1 = (const t_reg_indice*)dato1;
    t_reg_indice* dat2 = (const t_reg_indice*)dato2;

    if(dat1->dni < dat2->dni)
        return -1;
    else if( dat1->dni  > dat2->dni)
        return 1;
    else
        return 0;
}

void altaUsuario(const char *nombre_archivo_bin, t_indice *indice)
{
    FILE *pf = fopen(nombre_archivo_bin, "r+b");
    if(!pf)
    {

        pf = fopen(nombre_archivo_bin, "w+b");
    }

    if(!pf)
    {
        printf("Error fatal: No se pudo abrir/crear el archivo binario %s\n", nombre_archivo_bin);
        return; 
    }

    s_miembros nuevoMiembro;
    t_reg_indice regIndice;
    long posArchivo;

    printf("\n--- Alta de Nuevo Miembro ---\n");
    printf("Ingresar DNI: ");
    scanf("%ld", &nuevoMiembro.DNI);


    regIndice.dni = nuevoMiembro.DNI;


    if(indice_buscar(indice, &regIndice, indice->cantidad_elementos_actual, sizeof(t_reg_indice), cmp) != NO_EXISTE)
    {
        printf("\nERROR: El miembro con DNI %ld ya existe.\n", nuevoMiembro.DNI);
        fclose(pf);
        return;
    }


    printf("Nombre y apellido: ");
    fflush(stdin);

    fgets(nuevoMiembro.apellidoYnombre, sizeof(nuevoMiembro.apellidoYnombre), stdin);
    nuevoMiembro.apellidoYnombre[strcspn(nuevoMiembro.apellidoYnombre, "\n")] = 0; 


    normalizarCadena(nuevoMiembro.apellidoYnombre);
    printf("Nombre normalizado: %s\n", nuevoMiembro.apellidoYnombre);

    printf("Fecha de nacimiento (dd mm aaaa): ");
    scanf("%d %d %d", &nuevoMiembro.fecha_nacimiento.dia, &nuevoMiembro.fecha_nacimiento.mes, &nuevoMiembro.fecha_nacimiento.anio);

    printf("Sexo (M/F): ");
    fflush(stdin);
    scanf(" %c", &nuevoMiembro.sexo);
    nuevoMiembro.sexo = toupper(nuevoMiembro.sexo);

    printf("Fecha de afiliacion (dd mm aaaa): ");
    scanf("%d %d %d", &nuevoMiembro.fecha_afiliacion.dia, &nuevoMiembro.fecha_afiliacion.mes, &nuevoMiembro.fecha_afiliacion.anio);

    printf("Fecha ultima cuota paga (dd mm aaaa): ");
    scanf("%d %d %d", &nuevoMiembro.fecha_ultima_cuota_paga.dia, &nuevoMiembro.fecha_ultima_cuota_paga.mes, &nuevoMiembro.fecha_ultima_cuota_paga.anio);

    printf("Plan (BASIC, PREMIUM, VIP, FAMILY): ");
    fflush(stdin);
    scanf("%s", nuevoMiembro.plan);
    // (Deberías normalizar el plan a mayúsculas)

    // El estado se crea con 'A' por defecto (según TP)
    nuevoMiembro.estado = 'A';

    // La categoría se calcula (según TP)
    strcpy(nuevoMiembro.categoria, validarCategoria(nuevoMiembro.fecha_nacimiento, fechaProcesoGlobal));
    printf("Categoria calculada: %s\n", nuevoMiembro.categoria);

    // Limpiamos el email del tutor
    strcpy(nuevoMiembro.email_tutor, "");

    // Si es menor, pedimos email (según headers.h)
    if (esMenorDeEdad(nuevoMiembro.fecha_nacimiento, fechaProcesoGlobal))
    {
        printf("Email tutor (obligatorio para menores): ");
        fflush(stdin);
        scanf("%s", nuevoMiembro.email_tutor);
    }

    // --- VALIDACION FINAL ---
    char motivoError[100];
    // Usamos la función de headers.h y la variable global
    if(!validarRegistros(&nuevoMiembro, &fechaProcesoGlobal, motivoError))
    {
        printf("\nDatos incorrectos: %s. Alta cancelada.\n", motivoError);
        fclose(pf);
        return;
    }

    fseek(pf, 0, SEEK_END);
    posArchivo = ftell(pf) / sizeof(s_miembros); 
    fwrite(&nuevoMiembro, sizeof(s_miembros), 1, pf); 


    regIndice.dni = nuevoMiembro.DNI; 
    regIndice.nro_reg = posArchivo;     
    

    indice_insertar(indice, &regIndice, sizeof(t_reg_indice), cmp);

    printf("\nMiembro con DNI %ld agregado exitosamente en la posicion %ld.\n", nuevoMiembro.DNI, posArchivo);

    fclose(pf);
}

void bajaUsuario(const char *nombre_archivo_bin, t_indice *indice)
{
    FILE *pf = fopen(nombre_archivo_bin, "r+b"); // Necesitamos leer y escribir
    if (!pf)
    {
        printf("\nError al abrir el archivo binario %s.\n", nombre_archivo_bin);
        return;
    }

    long dni_baja;
    printf("\n--- Baja de Miembro ---\n");
    printf("Ingresar el DNI del miembro a dar de baja: ");
    scanf("%ld", &dni_baja);

    t_reg_indice reg_buscar;
    reg_buscar.dni = dni_baja;


    int pos_indice = indice_buscar(indice, &reg_buscar, indice->cantidad_elementos_actual, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("\nNo existe un miembro con el DNI %ld.\n", dni_baja);
        fclose(pf);
        return;
    }


    t_reg_indice *vindice_array = (t_reg_indice *)indice->vindice;
    long pos_registro_archivo = vindice_array[pos_indice].nro_reg;


    fseek(pf, pos_registro_archivo * sizeof(s_miembros), SEEK_SET);

    s_miembros miembro; 
    if (fread(&miembro, sizeof(s_miembros), 1, pf) != 1)
    {
        printf("\nError al leer el registro %ld del archivo.\n", pos_registro_archivo);
        fclose(pf);
        return;
    }


    if (miembro.estado == 'B')
    {
        printf("\nEl miembro con DNI %ld ya se encontraba dado de baja.\n", dni_baja);
    }
    else
    {

        miembro.estado = 'B';

        fseek(pf, pos_registro_archivo * sizeof(s_miembros), SEEK_SET);
        fwrite(&miembro, sizeof(s_miembros), 1, pf);
        
        printf("\nMiembro con DNI %ld dado de baja correctamente.\n", dni_baja);
    }

    indice_eliminar(indice, &reg_buscar, sizeof(t_reg_indice), cmp);

    fclose(pf);
}

void modificarUsuario(const char *nombre_archivo_bin, t_indice *indice)
{
    char* vOpc[8] = {
    "1- Apellido y Nombre",
    "2- Fecha de nacimiento",
    "3- Sexo",
    "4- Fecha de afiliacion",
    "5- Fecha de ultima cuota paga",
    "6- Plan",
    "7- Email del tutor",
    "0- Cancelar y Salir"
};

    FILE* pf;

    pf = fopen(nombre_archivo_bin, "r+b");
    if (!pf)
    {
        printf("\nError al abrir el archivo binario %s.\n", nombre_archivo_bin);
        return;
    }


    long dni_modif;
    printf("Ingresar el DNI del miembro a modificar: ");
    scanf("%ld", &dni_modif);

    t_reg_indice reg_modif;
    reg_modif.dni = dni_modif;

    int pos_indice = indice_buscar(indice,&reg_modif, indice->cantidad_elementos_actual,sizeof(t_reg_indice),cmp);

    if (pos_indice == NO_EXISTE)
        {
            printf("\nNo existe un miembro con el DNI %ld.\n", dni_modif);
            fclose(pf);
            return;
        }

    t_reg_indice *vindice_array = (t_reg_indice *)indice->vindice;
    long pos_registro_archivo = vindice_array[pos_indice].nro_reg;

    fseek(pf, pos_registro_archivo * sizeof(s_miembros), SEEK_SET);

    s_miembros miembro; 
    if (fread(&miembro, sizeof(s_miembros), 1, pf) != 1)
    {
        printf("\nError al leer el registro %ld del archivo.\n", pos_registro_archivo);
        fclose(pf);
        return;
    }

    puts("Que dato desea modificar del usuario? \n");
    for (int i = 0; i < 10; i++)
    {
        printf("%s\n", vOpc[i]);
    }
     int op = validar2(1,10);
    do
    {
      
       
        /*
        switch (op) // busco segun opcion, en cada caso del switch hago el pedido de nuevo dato y actualizo
        {
        case constant expression:

            break;
            default:
                break;
            }
        */
        

    } while (op != 0);
    
    fclose(pf);

}

















