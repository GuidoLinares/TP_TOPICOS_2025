#include "validacion.h"

void normalizar(char *cad)
{
    char temp[60];
    char *lect = cad;    // Puntero de lectura sobre el string original
    char *esc = temp;    // Puntero de escritura sobre el buffer temporal
    int primera_letra_palabra;
    int primera_palabra_procesada = 0; // Flag para saber si ya procesamos la 1ra palabra

    while (*lect)
    {
        // 1. Saltar todos los espacios Y comas al inicio o entre palabras
        while (*lect && (isspace(*lect) || *lect == ','))
            lect++;

        if (*lect) // Si encontramos el inicio de una palabra
        {
            // 2. Si NO es la primera palabra que escribimos...
            if (primera_palabra_procesada)
            {
                *esc++ = ' '; // ...agregar un espacio antes
            }

            // 3. Procesar la palabra
            primera_letra_palabra = 1;
            // Leer mientras no sea espacio, no sea coma, y no sea fin de string
            while (*lect && !isspace(*lect) && *lect != ',')
            {
                *esc = primera_letra_palabra ? toupper(*lect) : tolower(*lect);
                primera_letra_palabra = 0;
                esc++;
                lect++;
            }

            // 4. Si FUE la primera palabra que procesamos...
            if (!primera_palabra_procesada)
            {
                *esc++ = ',';
                primera_palabra_procesada = 1; // Marcar que ya pas� la primera
            }
        }
    }
    *esc = '\0'; // Terminar el string temporal
    strcpy(cad, temp);  // Copiar el resultado final
}

void limpiar_buffer()
{
    int c;
    while ((c = getchar()) != '\n');
}

char* validar_miembro(const t_miembro *miembro, const t_fecha *fecha_proceso)
{
    int edad;
    
    if (miembro->dni < 1000001 || miembro->dni > 99999999)
        return "DNI: Fuera de rango (1000001-99999999)";

    if (!esFechaValida(&miembro->fecha_nac))
    {
        return "Fecha Nacimiento: Fecha invalida";
    }

    edad = calcular_edad(&miembro->fecha_nac, fecha_proceso);

    if (edad < 10)
    {
        return "Fecha Nacimiento: El miembro debe tener al menos 10 anios";
    }

    if (miembro->sexo != 'F' && miembro->sexo != 'M')
    {
        return "Sexo: Valor invalido (debe ser 'F' o 'M')";
    }

    if (!esFechaValida(&miembro->fecha_afi))
    {
        return "Fecha Afiliacion: Fecha invalida";
    }

    if (cmp_fechas(&miembro->fecha_afi, fecha_proceso) > 0)
    {
        return "Fecha Afiliacion: No puede ser posterior a la fecha de proceso";
    }

    if (cmp_fechas(&miembro->fecha_afi, &miembro->fecha_nac) <= 0)
    {
        return "Fecha Afiliacion: Debe ser posterior a la fecha de nacimiento";
    }

    // Comprobación de Categoría vs Edad
    if (edad < 18)
    {
        if (strcmp(miembro->categoria, "MENOR") != 0)
        {
            return "Categoria: Inconsistente (deberia ser MENOR)";
        }
    }
    else // edad >= 18
    {
        if (strcmp(miembro->categoria, "ADULTO") != 0)
        {
            return "Categoria: Inconsistente (deberia ser ADULTO)";
        }
    }

    if (!esFechaValida(&miembro->fecha_ult_cuo))
    {
        return "Fecha Ultima Cuota: Fecha invalida";
    }

    // Corregido: La cuota puede ser igual o posterior a la afiliación
    if (cmp_fechas(&miembro->fecha_ult_cuo, &miembro->fecha_afi) < 0)
    {
        return "Fecha Ultima Cuota: Debe ser posterior o igual a la fecha de afiliacion";
    }

    if (cmp_fechas(&miembro->fecha_ult_cuo, fecha_proceso) > 0)
    {
        return "Fecha Ultima Cuota: No puede ser posterior a la fecha de proceso";
    }

    // El estado 'B' es válido si viene del TXT, pero 'A' es requerido para nuevos registros (Primera Parte)
    // OJO: Esta validación puede ser distinta para 'modificar_miembro' si permites cambiar el estado.
    if (miembro->estado != 'A' && miembro->estado != 'B')
    {
        return "Estado: Valor invalido (debe ser 'A' o 'B')";
    }


    if (strcmp(miembro->plan, "BASIC") != 0 &&
        strcmp(miembro->plan, "PREMIUM") != 0 &&
        strcmp(miembro->plan, "VIP") != 0 &&
        strcmp(miembro->plan, "FAMILY") != 0)
    {
        return "Plan: Valor invalido (debe ser BASIC, PREMIUM, VIP o FAMILY)";
    }

    // Validación de Email Tutor
    if (edad < 18)
    {
        if (miembro->emailTutor[0] == '\0')
        {
            return "Email Tutor: Menor de edad debe tener un email de tutor";
        }
        if (!email_valido(miembro->emailTutor))
        {
            return "Email Tutor: Formato de email invalido";
        }
    }
    else // es Adulto
    {
        // Si es adulto, NO debe tener email de tutor.
        // (Si la regla es "puede tenerlo pero no es obligatorio", borra este 'else')
        if (miembro->emailTutor[0] != '\0')
        {
            return "Email Tutor: Los adultos no deben tener email de tutor";
        }
    }

    // Si pasó todas las validaciones
    return NULL;
}

int validar_dni_unico(t_indice *indice, long dni, int (*cmp)(const void*, const void*))
{
    // Validar rango de DNI
    if (dni < 1000001 || dni > 99999999)
    {
        printf("\nError: DNI fuera de rango (1000001-99999999).\n");
        return 0;
    }
    
    // Validar que no exista en el índice
    t_reg_indice reg_buscar;
    reg_buscar.dni = dni;
    
    if(indice_buscar(indice, &reg_buscar, 0, sizeof(t_reg_indice), cmp) != NO_EXISTE)
    {
        printf("\nError: El miembro con DNI %ld ya existe.\n", dni);
        return 0;
    }
    return 1;
}

int validar_nombre(const char *apeynom)
{
    if(strlen(apeynom) == 0)
    {
        printf("\nError: El nombre no puede estar vacío.\n");
        return 0;
    }
    return 1;
}

int validar_fecha_nacimiento(t_fecha *fecha_nac, t_fecha *fecha_actual)
{
    if(!esFechaValida(fecha_nac))
    {
        printf("\nError: Fecha de nacimiento inválida.\n");
        return 0;
    }
    
    if(cmp_fechas(fecha_nac, fecha_actual) > 0)
    {
        printf("\nError: La fecha de nacimiento no puede ser futura.\n");
        return 0;
    }
    
    int edad = calcular_edad(fecha_nac, fecha_actual);
    if(edad < 10)
    {
        printf("\nError: El miembro debe tener al menos 10 años.\n");
        return 0;
    }
    
    return 1;
}

int validar_fecha_afiliacion(t_fecha *fecha_afi, t_fecha *fecha_nac, t_fecha *fecha_actual)
{
    if(!esFechaValida(fecha_afi))
    {
        printf("\nError: Fecha de afiliación inválida.\n");
        return 0;
    }
    
    if(cmp_fechas(fecha_afi, fecha_actual) > 0)
    {
        printf("\nError: La fecha de afiliación no puede ser posterior a la fecha de proceso.\n");
        return 0;
    }
    
    if(cmp_fechas(fecha_afi, fecha_nac) <= 0)
    {
        printf("\nError: La fecha de afiliación debe ser posterior a la fecha de nacimiento.\n");
        return 0;
    }
    
    return 1;
}

int validar_sexo(char sexo)
{
    char sexo_upper = toupper(sexo);
    if(sexo_upper != 'M' && sexo_upper != 'F')
    {
        printf("\nError: El sexo debe ser 'M' o 'F'.\n");
        return 0;
    }
    return 1;
}

int validar_categoria(const char *categoria, t_fecha *fecha_nac, t_fecha *fecha_actual)
{
    int edad = calcular_edad(fecha_nac, fecha_actual);
    
    if(strcmp(categoria, "ADULTO") != 0 && strcmp(categoria, "MENOR") != 0)
    {
        printf("\nError: La categoría debe ser 'ADULTO' o 'MENOR'.\n");
        return 0;
    }
    
    if(edad < 18 && strcmp(categoria, "MENOR") != 0)
    {
        printf("\nError: Categoría inconsistente (debería ser MENOR).\n");
        return 0;
    }
    
    if(edad >= 18 && strcmp(categoria, "ADULTO") != 0)
    {
        printf("\nError: Categoría inconsistente (debería ser ADULTO).\n");
        return 0;
    }
    
    return 1;
}

int validar_fecha_ultima_cuota(t_fecha *fecha_ult_cuo, t_fecha *fecha_afi, t_fecha *fecha_actual)
{
    if(!esFechaValida(fecha_ult_cuo))
    {
        printf("\nError: Fecha de última cuota inválida.\n");
        return 0;
    }
    
    if(cmp_fechas(fecha_ult_cuo, fecha_afi) < 0)
    {
        printf("\nError: La fecha de última cuota debe ser posterior o igual a la fecha de afiliación.\n");
        return 0;
    }
    
    if(cmp_fechas(fecha_ult_cuo, fecha_actual) > 0)
    {
        printf("\nError: La fecha de última cuota no puede ser posterior a la fecha de proceso.\n");
        return 0;
    }
    
    return 1;
}

int validar_plan(const char *plan)
{
    if(strcmp(plan, "BASIC") != 0 && 
       strcmp(plan, "PREMIUM") != 0 && 
       strcmp(plan, "VIP") != 0 && 
       strcmp(plan, "FAMILY") != 0)
    {
        printf("\nError: El plan debe ser 'BASIC', 'PREMIUM', 'VIP' o 'FAMILY'.\n");
        return 0;
    }
    return 1;
}

int validar_email_tutor(const char *emailTutor, int es_menor)
{
    if(es_menor)
    {
        if(emailTutor[0] == '\0')
        {
            printf("\nError: Los menores de edad deben tener un email de tutor.\n");
            return 0;
        }
        
        if(!email_valido(emailTutor))
        {
            printf("\nError: Formato de email inválido.\n");
            return 0;
        }
    }
    else // es adulto
    {
        if(emailTutor[0] != '\0')
        {
            printf("\nError: Los adultos no deben tener email de tutor.\n");
            return 0;
        }
    }
    
    return 1;
}

int email_valido(const char *email)
{
    // 1. Buscar el '@'
    const char *at = strchr(email, '@');
    if (at == NULL || at == email) // No hay '@' o empieza con '@'
        return 0;

    // 2. Buscar un '.' DESPUES del '@'
    const char *dot = strchr(at + 1, '.');
    if (dot == NULL || dot == at + 1) // No hay '.' o esta justo despues de '@'
        return 0;

    // 3. Asegurarse que no termina en '.'
    if (dot[1] == '\0')
        return 0;

    return 1;
}




