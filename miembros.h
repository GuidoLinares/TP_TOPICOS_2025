#ifndef MIEMBROS_H_INCLUDED
#define MIEMBROS_H_INCLUDED

#include "fecha.h"

typedef struct {
    long dni;
    char apeynom[60];
    t_fecha fecha_nac;
    char sexo;
    t_fecha fecha_afi;
    char categoria[10];
    t_fecha fecha_ult_cuo;
    char estado;
    char plan[10];
    char emailTutor[30];
} t_miembro;

#endif // MIEMBROS_H_INCLUDED
