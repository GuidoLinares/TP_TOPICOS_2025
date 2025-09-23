#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define tam 20
#define arch_miembros_txt "miembros-VC.txt"

typedef struct 
{
    int dia;
    int mes;
    int anio;
}t_Fecha;

typedef struct 
{
    long DNI; //(entre 1000000  y 10000000)
    char apellidoYnombre[60];
    t_Fecha fecha_nacimiento; //Validación formal y < fecha de proceso – 10 años
    char sexo;//‘F’ (Femenino), ‘M’(Masculino).
    t_Fecha fecha_afiliacion; //Validación formal, <= fecha de proceso y > fecha nacimiento
    char categoria[10]; // ‘MENOR’, menor de 18 años ‘ADULTO’, mayor de 18 años.
    t_Fecha fecha_ultima_cuota_paga; //> fecha de afiliación y <=fecha de proceso.
    char estado; //‘A’ Alta, ‘B’ Baja - Se crean con'A'.
    char plan[10]; //'BASIC', 'PREMIUM', 'VIP', 'FAMILY'
    char email_tutor[30]; //Si el miembro es menor de edad, deberá tener un tutor asociado. pepe@galleta.com Se deberá validar el formato del email.
}s_miembros;

t_Fecha fechaProcesoGlobal;

///////////// VALIDACIONES ///////////// 
long validarDNI(long,long, long);
char validarChar();
char ingresoYvalidaOpcion(char,char);
bool validarFecha(t_Fecha fecha);
bool validarFechaNacimiento(t_Fecha fechaNacimiento, t_Fecha fechaProceso);
bool validarFechaAfiliacion(t_Fecha fechaAfiliacion, t_Fecha fechaProceso, t_Fecha fechaNacimiento);
bool validarFechaUltimaCuota(t_Fecha fechaCuota, t_Fecha fechaAfiliacion, t_Fecha fechaProceso);
bool validarEmail(char email[]);
char* validarCategoria(t_Fecha fechaNacimiento , t_Fecha fechaProceso);
char* validarPlan();
char validarOp(char li, char ls);
bool esPlanValido(const char* plan);
bool esMenorDeEdad(t_Fecha fechaNac, t_Fecha fechaProc);

///////////// MENU ///////////// 
void mostrarMenu();

///////////// MANEJO DE TEXTO ///////////// 
void normalizarCadena(char*);
bool parsearLineas(s_miembros *miembroTemp, char* buffer);

///////////// FECHAS ///////////// 
void ingresarFecha();
bool esBisiesto(int anio);

///////////// ARCHIVOS ///////////// 
bool leerYGenerarArchivo();
bool validarRegistros(const s_miembros* pMiembro, const t_Fecha* pFechaProceso, char* motivoError);
int* cmp(const void *dato1, const void *dato2);




#endif