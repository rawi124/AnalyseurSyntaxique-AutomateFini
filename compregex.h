#ifndef COMPREGEX_H
#define COMPREGEX_H

#include "afn.h"
#include "afd.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>

#include "afn.h"
#include "afd.h"

int nb_element(char*s);
char* Scanner(char*s);
int ResteE();
int Facteur();
int Terme();
int ResteT();
int Expr();
char* parser(char *l, int t);
void afficher(char*s);
void chaine_accepte(char*expressionReguliere , char*chaine);

#endif 
