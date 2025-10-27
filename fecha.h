#ifndef FECHA_H_INCLUDED
#define FECHA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int dia, mes, anio;
}t_fecha;
/*
void ingresarFechaValida (t_fecha *fecha);
int esFechaValida (const t_fecha *fecha);
int cantDiasMes (const int *anio, const int *mes);
int esBisiesto (const int *anio);
int cmp_fechas(const t_fecha *f1, const t_fecha *f2);
int calcular_edad(const t_fecha *fecha_nac, const t_fecha *fecha_proceso);
void restar_un_dia(t_fecha *fecha);
*/


void ingresarFechaValida(t_fecha *fecha);
int esFechaValida(const t_fecha *fecha);
int cantDiasMes(int anio, int mes);
int esBisiesto(int anio);
int cmp_fechas(const t_fecha *f1, const t_fecha *f2);
int calcular_edad(const t_fecha *fecha_nac, const t_fecha *fecha_proceso);
void restar_un_dia(t_fecha *fecha);


#endif // FECHA_H_INCLUDED
