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
                primera_palabra_procesada = 1; // Marcar que ya pasó la primera
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
        return "Fecha Nacimiento: Fecha invalida";

    edad = calcular_edad(&miembro->fecha_nac, fecha_proceso);

    if (edad < 10)
        return "Fecha Nacimiento: El miembro debe tener al menos 10 anios";

    if (miembro->sexo != 'F' && miembro->sexo != 'M')
        return "Sexo: Valor invalido (debe ser 'F' o 'M')";

    if (!esFechaValida(&miembro->fecha_afi))
        return "Fecha Afiliacion: Fecha inválida";

    if (cmp_fechas(&miembro->fecha_afi, fecha_proceso) > 0)
        return "Fecha Afiliacion: No puede ser posterior a la fecha de proceso";

    if (cmp_fechas(&miembro->fecha_afi, &miembro->fecha_nac) <= 0)
        return "Fecha Afiliacion: Debe ser posterior a la fecha de nacimiento";

    if (edad < 18)
    {
        if (strcmp(miembro->categoria, "MENOR") != 0)
            return "Categoría: Inconsistente (debería ser MENOR)";
    }
    else
    {
        if (strcmp(miembro->categoria, "ADULTO") != 0)
            return "Categoría: Inconsistente (debería ser ADULTO)";
    }

    if (!esFechaValida(&miembro->fecha_ult_cuo))
        return "Fecha Última Cuota: Fecha inválida";

    if (cmp_fechas(&miembro->fecha_ult_cuo, &miembro->fecha_afi) <= 0)
        return "Fecha Última Cuota: Debe ser posterior a la fecha de afiliación";

    if (cmp_fechas(&miembro->fecha_ult_cuo, fecha_proceso) > 0)
        return "Fecha Última Cuota: No puede ser posterior a la fecha de proceso";

    if (miembro->estado != 'A')
        return "Estado: Los registros nuevos deben crearse con estado 'A'";

    if (strcmp(miembro->plan, "BASIC") != 0 &&
        strcmp(miembro->plan, "PREMIUM") != 0 &&
        strcmp(miembro->plan, "VIP") != 0 &&
        strcmp(miembro->plan, "FAMILY") != 0)
    {
        return "Plan: Valor invalido (debe ser BASIC, PREMIUM, VIP o FAMILY)";
    }

    if (edad < 18)
    {
        if (miembro->emailTutor[0] == '\0')
            return "Email Tutor: Menor de edad debe tener un email de tutor";
        if (!email_valido(miembro->emailTutor))
            return "Email Tutor: Formato de email invalido";
    }
    else
    {
        if (miembro->emailTutor[0] != '\0' && !email_valido(miembro->emailTutor))
            return "Email Tutor: Formato de email invalido";
    }

    return NULL;
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
