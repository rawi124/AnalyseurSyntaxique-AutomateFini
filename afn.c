#include "afn.h"


char alphab[64]="&abcdefghijklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTVUWXYZ0123456789";

void afn_init(afn *A, uint nbetat, char * alphabet, ullong init, ullong finals)
{
  A->nbetat = nbetat;

  A->alphabet = malloc(strlen(alphabet)+1);
  strcpy(A->alphabet, alphabet);
  A->nbsymb = strlen(alphabet);

  A->init = init;
  A->finals = finals;

  int i;
  uchar symb;

  for (i=0; i<SYMB_NB_MAX; i++)
    A->tsymb[i] = SYMB_NONE;

  for (i=0; i<A->nbsymb; i++){
    if ( (alphabet[i] < SYMB_ASCII_DEB) ||
	 (alphabet[i] > SYMB_ASCII_FIN) ){
      fprintf(stderr,"[afd_init] caractere ascii numero %d invalide\n", alphabet[i]);
      exit(-1);
    }
    symb = (uchar) (alphabet[i] - SYMB_ASCII_DEB);
    if ( A->tsymb[symb] != SYMB_NONE ){
      //fprintf(stderr,"[afd_init] caractere <%c> deja defini (ignorer)\n",alphabet[i]);
    }
    else {
      A->tsymb[symb] = i;
    }
  }
  A->delta = calloc(nbetat, sizeof(ullong *));
  for (i=0; i<nbetat; i++){
    A->delta[i] = calloc(A->nbsymb, sizeof(ullong));
  }
}

  /*
  Ajoute la relation (<q1>, <s>, <q2>) a l'AFN <A>
*/
void afn_add_trans(afn *A, uint q1, uint s, uint q2)
{
  uchar symb = A->tsymb[s-SYMB_ASCII_DEB];

  if (symb == SYMB_NONE){
    fprintf(stderr, "[add_trans] %u -- %c --> %u symbole inconnu\n", q1,s,q2);
    exit(-1);
  }

  if ( (q1<0) || (q1>=A->nbetat) ){
   fprintf(stderr, "[add_trans] etat <%d> non reconnu\n", q1);
    exit(-1);
  }
  if ( (q2<0) || (q2>=A->nbetat) ){
   fprintf(stderr, "[add_trans] etat <%d> non reconnu\n", q2);
    exit(-1);
  }

  A->delta[q1][symb] |= INT_ETAT(q2);
}


/*
  Libere la memoire de l'AFN <A>
*/
void afn_free(afn *A)
{
  free(A->alphabet);

  int i;
  for (i=0; i < A->nbetat; i++)
    free(A->delta[i]);
  free(A->delta);
}


/*
  Affiche l'AFN <A>
*/
void afn_print(afn A){
  uint q,q1,s;
  printf("init:");
  for (q=0; q<A.nbetat; q++)
    {
      if IN(q,A.init)
	     printf(" %d",q);
    }
  printf("\n");
  printf("finals:");
  for (q=0; q<A.nbetat; q++)
    {
      if IN(q,A.finals)
	     printf(" %d",q);
    }
  printf("\n");
  for (q=0; q<A.nbetat; q++){
    for (s=0; s<A.nbsymb; s++){
      if (A.delta[q][s]!=0){
	printf("%d -- %c --> {", q, A.alphabet[s]);
	for (q1=0; q1<A.nbetat; q1++)
	  {
	    if (IN(q1, A.delta[q][s]))
	      printf("%d,",q1);
	  }
	printf("\b}\n");
      }
    }
  }
}
/*
  Initialise l'AFN <A> a partir des donnees du fichier <nomfichier> au
  format:

  nb_etats nb_symboles
  etat_initial
  nb_etats_finals
  etat_final_1 etat_final_2 ... etat_final_n
  etat_i_11 symb_j_1 etat_i_12
  etat_i_21 symb_j_2 etat_i_22
  .
  .
  .
  etat_i_n1 symb_j_n etat_i_n2

*/
void afn_finit(char *nomfichier, afn *A){

    FILE *fd;
    fd=fopen(nomfichier,"r");
    uint nbetat;
    ullong init,finals;
    char alphabet[SYMB_NB_MAX+1];

    fscanf(fd,"%u %s", &nbetat, alphabet);
    fscanf(fd,"%llu %llu",&init, &finals);

    afn_init(A,nbetat,alphabet,init,finals);

    uint q1,q2;
    char s;
    while (fscanf(fd,"%u %c %u",&q1,&s,&q2)==3){
        afn_add_trans(A,q1,s,q2);
    }
    fclose(fd);

}
/*
  calculer la fermeture epsilon de l'automate passé en paramètre
*/
//les deux fonctions decrite ici epsilon-fermeture et determinisation sont les fonctions d ALEX et DANNY
ullong afn_epsilon_fermeture(afn A, ullong R)  //Retourne l'epsilon fermeture de l'ensemble d'états <R> par l'automate <A>
{
	ullong ferm = R;
	uint q1, q2;
	uint epsilon = A.tsymb['&' - SYMB_ASCII_DEB];

	int sommet_pile = -1;
	uint pile[64];

	for (q1 = 0; q1 < A.nbetat && INT_ETAT(q1) <= R; q1++)
		if (IN(q1, R)) //"IN" pour savoir si q1 appartient à un etat final
			pile[++sommet_pile] = q1;

	while (sommet_pile >= 0) {
		q1 = pile[sommet_pile--];
		for (q2 = 0; q2 < A.nbetat; q2++)
			if ((ferm & (INT_ETAT(q2))) == 0)
				if (IN(q2, A.delta[q1][epsilon])) {
					ferm |= INT_ETAT(q2);   //INT_ETAT"=>transforme un nombre de 64bit avec le bit de numero presenté a 1
					pile[++sommet_pile] = q2;
				}
	}

	return ferm;
}

/*
  Calcule un automate deterministe equivalent à <A> et affecte le
  resultat a <D>. Les etats de l'afd sont renumerotés à partir de 0
*/
//algorithme DETERMINISATION n'est pas personnel//

void afn_determinisation(afn A, afd *D) {
	uint i, j, k, q_i;

	// On va supposer que A.alphabet == D.alphabet (avec epsilon inclus)
	uint taille_alphabet = strlen(A.alphabet);

	uint sigma;
	ullong ferm;
	uint indice_ferm_dans_etats;

	// Préparation et initialisation des informations pour la liste d'états et de transitions
	uint nb_lignes_init = 1024;
	uint increment_lignes = 512;
	uint taille_alouee = nb_lignes_init;
	uint nb_etats = 0;

	ullong** lignes_etats = malloc(sizeof(ullong*) * taille_alouee);
	ullong** tmp; // utilisé pour réallouer lignes_etats lorsque lignes_etats est plein
	lignes_etats[nb_etats++] = malloc(sizeof(ullong) * (1 + taille_alphabet));
	lignes_etats[0][0] = afn_epsilon_fermeture(A, A.init);

	// Calcul des nouveaux états et des transitions pour chaque ligne de lignes_etats
	for (i = 0; i < nb_etats; i++)
		for (j = 0; j < taille_alphabet; j++)
			if (A.alphabet[j] != '&') { // Nous ignorons les transitions epsilon
				// Calcul de la fermeture pour l'état courant
				ferm = 0;
				sigma = A.tsymb[A.alphabet[j] - SYMB_ASCII_DEB];
				for (q_i = 0; q_i < A.nbetat; q_i++)
					if (IN(q_i, lignes_etats[i][0]))
						ferm |= afn_epsilon_fermeture(A, A.delta[q_i][sigma]);

				// Vérification de la présence de ferm dans la liste d'états existants
				indice_ferm_dans_etats = -1;
				for (k = 0; k < nb_etats; k++)
					if (ferm == lignes_etats[k][0]) {
						indice_ferm_dans_etats = k;
						break;
					}

				if (indice_ferm_dans_etats == -1) {
					// Extension de la liste si nécessaire
					if (nb_etats == taille_alouee) {
						tmp = lignes_etats;
						taille_alouee += increment_lignes;
						lignes_etats = malloc(sizeof(ullong*) * taille_alouee);
						for (k = 0; k < taille_alouee - increment_lignes; k++)
							lignes_etats[k] = tmp[k];
						free(tmp);
					}

					// Ajout d'un élément à la liste
					lignes_etats[nb_etats++] = malloc(sizeof(ullong) * (1 + taille_alphabet));
					lignes_etats[nb_etats - 1][0] = ferm;
					indice_ferm_dans_etats = nb_etats - 1;
				}

				// Stockage de la transition beta(lignes_etats[i][0], A.alphabet[j]) = ferm
				lignes_etats[i][j + 1] = indice_ferm_dans_etats; // Correspond à la transition
			}

	// Construction de l'ensemble des états finals
	uint nb_finals = 0;
	for (i = 0; i < nb_etats; i++)
		if (lignes_etats[i][0] & A.finals)
			nb_finals++;

	uint* finals = malloc(sizeof(uint) * nb_finals);

	j = 0;
	for (i = 0; i < nb_etats; i++)
		if (lignes_etats[i][0] & A.finals)
			finals[j++] = i;

	// Initialisation de l'AFD
	afd_init(D, nb_etats, A.alphabet, nb_finals, 0, finals);
	// Ajout des transitions dans l'AFD
	for (i = 0; i < nb_etats; i++)
		for (j = 0; j < taille_alphabet; j++)
			if (A.alphabet[j] != '&')
				afd_add_trans(D, i, A.alphabet[j], lignes_etats[i][j + 1]);

	// Libération des éléments alloués
	for (i = 0; i < nb_etats; i++)
		free(lignes_etats[i]);
	free(lignes_etats);
	free(finals);
}

 /*
  contruire un AFN qui accepte le langage constitué d'un seul symbole c
*/
void afn_char(afn *C, char c)
{
    afn_init(C,2,alphab,1ULL,2ULL);//nombre d etats egale a 2 : etat initial et un etat final
    afn_add_trans(C,0,c,1);//mettre la seule transition existante entre etat intial et etat final 
}

/*
  Calcule un automate qui reconnait l'union de <A> et <B>
*/
void afn_union(afn *C, afn A, afn B){

    int nbC = A.nbetat + B.nbetat+1;//le nombre d etat de C est egale au nombre d etat de A et B +1 (+1 pour le nouvel etat 0)

    ullong init = 1ULL;   //on ajoute un nouvel etat initial 0 global pour les deux automates
    ullong fA = A.finals<<1 ;//les etats finals de l'automate A sont decale d un pas vers l avant a cause du nouvel etat initial de C
    ullong fB = ((B.finals)<<(A.nbetat+1));//les etats finals de B decale du nombre d etats de A + 1 pour le nouvel etat initial
    ullong final = fA | fB; // les etats finals de C sont ceux de A et B

    afn_init(C,nbC,alphab,init,final);//creer le nouvel automate

    afn_add_trans(C,0,'&',1); //epsilon transition qui relie l etat initial de C et l'automate A
    afn_add_trans(C,0,'&',A.nbetat+1); //epsilon transition qui relie l etat initial de C et l'automate B

    //les etats de A sont decale de 1 et les etats de B sont decale de nA+1 (nA nombre d'etats de A)
    int tmp ;
      for(int i = 0 ; i < A.nbetat ; i++)
      {  //parcourir l'ensemble des etats de A
        for(int j = 0 ; j < A.nbsymb ;j++)
        {//on decale tous les elements de A d un pas
          tmp = A.delta[i][j]<<1;
          C->delta[i+1][j] = tmp;
        }
      }
      for(int i = 0;  i < B.nbetat;i++)
      {//parcourir l'ensemble des etats de B
         for(int j=0;j < B.nbsymb;j++)
         {//on decale tous les elements de B de nombres d etats de A +1
           tmp = B.delta[i][j]<<(A.nbetat+1);
           C->delta[i+A.nbetat+1][j]=tmp;
         }
      }
}

/*
  Calcule un automate qui reconnait la concatenation de <A> et <B>
*/

void afn_concat(afn *C, afn A, afn B)
{
  //initialisation de l etat initial , finals et le nombre d etat de C
	ullong initC = A.init ;// l etat initial de C est egale a celui de A
	int nbC = A.nbetat+B.nbetat; //le nombre d etats est egale a la somme des deux nombres d etats des deux automates
	ullong fC = B.finals<<A.nbetat;//les etats finals de C sont ceux de l automate B mais decale du nombre d automate de A

  //creer le nouvel automate C
	afn_init(C,nbC,alphab,initC,fC);

  int tmp ;
  //ajouter les transitions de l automates A au nouvel automate
  for(int i = 0 ; i < A.nbetat ; i++)
  {
    for(int j = 0 ; j < A.nbsymb ;j++)
    {
      tmp = A.delta[i][j];
      C->delta[i][j] = tmp;
    }
  }

 //ajouter les transitions de l automate B au nouvel automate avec un decalage de nombre d etats de A
  for(int i = 0;  i < B.nbetat;i++)
  {
    for(int j=0;j < B.nbsymb;j++)
    {
      tmp = B.delta[i][j]<<A.nbetat;
      C->delta[i+A.nbetat][j]=tmp;
    }
  }
  //ajouter epsilon transition entre etats finals de A et etat initial de B
  for(int i = 0 ; i < A.nbetat ; i++)
  {
    if(IN(i,A.finals))
    { // si i est un etat final de A
      afn_add_trans(C,i,'&',A.nbetat);
    }
  }

}

/*
  Calcule un automate qui reconnait la fermeture de Kleene de <A>
*/
void afn_kleene(afn *C, afn A)
{
  int nbetat= A.nbetat+1; //nombre d etat de A + 1 etat pour (0,'&',1)
  ullong init = 1ULL;
  ullong finals = ((A.finals)<<1) | 1ULL; //etats finals finals de A decale de 1 et l etat initial de C
  afn_init(C,nbetat,alphab,init,finals); //creation de l automate C
  afn_add_trans(C,0,'&',1); // ajouter epsilon transition entre l etat initial de C et l automate A

  for(int i = 0; i < A.nbetat;i++)
  {
    for(int j=0;j < A.nbsymb;j++)
      C->delta[i+1][j] = A.delta[i][j]<<1; //decaler les etats de A de 1
  }

  //cette boucle sert a ajouter des &-transitions entre les etats finals de A et l etat 1 pour faire l etoile
  for (int f =0; f <nbetat; f++)
  {
    if IN(f,finals) // si f est un etat final de C
      afn_add_trans(C,f,'&',1);
  }
}
