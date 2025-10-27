#include "fecha.h"

void ingresarFechaValida(t_fecha *fecha)
{
    puts("Ingrese una fecha valida bajo el formato dd/mm/aaaa:");
    scanf("%d/%d/%d", &fecha->dia, &fecha->mes, &fecha->anio);

    while (!esFechaValida(fecha))
    {
        puts("Fecha invalida, ingrese nuevamente (dd/mm/aaaa):");
        scanf("%d/%d/%d", &fecha->dia, &fecha->mes, &fecha->anio);
    }

    puts("Fecha ingresada correctamente.\n");
}

int esFechaValida(const t_fecha *fecha)
{
    if (fecha->mes >= 1 && fecha->mes <= 12)
    {
        if (fecha->anio >= 1601 && fecha->anio <= 9999)
        {
            if (fecha->dia > 0 && fecha->dia <= cantDiasMes(fecha->anio, fecha->mes))
                return 1;
        }
    }
    return 0;
}

int cantDiasMes(int anio, int mes)
{
    int dias[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2 && esBisiesto(anio))
        return dias[mes] + 1;

    return dias[mes];
}

int esBisiesto(int anio)
{
    if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0))
        return 1;

    return 0;
}

int cmp_fechas(const t_fecha *f1, const t_fecha *f2)
{
    long f1_num = f1->anio * 10000L + f1->mes * 100 + f1->dia;
    long f2_num = f2->anio * 10000L + f2->mes * 100 + f2->dia;

    if (f1_num < f2_num)
        return -1;
    if (f1_num > f2_num)
        return 1;

    return 0;
}

int calcular_edad(const t_fecha *fecha_nac, const t_fecha *fecha_proceso)
{
    int edad = fecha_proceso->anio - fecha_nac->anio;

    if (fecha_proceso->mes < fecha_nac->mes ||
        (fecha_proceso->mes == fecha_nac->mes && fecha_proceso->dia < fecha_nac->dia))
    {
        edad--;
    }

    return edad;
}

void restar_un_dia(t_fecha *fecha)
{
    fecha->dia--;

    if (fecha->dia == 0)
    {
        fecha->mes--;

        if (fecha->mes == 0)
        {
            fecha->mes = 12;
            fecha->anio--;
        }

        fecha->dia = cantDiasMes(fecha->anio, fecha->mes);
    }
}
