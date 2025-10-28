#include "menu.h"

void mostrar_menu (const char *menu[], int tam)
{
    puts("Seleccione algunas de las siguientes opciones:");
    for(int i = 0; i < tam; i++)
        printf("%s\n", menu[i]);
}

char capturar_opcion(char li, char ls)
{
    char opcion;
    do
    {
        scanf(" %c",&opcion);
        limpiar_buffer();

        if (opcion >= 'a' && opcion <= 'z')
            opcion = toupper(opcion);

        if(opcion < li || opcion > ls)
            puts("Opcion invalida, ingrese nuevamente: ");

    }
    while (opcion < li || opcion > ls);
    return opcion;
}

void alta_miembro(const char *nombre_archivo_bin, t_indice *indice, t_fecha *fecha, int (*cmp)(const void*, const void*))
{
    FILE *pf = fopen(nombre_archivo_bin, "ab");
    if(!pf)
    {
        printf("Error al abrir el archivo binario.\n");
        exit(1);
    }

    t_miembro nuevo;
    t_reg_indice reg_buscar, reg;
    long pos;

    printf("Ingresar DNI: ");
    scanf("%ld", &reg_buscar.dni);

    /// VERIFICAR EXISTENCIA EN EL �NDICE
    if(indice_buscar(indice, &reg_buscar, 0, sizeof(t_reg_indice), cmp) != NO_EXISTE)
    {
        printf("\nEl miembro con DNI %ld ya existe.\n", reg_buscar.dni);
        fclose(pf);
        return;
    }

    /// SI NO EXISTE, CARGAR DATOS
    nuevo.dni = reg_buscar.dni;

    // Limpiar buffer despu�s del primer scanf
    while(getchar() != '\n');

    printf("Nombre y apellido: ");
    fgets(nuevo.apeynom, sizeof(nuevo.apeynom), stdin);
    nuevo.apeynom[strcspn(nuevo.apeynom, "\n")] = '\0';

    printf("Fecha de nacimiento (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_nac.dia, &nuevo.fecha_nac.mes, &nuevo.fecha_nac.anio);

    printf("Fecha de afiliaci�n (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_afi.dia, &nuevo.fecha_afi.mes, &nuevo.fecha_afi.anio);

    printf("Sexo (M/F): ");
    while(getchar() != '\n');
    scanf("%c", &nuevo.sexo);
    nuevo.sexo = toupper(nuevo.sexo); // Convertir a may�scula

    printf("Categoria (ADULTO/MENOR): ");
    while(getchar() != '\n');
    fgets(nuevo.categoria, sizeof(nuevo.categoria), stdin);
    nuevo.categoria[strcspn(nuevo.categoria, "\n")] = '\0';
    // Convertir a may�sculas
    for(int i = 0; nuevo.categoria[i]; i++) {
        nuevo.categoria[i] = toupper(nuevo.categoria[i]);
    }

    printf("Fecha ultima cuota paga (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_ult_cuo.dia, &nuevo.fecha_ult_cuo.mes, &nuevo.fecha_ult_cuo.anio);

    nuevo.estado = 'A';

    printf("Plan (BASIC/PREMIUM/VIP/FAMILY): ");
    while(getchar() != '\n');
    fgets(nuevo.plan, sizeof(nuevo.plan), stdin);
    nuevo.plan[strcspn(nuevo.plan, "\n")] = '\0';
    // Convertir a may�sculas
    for(int i = 0; nuevo.plan[i]; i++) {
        nuevo.plan[i] = toupper(nuevo.plan[i]);
    }

    /// SI ES MENOR DE EDAD TIENE TUTOR CON EMAIL
    if(calcular_edad(&nuevo.fecha_nac, fecha) < 18)
    {
        printf("Email tutor: ");
        while(getchar() != '\n');
        fgets(nuevo.emailTutor, sizeof(nuevo.emailTutor), stdin);
        nuevo.emailTutor[strcspn(nuevo.emailTutor, "\n")] = '\0';
    }
    else
    {
        strcpy(nuevo.emailTutor, "");
    }

    char *error_msg = validar_miembro(&nuevo, fecha);
    if(error_msg != NULL)
    {
        printf("\nDatos incorrectos: %s\n", error_msg);
        printf("Alta cancelada.\n");
        fclose(pf);
        return;
    }

    /// AGREGAR REGISTRO AL FINAL DEL ARCHIVO
    fseek(pf, 0, SEEK_END);
    pos = ftell(pf);
    fwrite(&nuevo, sizeof(t_miembro), 1, pf);

    /// INSERTAR EN EL �NDICE
    reg.dni = nuevo.dni;
    reg.nro_reg = pos / sizeof(t_miembro);
    indice_insertar(indice, &reg, sizeof(t_reg_indice), cmp);

    printf("\nMiembro agregado exitosamente.\n");
    fclose(pf);
}

void baja_miembro(const char *nombre_archivo_bin, t_indice *indice, int (*cmp)(const void*, const void*))
{
    FILE *pf = fopen(nombre_archivo_bin, "r+b");
    if (!pf)
    {
        printf("\nError al abrir el archivo binario.\n");
        exit(1);
    }

    long dni_baja;
    printf("Ingresar el DNI del miembro a dar de baja: ");
    scanf("%ld", &dni_baja);

    t_reg_indice reg_buscar;
    reg_buscar.dni = dni_baja;

    // Buscar en el �ndice
    int pos_indice = indice_buscar(indice, &reg_buscar, 0, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("\nNo existe un miembro con el DNI ingresado.\n");
        fclose(pf);
        return;
    }

    // Acceder al �ndice para obtener la posici�n del registro en el archivo
    t_reg_indice *vindice = (t_reg_indice *)indice->vindice;
    long pos_registro = vindice[pos_indice].nro_reg;

    fseek(pf, pos_registro * sizeof(t_miembro), SEEK_SET);

    t_miembro miembro;
    if (fread(&miembro, sizeof(t_miembro), 1, pf) != 1)
    {
        printf("\nError al leer el registro del archivo.\n");
        fclose(pf);
        return;
    }

    // Marcar como dado de baja
    miembro.estado = 'B';

    // Reescribir el registro actualizado
    fseek(pf, pos_registro * sizeof(t_miembro), SEEK_SET);
    fwrite(&miembro, sizeof(t_miembro), 1, pf);

    // Eliminar del �ndice
    indice_eliminar(indice, &reg_buscar, sizeof(t_reg_indice), cmp);

    printf("\nMiembro con DNI %ld dado de baja correctamente.\n", dni_baja);

    fclose(pf);
}

void mostrar_miembro(const t_indice *indice, const char *nombre_archivo_bin, int (*cmp)(const void*, const void*))
{
    // Pedir el DNI
    long dni_buscar;
    printf("\n--- Mostrar Informacion de Miembro ---\n");
    printf("Ingresar el DNI del miembro a buscar: ");
    scanf("%ld", &dni_buscar);
    limpiar_buffer(); // Limpiar el \n

    t_reg_indice reg_buscar;
    reg_buscar.dni = dni_buscar;

    // Buscar en el �ndice
    int pos_indice = indice_buscar(indice, &reg_buscar, 0, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("\nError: No existe un miembro con el DNI %ld.\n", dni_buscar);
        return;
    }

    // Abrir archivo y leer el registro
    FILE *pf = fopen(nombre_archivo_bin, "rb"); // Solo "rb" (lectura)
    if (!pf)
    {
        printf("\nError al abrir el archivo binario.\n");
        return;
    }

    // Acceder al �ndice para obtener la posici�n del registro en el archivo
    t_reg_indice *vindice = (t_reg_indice *)indice->vindice;
    long pos_registro = vindice[pos_indice].nro_reg;

    // Ir a la posici�n y leer
    fseek(pf, pos_registro * sizeof(t_miembro), SEEK_SET);

    t_miembro miembro;
    if (fread(&miembro, sizeof(t_miembro), 1, pf) != 1)
    {
        printf("\nError al leer el registro del archivo.\n");
        fclose(pf);
        return;
    }

    fclose(pf); // Cerramos el archivo

    // Mostrar toda la informaci�n
    printf("\n--- Datos del Miembro ---\n");
    printf("DNI: \t\t\t%ld\n", miembro.dni);
    printf("Nombre y Apellido: \t%s\n", miembro.apeynom);
    printf("Fecha de Nacimiento: \t%02d/%02d/%04d\n", miembro.fecha_nac.dia, miembro.fecha_nac.mes, miembro.fecha_nac.anio);
    printf("Sexo: \t\t\t%c\n", miembro.sexo);
    printf("Fecha de Afiliacion: \t%02d/%02d/%04d\n", miembro.fecha_afi.dia, miembro.fecha_afi.mes, miembro.fecha_afi.anio);
    printf("Categoria: \t\t%s\n", miembro.categoria);
    printf("Ultima Cuota Paga: \t%02d/%02d/%04d\n", miembro.fecha_ult_cuo.dia, miembro.fecha_ult_cuo.mes, miembro.fecha_ult_cuo.anio);
    printf("Estado: \t\t%c\n", miembro.estado);
    printf("Plan: \t\t\t%s\n", miembro.plan);
    if(miembro.emailTutor[0] != '\0')
    {
        printf("Email Tutor: \t\t%s\n", miembro.emailTutor);
    }
    printf("---------------------------\n");
}

void listar_miembros_dni(const t_indice *indice, const char *nombre_archivo_bin)
{
    puts("\n--- Listado de Miembros Activos (Ordenados por DNI) ---\n");
    // Verificamos si el �ndice est� vac�o
    if (indice_vacio(indice))
    {
        puts("No hay miembros activos para listar.\n");
        return;
    }

    // Abrimos el archivo binario en modo lectura
    FILE *pf = fopen(nombre_archivo_bin, "rb");
    if (!pf)
    {
        puts("\nError al abrir el archivo binario.\n");
        return;
    }

    // Recorremos el array del �ndice (que ya est� ordenado)
    t_reg_indice *vindice = (t_reg_indice *)indice->vindice;
    t_miembro miembro;
    long pos_bytes;

    printf("\n%-10s %-30s %-15s %-10s\n", "DNI", "Apellido y Nombre", "F. Afiliacion", "Plan");
    puts("--------------------------------------------------------------------\n");

    for (int i = 0; i < indice->cantidad_elementos_actual; i++)
    {
        // Obtenemos la posici�n en el archivo del registro actual del �ndice
        pos_bytes = vindice[i].nro_reg * sizeof(t_miembro);

        // Vamos a esa posici�n en el archivo
        fseek(pf, pos_bytes, SEEK_SET);

        // Leemos el miembro completo
        if (fread(&miembro, sizeof(t_miembro), 1, pf) == 1)
        {
            // Mostramos los datos relevantes
            printf("%-10ld %-30s %02d/%02d/%04d      %-10s\n",
                   miembro.dni,
                   miembro.apeynom,
                   miembro.fecha_afi.dia, miembro.fecha_afi.mes, miembro.fecha_afi.anio,
                   miembro.plan);
        }
    }
    puts("--------------------------------------------------------------------\n");
    fclose(pf);
}

void listar_miembros_plan(const t_indice *indice, const char *nombre_archivo_bin)
{
    puts
    ("\n--- Listado de Miembros Activos (Agrupados por Plan, Ordenados por DNI) ---\n");

    // 1. Verificar si hay miembros
    if (indice_vacio(indice))
    {
        puts("No hay miembros activos para listar.\n");
        return;
    }

    // 2. Pedir memoria para todos los miembros activos
    int num_activos = indice->cantidad_elementos_actual;
    t_miembro *miembros_activos = (t_miembro*)malloc(num_activos * sizeof(t_miembro));

    if (miembros_activos == NULL)
    {
        puts("Error: No hay suficiente memoria para cargar los miembros.\n");
        return;
    }

    // 3. Abrir el archivo binario
    FILE *pf = fopen(nombre_archivo_bin, "rb");
    if (!pf)
    {
        puts("\nError al abrir el archivo binario.\n");
        free(miembros_activos); // Liberar memoria antes de salir
        return;
    }

    // 4. Leer todos los miembros activos al array usando el �ndice
    t_reg_indice *vindice = (t_reg_indice *)indice->vindice;
    long pos_bytes;
    int miembros_cargados = 0; // Contador de miembros realmente le�dos y v�lidos
    for (int i = 0; i < num_activos; i++)
    {
        pos_bytes = vindice[i].nro_reg * sizeof(t_miembro);
        fseek(pf, pos_bytes, SEEK_SET);
        // Intentamos leer en la posici�n 'miembros_cargados' del array
        if (fread(&miembros_activos[miembros_cargados], sizeof(t_miembro), 1, pf) == 1)
        {
            // Verificamos si realmente est� activo (aunque el �ndice deber�a ser correcto)
            if (miembros_activos[miembros_cargados].estado == 'A')
            {
                miembros_cargados++; // Solo incrementamos si le�mos y est� activo
            }
        }
    }
    fclose(pf);

    // Actualizamos num_activos por si alg�n registro no se pudo leer o estaba inactivo
    num_activos = miembros_cargados;

    if (num_activos == 0)
    {
        printf("No se encontraron miembros activos v�lidos.\n");
        free(miembros_activos);
        return;
    }


    // 5. Ordenar el array en memoria
    // Usamos num_activos actualizado
    qsort(miembros_activos, num_activos, sizeof(t_miembro), cmp_plan_dni);

    // 6. Imprimir el array ordenado con encabezados de grupo
    printf("\n%-10s %-30s %-15s\n", "DNI", "Apellido y Nombre", "F. Afiliacion");
    printf("------------------------------------------------------------\n");

    char plan_actual[11] = ""; // Para rastrear cambios de plan
    for (int i = 0; i < num_activos; i++)
    {
        // Verificar si cambi� el plan para imprimir un nuevo encabezado
        if (strcmp(miembros_activos[i].plan, plan_actual) != 0)
        {
            strcpy(plan_actual, miembros_activos[i].plan);
            printf("\n--- Plan: %s ---\n", plan_actual);
        }

        // Imprimir detalles del miembro
        printf("%-10ld %-30s %02d/%02d/%04d\n",
               miembros_activos[i].dni,
               miembros_activos[i].apeynom,
               miembros_activos[i].fecha_afi.dia, miembros_activos[i].fecha_afi.mes, miembros_activos[i].fecha_afi.anio);
    }

    puts("------------------------------------------------------------\n");

    // 7. Liberar la memoria pedida
    free(miembros_activos);
}

int cmp_plan_dni(const void *a, const void *b)
{
    // Convertir los punteros void* a punteros t_miembro*
    t_miembro *m1 = (t_miembro*)a;
    t_miembro *m2 = (t_miembro*)b;

    // 1. Comparar por Plan primero
    int cmp_plan = strcmp(m1->plan, m2->plan);

    if (cmp_plan != 0)
    {
        // Los planes son diferentes, devolver el resultado de la comparaci�n de planes
        return cmp_plan;
    }
    else
    {
        // 2. Los planes son iguales, comparar por DNI
        if (m1->dni < m2->dni) return -1;
        if (m1->dni > m2->dni) return 1;
        return 0; // Los DNI tambi�n son iguales (no deber�a pasar con DNI �nicos)
    }
}

void modificar_miembro(const char *arch_bin, t_indice *pIndice, t_fecha *fecha_proceso, int (*cmp)(const void*, const void*))
{
    printf("\n--- Modificacion de Miembro ---\n");

    // 1. Pedir DNI
    long dni_modificar;
    printf("Ingrese DNI del miembro a modificar: ");
    scanf("%ld", &dni_modificar);
    limpiar_buffer();

    t_reg_indice reg_buscar;
    reg_buscar.dni = dni_modificar;

    // 2. Buscar en el indice
    int pos_indice = indice_buscar(pIndice, &reg_buscar, 0, sizeof(t_reg_indice), cmp);

    if (pos_indice == NO_EXISTE)
    {
        printf("\nError: El DNI %ld no existe en el sistema.\n", dni_modificar);
        return;
    }

    // 3. Obtener nro registro
    t_reg_indice *vindice = (t_reg_indice *)pIndice->vindice;
    long nro_reg_original = vindice[pos_indice].nro_reg;

    // 4. Abrir archivo binario
    FILE *fp = fopen(arch_bin, "r+b");
    if (!fp)
    {
        printf("\nError al abrir el archivo binario.\n");
        return;
    }

    t_miembro miembro_original, miembro_modificado;
    long pos_bytes = nro_reg_original * sizeof(t_miembro);

    fseek(fp, pos_bytes, SEEK_SET);
    if (fread(&miembro_original, sizeof(t_miembro), 1, fp) != 1)
    {
        printf("\nError al leer el registro original del archivo.\n");
        fclose(fp);
        return;
    }

    miembro_modificado = miembro_original;

    // 5. Submenu
    char opcion;
    do
    {
        printf("\n--- Datos actuales del miembro ---\n");
        printf("1. Apellido y Nombre: %s\n", miembro_modificado.apeynom);
        printf("2. Fecha Nacimiento: %02d/%02d/%04d\n", miembro_modificado.fecha_nac.dia, miembro_modificado.fecha_nac.mes, miembro_modificado.fecha_nac.anio);
        printf("3. Sexo: %c\n", miembro_modificado.sexo);
        printf("4. Categoria: %s\n", miembro_modificado.categoria);
        printf("5. Fecha Ultima Cuota: %02d/%02d/%04d\n", miembro_modificado.fecha_ult_cuo.dia, miembro_modificado.fecha_ult_cuo.mes, miembro_modificado.fecha_ult_cuo.anio);
        printf("6. Estado (A/B): %c\n", miembro_modificado.estado);
        printf("7. Plan: %s\n", miembro_modificado.plan);
        printf("8. Email Tutor: %s\n", miembro_modificado.emailTutor);
        printf("0. Guardar y salir\n");

        printf("\nIngrese campo a modificar (0-8): ");
        scanf(" %c", &opcion);
        limpiar_buffer();

        switch(opcion)
        {
        case '1':
            printf("Nuevo Apellido y Nombre: ");
            fgets(miembro_modificado.apeynom, sizeof(miembro_modificado.apeynom), stdin);
            miembro_modificado.apeynom[strcspn(miembro_modificado.apeynom, "\n")] = 0;
            break;

        case '2':
            printf("Nueva Fecha de Nacimiento (dd mm aaaa): ");
            scanf("%d %d %d", &miembro_modificado.fecha_nac.dia, &miembro_modificado.fecha_nac.mes, &miembro_modificado.fecha_nac.anio);
            limpiar_buffer();
            break;

        case '3':
            printf("Nuevo Sexo (M/F): ");
            scanf(" %c", &miembro_modificado.sexo);
            limpiar_buffer();
            break;

        case '4':
            printf("Nueva Categoria: ");
            fgets(miembro_modificado.categoria, sizeof(miembro_modificado.categoria), stdin);
            miembro_modificado.categoria[strcspn(miembro_modificado.categoria, "\n")] = 0;
            break;

        case '5':
            printf("Nueva Fecha Ultima Cuota (dd mm aaaa): ");
            scanf("%d %d %d", &miembro_modificado.fecha_ult_cuo.dia, &miembro_modificado.fecha_ult_cuo.mes, &miembro_modificado.fecha_ult_cuo.anio);
            limpiar_buffer();
            break;

        case '6':
            printf("Nuevo Estado (A/B): ");
            scanf(" %c", &miembro_modificado.estado);
            limpiar_buffer();
            break;

        case '7':
            printf("Nuevo Plan: ");
            fgets(miembro_modificado.plan, sizeof(miembro_modificado.plan), stdin);
            miembro_modificado.plan[strcspn(miembro_modificado.plan, "\n")] = 0;
            break;

        case '8':
            printf("Nuevo Email Tutor (si no corresponde, dejar vacio): ");
            fgets(miembro_modificado.emailTutor, sizeof(miembro_modificado.emailTutor), stdin);
            miembro_modificado.emailTutor[strcspn(miembro_modificado.emailTutor, "\n")] = 0;
            break;

        case '0':
            printf("\nSaliendo del menu de modificacion...\n");
            break;

        default:
            printf("\nOpcion incorrecta.\n");
        }
    }
    while(opcion != '0');

    // 6. Validacion final
    const char* motivo_error = validar_miembro(&miembro_modificado, fecha_proceso);

    if (motivo_error != NULL)
    {
        printf("\nError: Los datos modificados son invalidos (%s).\n", motivo_error);
        printf("Se descartaron todos los cambios.\n");
    }
    else
    {
        // 7. Guardar cambios
        fseek(fp, pos_bytes, SEEK_SET);
        fwrite(&miembro_modificado, sizeof(t_miembro), 1, fp);
        printf("\nModificacion realizada con exito.\n");

        // 8. Actualizar indice si cambia estado
        if (miembro_original.estado == 'A' && miembro_modificado.estado == 'B')
        {
            printf("El miembro fue dado de baja. Eliminando del indice...\n");
            indice_eliminar(pIndice, &reg_buscar, sizeof(t_reg_indice), cmp);
        }
        else if (miembro_original.estado == 'B' && miembro_modificado.estado == 'A')
        {
            printf("El miembro fue reactivado. Insertando en el indice...\n");
            t_reg_indice reg_reinsertar;
            reg_reinsertar.dni = miembro_modificado.dni;
            reg_reinsertar.nro_reg = nro_reg_original;
            indice_insertar(pIndice, &reg_reinsertar, sizeof(t_reg_indice), cmp);
        }
    }

    fclose(fp);
}

void mostrar_miembros_inactivos(const char* arch_bin)
{
    t_miembro miembro;

    FILE*fp = fopen(arch_bin, "rb");
    if (!fp)
    {
        puts("No se pudo abrir el archivo");
        exit (-1);
    }

    puts("------LISTADO DE MIEMBROS DADOS DE BAJA------");
    while (fread(&miembro, sizeof(t_miembro),1,fp) == 1)
    {   

        if (miembro.estado == 'B')
        {
            printf("NOMBRE Y APELLIDO: %s",miembro.apeynom);
            printf("CATEGORIA: %s",miembro.categoria);
            printf("DNI: %ld",miembro.dni);
            printf("EMAIL TUTOR: %s",miembro.emailTutor);
            printf("FECHA AFILIACION: %d-%d-%d",miembro.fecha_afi.dia,miembro.fecha_afi.mes,miembro.fecha_afi.anio);
            printf("FECHA NACIMIENTO: %d-%d-%d",miembro.fecha_nac.dia,miembro.fecha_nac.mes,miembro.fecha_nac.anio);
            printf("FECHA ULTIMA CUOTA: %d-%d-%d",miembro.fecha_ult_cuo.dia,miembro.fecha_ult_cuo.mes,miembro.fecha_ult_cuo.anio);
            printf("ESTADO: %c",miembro.estado);
            printf("PLAN: %s",miembro.plan);
            printf("SEXO: %c",miembro.sexo);
        }
        
    }
    fclose(fp);
}





