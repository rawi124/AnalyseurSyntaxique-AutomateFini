#ifndef AFN_H
#define AFN_H

#include <stdlib.h>
#include <stdio.h>
#include "afd.h"


#define INT_ETAT(q) (1ULL<< (q))   //int_etat(q)=> elle prend un etet (nb entre 0 et 63) et envoie l entier de 64 bit avec le q'eme bit a 1
#define IN(q,X) ((INT_ETAT(q) & X ) > 0) //pour savoir si q appartient à un etat final

typedef unsigned int uint;
typedef unsigned long long int ullong;

typedef struct
{
  uint nbetat, nbsymb;
  char * alphabet;
  uchar tsymb[SYMB_NB_MAX];
  ullong init, finals;
  ullong **delta;
} afn;


void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals);
void afn_add_trans(afn *A, uint q1, uint s, uint q2);
void afn_free(afn *A);

void afn_print(afn A);
void afn_finit(char *f, afn *A);

ullong afn_epsilon_fermeture(afn A, ullong R);
void afn_determinisation(afn A, afd *D);
void afn_char(afn *C, char c);
void afn_union(afn *C, afn A, afn B);
void afn_concat(afn *C, afn A, afn B);
void afn_kleene(afn *C, afn A);


#endif
