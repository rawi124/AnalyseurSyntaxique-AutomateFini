/*
LA GRAMMAIRE :
	EXPR--> Terme ResteE
	ResteE --> +Terme ResteE |epsilon
	Terme --> Facteur ResteT
	ResteT -->.Facteur ResteT | epsilon
	Facteur --> lettre | (EXPR) |Facteur*
*/
/***************************************DEBUT DU PROGRAMME*******************************************************************/
#include "compregex.h"
//*************declaration des variables globales****************//

/*variable globale pour la fonction Parser*/
int i ;//indexation de la liste d unite lexicale
int j ;//indexation de la liste postfixe
       // i et j doivent etre different en raison de l ecriture postfixe
       // en effet un symbole a la position x dans la chaine a traiter
       // n a pas forcement la position x dans la nouvelle liste postfixe

char*postfixe ;//liste qui va contenir l ecriture postfixe
char*expr ;    //liste d unite lexicale
int n ;       //le nombre d unite lexicale



/*
	nb_element est une fonction qui calcule le nombre
	de symbole dans la chaine sans considerer les espace
	et ceci pour pouvoir creer une liste postfixe
	de taille adapte au nombre reel d elemets
*/
int nb_element(char*s)
{
  int len = 0 ;
  for(int i = 0 ; i < strlen(s) ; i++)
  {
    if (s[i] != ' ')
      len++;
  }
  return len ;
}

/*
 une fonction qui prendra en entrée un chaîne de caractères et renverra une liste d'unités lexicales
*/
char* Scanner(char*s)  //l'analyseur lexical
{
  int len = nb_element(s); ;
  char*tokens = malloc(len*sizeof(char));
  int i = 0 ;
  for(int j = 0 ; j < strlen(s) ; j++)
  {
      //ignorer les espaces
      if (s[j] != ' ')
      {
        //les caracteres pris en consideration
      	if((s[j] == '*')||(s[j]=='+')||(s[j]=='.')||(s[j]=='(')||(s[j]==')')||(isalpha(s[j])|| (isdigit(s[j]))))
      	{
      		tokens[i] = s[j];
      		i++ ;
      	}
      else
      {
      	printf("    erreur pres du caractere %d , verifiez votre expression  \n",j);
      	return  s;
      }
     }
  }
  return tokens; // liste d'unité lexical
}

int Expr()
{
	if (Terme() && ResteE() )
		return 1;
	else
		return 0;
}

int ResteT()
{
	if ( n > i )
	{

		if (expr[i] == '.')
		{
			char x = expr[i] ;
			i = i + 1;
			if (Facteur())
			{
				postfixe[j] = x ;
				j++;
				if (ResteT())
				{
					return 1;
				}
			}
		}
		else
			return 1;
	}
	else
		return 1;
}

int Terme()
{
	return Facteur() && ResteT() ;
}

int Facteur()
{
	if (i < n )
	{
		if ( isalpha(expr[i]) || (expr[i]>=48 && expr[i] <=57))
		{
			postfixe[j]= expr[i];
			j++;
			i = i + 1;
			if(expr[i] == '*')
			{
				postfixe[j] = expr[i];
				j++;
				i++;
			}
			return 1;
		}
		if (expr[i] == '(')
		{
			i = i + 1;
			Expr();
			if (expr[i] == ')')
			{	i = i + 1;
				if(expr[i] == '*')
				{
					postfixe[j] = expr[i];
					j++;
					i++;
					//return 1 ;
				}
				return 1 ;
			}
		}

	}
	else
	{
		printf("erreur caractere %d\n",i);
		return 0;
	}
}

int ResteE()
{
	if ( n > i )
	{
		if (expr[i] == '+')
		{
			char y = expr[i];
			i = i + 1;

			if (Terme())
			{
				postfixe[j] = y ;
				j++;
				if (ResteE())
					return 1;
			}
		}
		else
			return 1;
	}
	else
		return 1;
}


/*
parser prend en entree une liste "d unites lexicales" et le nombre d'unites lexicales
*/
char* parser(char *l, int t)
{
	i = 0 ;
	j = 0;
	expr = l ;
	n = t ;
	postfixe = malloc(n*sizeof(char));
	if (Expr())
		return postfixe;//postfixe est la sortie de notre analyseur syntaxique
	else
		return 0 ;
}

/*
 la fonction afficher ne fait pas grande chose
 elle a un interet esthetique
 qui est l affichage des caracteres de l expression separes par des espaces
*/
void afficher(char*s)
{
  for(int i = 0 ; i<strlen(s) ; i++)
    printf("%c ",s[i]);
  printf("\n");
}


/*
 verifier si la chaine est accepee ou pas  par l expression reguliere passe en parametres
*/
void chaine_accepte(char*expressionReguliere , char*chaine)
{
  int len = nb_element(expressionReguliere);//nombre d element de l expression
                                    //len retourne le nombre de char d une chaine sans compter les espaces

  char*tok = Scanner(expressionReguliere); //analyse lexicale de l expression reguliere
  char*post = parser(tok,len);//ecriture postfixe de l expression

  printf("********************   expression reguliere ********************************\n                   ");
  afficher(tok);

  printf("=>son ecriture postfixe est :\n");
  afficher(post);

  afn  pile[len] ;
  int sommet = 0 ;//pour parcourir la pile
  for(int j = 0 ; j < len ; j++)
  {
    if (isalpha(post[j]) ||(post[j]>=48 && post[j] <=57)) //l automate qui acceptant le langage constitué dun seul symbole.
    {
          afn_char(&pile[sommet],post[j]);
          sommet++;
    }
    else if (post[j] == '+') // l union de 2 automates
    {
         afn  tmp ;
      	 afn_union(&tmp,pile[sommet-1],pile[sommet-2]);
         pile[sommet-2] = tmp ;
         sommet = sommet - 1 ;
    }
    else if (post[j] == '.') //la concatenation de 2 automates
    {
          afn  tmp ;
          afn_concat(&tmp,pile[sommet-2],pile[sommet-1]);
          pile[sommet-2] = tmp ;
          sommet = sommet - 1 ;
    }
    else if (post[j] == '*') //l etoile de Kleene
    {
        afn  tmp ;
      	afn_kleene(&tmp,pile[sommet-1]);
     	pile[sommet-1] = tmp ;
    }
  }
  sommet--;
  afd A ;
  afn_determinisation(pile[sommet],&A);
  int accepte = afd_simul(chaine,A);
  if (accepte)
  	printf("==>la chaine %s est accepte par l expression reguliere decrite au-dessus \n\n",chaine );
  else
  	printf("==>la chaine %s n'est pas accepté par lexpression reguliere decrite au-dessus !!!!\n\n",chaine);
  free(post);
  free(tok);
  afd_free(&A);
  afn_free(&pile[sommet]);
  //printf("********************************************************************************\n");

}
