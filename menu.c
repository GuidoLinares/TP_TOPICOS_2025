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

void alta_miembro(const char *nombre_archivo_bin, t_indice *indice,t_indice*indiceNomApe, t_fecha *fecha, int (*cmp)(const void*, const void*), int(*cmp_indice_nomape)(const void*,const void*))
{
    t_miembro nuevo;
    long dni_temp;

    printf("Ingresar DNI: ");
    scanf("%ld", &dni_temp);

    // VALIDACIÓN 1: DNI único y en rango
    if(!validar_dni_unico(indice, dni_temp, cmp))
    {
        while(getchar() != '\n');
        return;
    }

    nuevo.dni = dni_temp;
    while(getchar() != '\n');

    // NOMBRE Y APELLIDO
    printf("Nombre y apellido: ");
    if(!fgets(nuevo.apeynom, sizeof(nuevo.apeynom), stdin))
    {
        return;
    }
    nuevo.apeynom[strcspn(nuevo.apeynom, "\n")] = '\0';

    // VALIDACIÓN 2: Nombre
    if(!validar_nombre(nuevo.apeynom))
    {
        return;
    }

    // FECHA DE NACIMIENTO
    printf("Fecha de nacimiento (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_nac.dia, &nuevo.fecha_nac.mes, &nuevo.fecha_nac.anio);

    // VALIDACIÓN 3: Fecha de nacimiento (incluye edad >= 10)
    if(!validar_fecha_nacimiento(&nuevo.fecha_nac, fecha))
    {
        while(getchar() != '\n');
        return;
    }

    // FECHA DE AFILIACIÓN
    printf("Fecha de afiliación (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_afi.dia, &nuevo.fecha_afi.mes, &nuevo.fecha_afi.anio);

    // VALIDACIÓN 4: Fecha de afiliación
    if(!validar_fecha_afiliacion(&nuevo.fecha_afi, &nuevo.fecha_nac, fecha))
    {
        while(getchar() != '\n');
        return;
    }

    // SEXO
    printf("Sexo (M/F): ");
    while(getchar() != '\n');
    scanf("%c", &nuevo.sexo);

    // VALIDACIÓN 5: Sexo
    if(!validar_sexo(nuevo.sexo))
    {
        while(getchar() != '\n');
        return;
    }

    nuevo.sexo = toupper(nuevo.sexo);

    // CATEGORÍA
    printf("Categoria (ADULTO/MENOR): ");
    while(getchar() != '\n');
    fgets(nuevo.categoria, sizeof(nuevo.categoria), stdin);
    nuevo.categoria[strcspn(nuevo.categoria, "\n")] = '\0';

    for(int i = 0; nuevo.categoria[i]; i++) {
        nuevo.categoria[i] = toupper(nuevo.categoria[i]);
    }

    // VALIDACIÓN 6: Categoría (consistencia con edad)
    if(!validar_categoria(nuevo.categoria, &nuevo.fecha_nac, fecha))
    {
        return;
    }

    // FECHA ÚLTIMA CUOTA
    printf("Fecha ultima cuota paga (dd/mm/aaaa): ");
    scanf("%d/%d/%d", &nuevo.fecha_ult_cuo.dia, &nuevo.fecha_ult_cuo.mes, &nuevo.fecha_ult_cuo.anio);

    // VALIDACIÓN 7: Fecha última cuota
    if(!validar_fecha_ultima_cuota(&nuevo.fecha_ult_cuo, &nuevo.fecha_afi, fecha))
    {
        while(getchar() != '\n');
        return;
    }

    nuevo.estado = 'A';

    // PLAN
    printf("Plan (BASIC/PREMIUM/VIP/FAMILY): ");
    while(getchar() != '\n');
    fgets(nuevo.plan, sizeof(nuevo.plan), stdin);
    nuevo.plan[strcspn(nuevo.plan, "\n")] = '\0';

    for(int i = 0; nuevo.plan[i]; i++) {
        nuevo.plan[i] = toupper(nuevo.plan[i]);
    }

    // VALIDACIÓN 8: Plan
    if(!validar_plan(nuevo.plan))
    {
        return;
    }

    // EMAIL TUTOR (si es menor)
    int es_menor = (calcular_edad(&nuevo.fecha_nac, fecha) < 18);

    if(es_menor)
    {
        printf("Email tutor: ");
        fgets(nuevo.emailTutor, sizeof(nuevo.emailTutor), stdin);
        nuevo.emailTutor[strcspn(nuevo.emailTutor, "\n")] = '\0';
    }
    else
    {
        strcpy(nuevo.emailTutor, "");
    }

    // VALIDACIÓN 9: Email tutor (incluye validación de formato y regla adulto/menor)
    if(!validar_email_tutor(nuevo.emailTutor, es_menor))
    {
        return;
    }

    // TODAS LAS VALIDACIONES PASARON - AHORA SÍ GRABAR
    FILE *pf = fopen(nombre_archivo_bin, "ab");
    if(!pf)
    {
        printf("Error al abrir el archivo binario.\n");
        return;
    }

    // AGREGAR REGISTRO AL FINAL DEL ARCHIVO
    fseek(pf, 0, SEEK_END);
    long pos = ftell(pf);
    fwrite(&nuevo, sizeof(t_miembro), 1, pf);
    fclose(pf);

    // INSERTAR EN EL ÍNDICE
    t_reg_indice reg;
    reg.dni = nuevo.dni;
    reg.nro_reg = pos / sizeof(t_miembro);
    indice_insertar(indice, &reg, sizeof(t_reg_indice), cmp);

    t_reg_indice_apeynom reg_apeynom;
    strcpy(reg_apeynom.nombreApe, nuevo.apeynom);
    reg_apeynom.nro_reg = reg.nro_reg;

    indice_insertar(indiceNomApe, &reg_apeynom, sizeof(t_reg_indice_apeynom), cmp_indice_nomape);

    printf("\nMiembro agregado exitosamente.\n");
}

void baja_miembro(const char *nombre_archivo_bin, t_indice *indice, t_indice*indice_nomApe, int (*cmp)(const void*, const void*),int(*cmp_indice_nomape)(const void*,const void*))
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
    t_reg_indice_apeynom  indice_apeNom;
    strcpy(indice_apeNom.nombreApe, miembro.apeynom);
    indice_apeNom.nro_reg = pos_registro;
    // Marcar como dado de baja
    miembro.estado = 'B';

    // Reescribir el registro actualizado
    fseek(pf, pos_registro * sizeof(t_miembro), SEEK_SET);
    fwrite(&miembro, sizeof(t_miembro), 1, pf);

    // Eliminar del �ndice
    indice_eliminar(indice, &reg_buscar, sizeof(t_reg_indice), cmp);
    indice_eliminar(indice_nomApe, &indice_apeNom, sizeof(t_reg_indice_apeynom), cmp_indice_nomape);


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

void mostrar_estadisticas(char *arch_bin)
{

    t_miembro miembros;
    int contAltas = 0;
    int contBajas = 0;
    int contTotMiemb = 0;

    FILE*fp = fopen(arch_bin, "rb");
    if (!fp)
    {
        puts("error al abrir el archivo");
        exit(-1);
    }

    while (fread(&miembros, sizeof(t_miembro),1,fp))
    {
        contTotMiemb++;
        if (miembros.estado == 'A')
            contAltas ++;
        else if (miembros.estado == 'B')
            contBajas++;

    }

    printf("\n ESTADISTICAS DEL ARCHIVO %s",arch_bin);
    printf("\n TOTAL MIEMBROS: %d \n TOTAL MIEMBROS ACTIVOS: %d \n TOTAL MIEMBROS INACTIVOS: %d",contTotMiemb, contAltas, contBajas);
}

void listar_miembros_nombre(const t_indice *indice_apeynom, const char *nombre_archivo_bin)
{
    puts("\n--- Listado de Miembros Activos (Ordenados por Nombre) ---\n");

    if (indice_vacio(indice_apeynom))
    {
        puts("No hay miembros activos para listar.\n");
        return;
    }

    FILE *pf = fopen(nombre_archivo_bin, "rb");
    if (!pf)
    {
        puts("\nError al abrir el archivo binario.\n");
        return;
    }


    t_reg_indice_apeynom *vindice_ape = (t_reg_indice_apeynom *)indice_apeynom->vindice;

    t_miembro miembro;
    for (int i = 0; i < indice_apeynom->cantidad_elementos_actual; i++)
    {
        unsigned nro_reg = vindice_ape[i].nro_reg;
        long pos_bytes = nro_reg * sizeof(t_miembro);

        // Vamos a esa posici�n en el archivo
        fseek(pf, pos_bytes, SEEK_SET);


        fread(&miembro, sizeof(t_miembro),1,pf);

        printf("\n APELLIDO: %s \n DNI: %ld",miembro.apeynom, miembro.dni);
    }

    fclose(pf);
}

void top5CuotasAntiguas(const t_indice* indiceCuota, const char *nombre_archivo_bin)
{
    // t_reg_indice_cuota, t_miembro, y t_fecha deben estar definidas y accesibles.

    puts("\n--- Listado de las 5 cuotas mas antiguas (Ordenados por Fecha) ---\n");

    if (indice_vacio(indiceCuota))
    {
        puts("No hay miembros activos para listar.\n");
        return;
    }

    FILE*pf = fopen(nombre_archivo_bin, "rb");
    if (!pf)
    {
        puts("Error: No se pudo abrir el archivo binario de miembros.\n");
        return;
    }

    // Calcular el lmite: mnimo entre 5 y la cantidad actual de elementos
    const t_reg_indice_cuota *vIndiceCuota = (const t_reg_indice_cuota*)indiceCuota->vindice;
    t_miembro miembro_leido;
    int limite = (indiceCuota->cantidad_elementos_actual < 5) ? indiceCuota->cantidad_elementos_actual : 5;

    printf("Se muestran las %d cuotas ms antiguas (registros con menor fecha):\n\n", limite);

    // Encabezado de la tabla (opcional)
    printf("%-5s | %-10s | %-10s | %s\n", "NRO", "DNI", "ESTADO", "FECHA CUOTA");
    printf("-------------------------------------------\n");

    for (int i = 0; i < limite; i++)
    {
        unsigned nro_reg = vIndiceCuota[i].nro_reg;
        long pos_bytes = nro_reg * sizeof(t_miembro);

        fseek(pf, pos_bytes, SEEK_SET);

        if (fread(&miembro_leido, sizeof(t_miembro), 1, pf) == 1)
        {
            // Muestra los datos formateados
            printf("%-5u | %-10ld | %-10c | %02d/%02d/%04d\n",
                   nro_reg,
                   miembro_leido.dni,
                   miembro_leido.estado,
                   miembro_leido.fecha_ult_cuo.dia,
                   miembro_leido.fecha_ult_cuo.mes,
                   miembro_leido.fecha_ult_cuo.anio);
        }
        else
        {
            printf("%-5u | ERROR: No se pudo leer el registro en posicin %ld.\n", nro_reg, pos_bytes);
        }
    }

    fclose(pf);
}




