
#include "headers.h"

long validarDNI(long li, long ls)
{
    long dni;
    do
    {
        scanf("%ld",&dni);
        if (dni < li || dni > ls)
            puts("DNI INVALIDO, ingrese nuevamente: ");
        
    } while (dni < li || dni > ls);
    
    return dni;
}

char ingresoYvalidaOpcion(char li, char ls)
{
    char op;
    puts("\nIngrese opcion (de A a G): ");
    do
    {   
        fflush(stdin);
        scanf("%c",&op);
        op = toupper(op);
        if(op < li || op > ls)
            puts("OPCION INVALIDA, ingrese nuevamente: ");  
        
    } while (op < li || op > ls);

    return op;
}

bool esBisiesto(int anio) 
{
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

bool validarFecha(t_Fecha fecha) 
{
    int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (fecha.anio < 1900 || fecha.anio > 2100) 
        return false;

    if (fecha.mes < 1 || fecha.mes > 12) 
        return false;

    if (esBisiesto(fecha.anio))
        diasPorMes[2] = 29;


    if (fecha.dia < 1 || fecha.dia > diasPorMes[fecha.mes]) 
        return false;

    return true;
}

bool validarFechaNacimiento(t_Fecha fechaNacimiento, t_Fecha fechaProceso)
{
    if (!validarFecha(fechaNacimiento))
        return false;
    

    t_Fecha fechaLimite = fechaProceso;
    fechaLimite.anio -= 10;

    if (fechaNacimiento.anio > fechaLimite.anio ||(fechaNacimiento.anio == fechaLimite.anio && fechaNacimiento.mes > fechaLimite.mes) ||
    (fechaNacimiento.anio == fechaLimite.anio && fechaNacimiento.mes == fechaLimite.mes && fechaNacimiento.dia >= fechaLimite.dia))
        return false;
    
    
    return true;
}

bool validarFechaAfiliacion(t_Fecha fechaAfiliacion, t_Fecha fechaProceso, t_Fecha fechaNacimiento) 
{
    if (!validarFecha(fechaAfiliacion)) 
        return false;
    
    
    if (fechaAfiliacion.anio > fechaProceso.anio ||(fechaAfiliacion.anio == fechaProceso.anio && fechaAfiliacion.mes > fechaProceso.mes) ||
    (fechaAfiliacion.anio == fechaProceso.anio && fechaAfiliacion.mes == fechaProceso.mes && fechaAfiliacion.dia > fechaProceso.dia)) 
        return false;
    

    if (fechaAfiliacion.anio < fechaNacimiento.anio ||(fechaAfiliacion.anio == fechaNacimiento.anio && fechaAfiliacion.mes < fechaNacimiento.mes) ||
    (fechaAfiliacion.anio == fechaNacimiento.anio && fechaAfiliacion.mes == fechaNacimiento.mes && fechaAfiliacion.dia <= fechaNacimiento.dia)) 
        return false;
    
    
    return true;
}

bool validarFechaUltimaCuota(t_Fecha fechaCuota, t_Fecha fechaAfiliacion, t_Fecha fechaProceso) 
{
    if (!validarFecha(fechaCuota)) 
        return false;
    

    if (fechaCuota.anio < fechaAfiliacion.anio ||(fechaCuota.anio == fechaAfiliacion.anio && fechaCuota.mes < fechaAfiliacion.mes) ||
    (fechaCuota.anio == fechaAfiliacion.anio && fechaCuota.mes == fechaAfiliacion.mes && fechaCuota.dia <= fechaAfiliacion.dia)) 
        return false;
    

    if (fechaCuota.anio > fechaProceso.anio ||(fechaCuota.anio == fechaProceso.anio && fechaCuota.mes > fechaProceso.mes) ||
    (fechaCuota.anio == fechaProceso.anio && fechaCuota.mes == fechaProceso.mes && fechaCuota.dia > fechaProceso.dia)) 
        return false;
    
    
    return true;
}

bool validarEmail(char email[]) 
{
    int i, at_pos = -1, dot_pos = -1, len = strlen(email);

    for (i = 0; i < len; i++) 
    {
        if (email[i] == '@') 
            at_pos = i;
        else if (email[i] == '.') 
            dot_pos = i;
        
    }

    if (at_pos == -1 || dot_pos == -1) 
    return false;
    if (at_pos == 0 || at_pos > dot_pos || dot_pos == len - 1) 
    return false;

    return true;
}

char* validarCategoria(t_Fecha fechaNacimiento , t_Fecha fechaProceso)
{
    int edadSocio = 0;

    edadSocio = fechaProceso.anio - fechaNacimiento.anio;

    if(fechaProceso.mes < fechaNacimiento.mes)
        edadSocio --;
    else if (fechaProceso.mes == fechaNacimiento.mes && fechaProceso.dia < fechaNacimiento.dia)
        edadSocio --;


    return edadSocio >= 18 ? "ADULTO":"MENOR" ;
}

char* validarPlan()
{
    char plan;
    puts("PLANES DISPONIBLES: 'BASIC','PREMIUM','VIP','FAMILY'");
    puts("INGRESE PLAN A ASIGNAR (B,P,V o F): ");
    
    do
    {
        scanf(" %c",&plan);
        plan = toupper(plan);
        if (plan != 'B' && plan != 'P' && plan != 'V' && plan != 'F')
            puts("OPCION INVALIDA, ingrese nuevamente: ");
        
    } while (plan != 'B' && plan != 'P' && plan != 'V' && plan != 'F');
    
    return  plan == 'B'? "BASIC": 
            plan == 'P'? "PREMIUM":
            plan == 'V'? "VIP":"FAMILY";
}






