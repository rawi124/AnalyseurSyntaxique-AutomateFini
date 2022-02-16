#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "afn.h"
#include "afd.h"
#include "compregex.h"

int main(int argc, char* argv[])
{
  char * a = malloc(12*sizeof(char));
  char * b =malloc(16*sizeof(char));
  char * c =malloc(19*sizeof(char)); 
  char * d =malloc(13*sizeof(char));
  char * e =malloc(44*sizeof(char)); 
  char * f =malloc(23*sizeof(char)); 
  char * g = malloc(5*sizeof(char));
  a ="a*+c*+t*+k*";  
  b = "a+b*+a.k.s.n.b*";
  c ="(a+b)*+c*+e.f.x.n*";
  d = "H.E.L.L.O+h.i";
  e = "T.P+A.U.T.O.M.A.T.E+E.X.P.R.E.S.S.I.O.N";
  f = "a+b.c+k.n*+(a.b+c+d).n*";
  g = "1.2.3.4";
  chaine_accepte(a , "aaaaa");
  chaine_accepte(c , "efn");
  chaine_accepte(c , "efxnnnnnnnnnnnnnnnnnnnnnnn");
  chaine_accepte(f , "abcn");
  chaine_accepte(f , "bc");
  chaine_accepte(b , "aksn");
  chaine_accepte(d , "hii");
  chaine_accepte(d , "HELLO");
  chaine_accepte(e , "EXPRESSION");
  chaine_accepte(e , "LANGAGE");
  chaine_accepte(g , "1234");
  chaine_accepte(g , "678");
  
  
  
  printf("\n\n*************************************************************************************************************");
  printf("\n\n*************************************************************************************************************\n");
  printf("si vous voulez testez d autres exemples decommenter la ligne dans le fichier mygrep.c et commenter les tests\n");
  printf("*****************************************************************************************************************\n");
  printf("******************************************************************************************************************\n");
  
  //chaine_accepte(argv[1],argv[2]);
  

  return 0 ;
}
