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
    
    long* dat1 = ((long*)dato1);
    long* dat2 = ((long*)dato2);

    if(dat1 < dat2)
        return -1;
    else if(dat1 > dat2)
        return 1;
    else
        return 0;
}

int extenderIndice (t_indice* indice)
{
    void* iniVec = indice->vindice;

    indice->cantidad_elementos_maxima *= 1.3;
    indice->vindice = realloc(indice->vindice, indice->cantidad_elementos_maxima * indice->cantidad_elementos_actual);
    if (!indice->vindice )
    {
        indice->vindice = iniVec;
        destruirIndice(indice); //ajustar para indice, no se si usar indice_eliminar o crear otra funcion
        return 0;
    }
    return 1;
}

bool altaUsuario(const char* nombreArchBin, t_indice* indice, int (*cmp)(const void*, const void*))
{
    s_miembros miembrosTemp;
    t_reg_indice indiceTemp;

    puts("------ PROCESO ALTA DE USUARIO ------");
    
    FILE* archBin = fopen(nombreArchBin, "ab");

    // 3. Lógica principal
    if (archBin == NULL) 
        printf("ERROR al abrir el archivo %s",nombreArchBin);

    do
    {
        puts("Ingrese DNI del usuario: ");
        scanf("%ld",&miembrosTemp.DNI);
        indiceTemp.dni = miembrosTemp.DNI;


        if (indice_buscar(indice, &indiceTemp,sizeof(t_reg_indice),sizeof(indiceTemp), cmp) != NO_EXISTE) //si devuelve true es que existe, descarto
        {            
            puts("ERROR DNI YA SE ENCUENTRA CARGADO");
            fclose(archBin);
            return false;    
        }           
        
        fflush(stdin);
        puts("Ingrese apellido y nombre: ");
        fgets(miembrosTemp.apellidoYnombre, 60, stdin);

        puts("Ingrese fecha de nacimiento (dd/mm/aaaa): ");
        scanf("%d",&miembrosTemp.fecha_nacimiento.dia);
        scanf("%d",&miembrosTemp.fecha_nacimiento.mes);
        scanf("%d",&miembrosTemp.fecha_nacimiento.anio);

        fflush(stdin);
        puts("Ingrese sexo (F o M): ");    
        scanf("%c",&miembrosTemp.sexo);

        puts("Ingrese fecha de afiliacion: ");
        scanf("%d",&miembrosTemp.fecha_afiliacion.dia);
        scanf("%d",&miembrosTemp.fecha_afiliacion.mes);
        scanf("%d",&miembrosTemp.fecha_afiliacion.anio);

        puts("Ingrese fecha de ultima cuota paga: ");
        scanf("%d",&miembrosTemp.fecha_ultima_cuota_paga.dia);
        scanf("%d",&miembrosTemp.fecha_ultima_cuota_paga.mes);
        scanf("%d",&miembrosTemp.fecha_ultima_cuota_paga.anio);

        fflush(stdin);
        puts("Ingrese plan: ");
        scanf("%s",miembrosTemp.plan);

        fflush(stdin);
        puts("Ingrese email del tutor: ");
        scanf("%s",miembrosTemp.email_tutor);


        if(!validarRegistros(&miembrosTemp,&fechaProcesoGlobal,"FALLO AL VALIDAR LOS REGISTROS"))
            puts("Fallo al validar los registros, ingrese nuevamente: ");
        

    } while (!validarRegistros(&miembrosTemp,&fechaProcesoGlobal,"FALLO AL VALIDAR LOS REGISTROS"));
        
    fwrite(&miembrosTemp, sizeof(s_miembros),1,archBin);
    indice_insertar(indice, &indiceTemp.dni,sizeof(t_reg_indice), cmp);

    fclose(archBin);
    return true;
}

/*Si existe la clave que se quiere dar de baja, actualizar el registro
correspondiente con un carácter ‘B’ en estado. Eliminar registro
correspondiente del índice.*/
bool bajaUsuario (const char* nombreArchBin, t_indice* indice, int (*cmp)(const void*, const void*))
{
    t_reg_indice* registro;

    

    if(!indice_buscar(indice, registro, sizeof(char*), sizeof(t_indice), cmp ) != ERROR)
    {

    }   
    

    return true;
}




