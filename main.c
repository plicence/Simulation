#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>


#include "gmp.h"
#include <mpfr.h>

#define Lambda 9
#define Mu 10

#define EPSILON 10
#define MAXEVENT 1000000
#define MAXTEMPS 100000

int Haut = EPSILON;
int Bas = -EPSILON;
double temps = 0;
//long int n = 0; //Etats du systeme
int compteur = 0;
double cumule = 0;
int K = 22;
int Nc = 0;
int * anneau;
int * delta;
int * N;
int * cpt;
int * Ta;
int *ancien;//tableau enregistrant les temps d'attente ancien et nouveau
int *nouveau;
int *iteration;//compteur pour indiquer le nombre de valeurs de suite vérifiant la condtion d'arrêt
int different =0;//nombre de valeurs ne respectant pas les conditions

typedef struct Event{
	int type; // 0 pour arrivée_client, 1 pour Traitement_Station, 2 pour décalage_anneau
	double n;
	int etat; //0 pour non traité et 1 pour traité
}event;

typedef struct Echeancier
{
	event Tab[MAXEVENT];
	int taille;
}echeancier;

echeancier Ech;

/*mpf_t * Lecture_Fichier_Grand(){
	FILE * f = fopen("Interarrivees.txt","r");
	mpf_t * file = calloc(109, sizeof(mpf_t));
	for(int i = 0; i < 109; i++){
		mpf_init(file[i]);
	}
	if(f != NULL){
		int cmp;
		mpf_t proba; 
		mpf_init(proba);
		for(int i = 0; i < 109; i++){
			gmp_fscanf(f,"%d %.50FE", &cmp, &proba);
			//gmp_printf("cmp = %d", cmp);
			//gmp_printf("proba = %Fe\n", proba);
			mpf_set(file[cmp], proba);
			//gmp_printf("cmp = %d, proba = %Fe\n", cmp, file[cmp]);
		}
		mpf_clear(proba);
		fclose(f);
		return file;
	}
	else return NULL;
}*/

long double * Lecture_Fichier(){
	FILE * f = fopen("Interarrivees.txt","r");
	long double * file = calloc(109 , sizeof(long double));
	if( f!= NULL){
		int cmp;
		long double proba; 
		for(int i = 0; i < 109; i++){
			fscanf(f,"%d %Le", &cmp, &proba);
			file[cmp] = proba;
			//printf("cmp = %d, proba = %Le\n", cmp, proba);
		}
		fclose(f);
		return file;
	}
	else return NULL;
}

/*mpfr_t * Lecture_Fichier_Grandm(){
	FILE * f = fopen("Interarrivees.txt","r");
	mpfr_t * file = calloc(109, sizeof(mpfr_t));
	for(int i = 0; i < 109; i++){
		mpfr_init2(file[i],100);
	}
	if(f != NULL){
		mpfr_t cmp;
		mpfr_t proba; 
		mpfr_init2(cmp,1);
		mpfr_init2(proba,100000);
		for(int i = 0; i < 109; i++){
			mpfr_inp_str(cmp, f, 10,MPFR_RNDD);
			mpfr_inp_str(proba, f, 10,MPFR_RNDD);
			mpfr_set(file[i], proba, MPFR_RNDD);
			//gmp_printf("cmp = %d, proba = %Fe\n", cmp, file[cmp]);
			mpfr_out_str(stdout, 10,0,file[i], MPFR_RNDD);
			printf("\n");
		}
		mpfr_clear(proba);
		fclose(f);
		return file;
	}
	else return NULL;
}

int Fct_Repart_mpfr() {

	mpfr_t * proba = calloc(109, sizeof(mpfr_t));
	mpfr_t * probaFichier = calloc(109, sizeof(mpfr_t));
	
	for(int i = 0; i < 109; i++){
		mpfr_init2(proba[i],100);
		mpfr_init2(probaFichier[i],1000);
	}

	probaFichier = Lecture_Fichier_Grandm();
	for(int i =0; i < 109; i++){
		mpfr_out_str(stdout, 10,0,probaFichier[i], MPFR_RNDD);
		printf("\n");
	}
	
	mpfr_set(proba[0], probaFichier[0], MPFR_RNDD);
	for(int i = 1; i < 109;i++){
		printf("%d ", i);
		mpfr_add(proba[i], proba[i-1], probaFichier[i], MPFR_RNDD);
		mpfr_out_str(stdout, 10,0,proba[i], MPFR_RNDD);
		printf("\n----------------------------------------\n");
	}
	
	mpfr_clear(*proba);
	mpfr_clear(*probaFichier);
	mpfr_free_cache();
	return 0;
}

int Fct_Repart_mpz() {

	mpf_t * proba = calloc(109, sizeof(mpf_t));
	mpf_t * probaFichier = calloc(109, sizeof(mpf_t));
	
	for(int i = 0; i < 109; i++){
		mpf_init(proba[i]);
		mpf_init(probaFichier[i]);
	}

	probaFichier = Lecture_Fichier_Grand();
	for(int i =0; i < 109; i++)
		gmp_printf("probafile[%d] = %FE\n",i,probaFichier[i]);
	
	mpf_set(proba[0], probaFichier[0]);
	for(int i = 1; i < 109;i++){
		
		mpf_add(proba[i], proba[i-1], probaFichier[i]);
		gmp_printf("proba[%d] = %.50FE, probafile[%d] = %.50FE\n", i-1,proba[i-1],i,probaFichier[i]);	
		gmp_printf("proba[%d] = %.50FE\n", i,proba[i]);
		printf("----------------------------------------\n");
	}
	
	mpf_clear(*proba);
	mpf_clear(*probaFichier);
	return 0;
}
*/
long double * Fct_Repart() {

	long double * proba = calloc(109, sizeof(long double));
	long double * probaFichier = Lecture_Fichier();
	
	proba[0] = probaFichier[0];
	for(int i = 1; i <= 108;i++){
		proba[i] = proba[i-1] + probaFichier[i];
		/*printf("proba[%d] = %.50Le, probafile[%d] = %.50Le\n", i-1,proba[i-1],i,probaFichier[i]);	
		printf("proba[%d] = %.50Le\n", i,proba[i]);
		printf("----------------------------------------\n");*/
	}
	free(probaFichier);
	return proba;
}

long double Generer_duree() {
	
	int i = 0;
	long double nbr_alea = 0;
	nbr_alea = (long double)rand() / (long double)RAND_MAX;
	//printf("nbr_alea = %Le\n", nbr_alea);
	long double * proba;
	
	proba = Fct_Repart(); 
	/*for(int i = 1; i <= 108;i++){
		printf("proba[%d] = %.50Le\n", i,proba[i]);
		printf("----------------------------------------\n");
	}*/
	
	while(proba[i] < nbr_alea){
		//printf("proba[%d] = %.Le < %Le\n", i,proba[i], nbr_alea);
		i++;
	}
	//printf("proba[%d] = %.Le\n", i-1,proba[i-1]);
	
	free(proba);
	return i-1;
}

void Ajouter_Evenement(event e){
	if(Ech.taille < MAXEVENT){
		Ech.Tab[Ech.taille] = e;
		Ech.taille++;
		printf("Taille = %d \n",Ech.taille);
	} else {
		printf("Echeancier plein ! \n");
		exit(0);
	}
}


void Arrive_Conteneur(event e){

	printf("J'execute AC ! \n");

	for(int i = 0; i < K; i++){
		N[i] ++;
	}
	//printf("FIle = %d\n",N[1]); 

	e.etat = 1;
	event e1;
	e1.n = temps + Generer_duree();
	e1.type = 0;
	e1.etat = 0;
	Ajouter_Evenement(e1);


}

void Decaler_Anneau() {

	int precedent = anneau[149];
	int courant = 0;
	for(int i = 0; i < 150; i ++) {

		courant = anneau[i];
		anneau[i] = precedent;
		precedent = courant;
	}

}

int Condition_Arret(long double Old, long double New){
	if(ancien[0]-nouveau[0] < Bas && ancien[0]-nouveau[0] > Haut ){
		Bas = (ancien[0]-nouveau[0]) - EPSILON/2;
		Haut = (ancien[0]-nouveau[0]) + EPSILON/2;
		iteration[0] = 0;
		return 0;
	}

	else {

		iteration[0]++;
		if(iteration[0] > 1000) {

			return 1;

		}



	}


return 0;

}


int Traitement_Station(event e,  FILE* F1,FILE* F10) {

	//printf("Traitement\n");
	int arret = 0;
	for(int i = 0; i < K; i++) {

		if (anneau[150/K * i] == 150/K*i) { // Verifie si on est a la station de depart du conteneur
			anneau[150/K * i] = -1;
				Nc--;
		}
		if(N[i] > 0) {
			
			if (delta[i] == 0 && anneau[150/K *i] == -1) {
				anneau[150/K * i] = ((150/K) * i);
				if(i == 1) { 
					ancien[0] = nouveau[0]; 
					nouveau[0] = Ta[1];
					fprintf(F1,"%d  %f \n", Ta[1], temps); 
					arret =Condition_Arret(0,0); 
				}
				else if(i == 10) {
					ancien[1] = nouveau[1]; 
					nouveau[1] = Ta[10];
					fprintf(F10,"%d  %f \n", Ta[1], temps); 
					}

				Ta[i] = 0;
				Nc++;
				N[i]--;
				delta[i] = 10;
			}
			else {
				if(delta[i] > 0)
					delta[i] --;
				Ta[i] ++;

				if (anneau[150/K * i] >= 0) {
					cpt[i]++ ;
				}
			}
		}
		else {

			Ta[i]++;
		}

	}
	/*for (int i = 0; i < 150; i++)
	{
		printf("anneau[%d] = %d ", i, anneau[i]);
	}
	printf("-----------------------------------------\n");*/

	e.etat = 1;
	event e1;
	e1.n = temps + 1;
	e1.type = 1;
	e1.etat = 0;
	Ajouter_Evenement(e1);
	return arret;
}




void Decalage_Anneau(event e) {

	//printf("Rotation\n");
	Decaler_Anneau();
	e.etat = 1;
	event e1;
	e1.n = temps + 1;
	e1.type = 2;
	e1.etat = 0;
	Ajouter_Evenement(e1);
	temps++;

}



void affiche_echeancier(){
	event e;

	printf("--> temps %f et N = %d taille : %d [",temps,Nc,Ech.taille);
	for (int i = 0; i < Ech.taille; i++)
	{
		e = Ech.Tab[i];

		if(e.type == 0)
			printf(" (AC, %lf, %d),",e.n,e.etat);
		if(e.type == 1)
			printf(" (FS, %lf, %d),",e.n,e.etat);
	}
	printf("] \n \n ");
}

event Extraire(){
	int i, imin;
	event min;

	for (i = 0; i < Ech.taille; i++)
	{
		if (Ech.Tab[i].etat == 0)
		{
			min = Ech.Tab[i];
			imin = i;
			break;
		}
	}

	for (i = 0; i < Ech.taille; i++)
	{
		if (min.n > Ech.Tab[i].n && Ech.Tab[i].etat == 0)
		{
			min = Ech.Tab[i];
			imin = i;
		}
	}

	Ech.Tab[imin].etat = 1;
	return min;
}


void Initialisation(int i) {

	for(int j = 0; j < 150; j++) {
		anneau[j] = -1;
	}
	
	Ech.taille = 0;
	
	event e1;
	e1.n = Generer_duree();
	e1.type = 0;
	e1.etat = 0;
	Ajouter_Evenement(e1);
	event e2;
	e2.n = 0;
	e2.type = 1;
	e2.etat = 0;
	Ajouter_Evenement(e2);
	event e3;
	e3.n = 0;
	e3.type = 2;
	e3.etat = 0;
	Ajouter_Evenement(e3);
}

void ajouter_point(FILE * F1,int Ta){
	fprintf(F1,"%d  %f \n", Ta, temps); 
}

void Simulation(FILE* f1,FILE* F1,FILE* F10, int i){
	long double OldNmoyen;
	long double Nmoyen;

	Initialisation(i);
	event e;
	int arret =0;
	while(arret == 0){ //(Condition_Arret(OldNmoyen, Nmoyen) == 0)
		e = Extraire();
		cumule += (e.n - temps) * Nc;
		OldNmoyen = Nmoyen;

		if (temps == 0)
		{
			//printf("temps = 0 et N = 0 et Nmoyen = 0\n");
			fprintf(f1, " 0   0\n");
		}else {
			Nmoyen = cumule/temps;
			printf("temps = %f  N = %d \n", temps, Nc);
			fprintf(f1, "%f  %d \n", temps, Nc);
			//fprintf(f1, "%f   %Lf\n", temps, Nmoyen);
		}
		//fprintf(F1,"%d  %f \n", Ta[1], temps); 
		//fprintf(F10,"%d  %f \n", Ta[10], temps); 

		//ajouter_point(F1, Ta[1]);
		//ajouter_point(F10, Ta[10]);//Sauvegarde le temps d’attente afin de pouvoir tracer une courbe
	

		if(e.type == 0){
			Arrive_Conteneur(e);printf("Arrive\n");}
		else if(e.type == 1){
			arret = Traitement_Station(e, F1, F10);printf("Traitement\n");}
		else{
			Decalage_Anneau(e);printf("Decalage\n");}

	}

}



int main(void) {


    srand(time(NULL) + getpid());
	int i = 150;
	anneau = malloc(i * sizeof(int));
	
	delta =calloc(K, sizeof(int)); //malloc(i * sizeof(int));
	N = calloc(K, sizeof(int));
	cpt = calloc(K, sizeof(int));
	Ta = calloc(K, sizeof(int));
	iteration = calloc(2, sizeof(int));
	nouveau = calloc(2, sizeof(int));
	ancien = calloc(2, sizeof(int));
	FILE *f1 = fopen("Simulation_MM2.data","w");
	FILE * F1  = fopen("STATION1.data","w");
	FILE * F10 = fopen("STATION10.data","w");
	Simulation(f1, F1, F10, i);
	//printf("Ech.taille = %d\n", Ech.taille);
	for (i = 0; i < K; i++)
	{
		printf("delta[%d] = %d ", i, delta[i]);
		printf("N[%d] = %d\n", i, N[i]);
	}
	fclose(f1);
	fclose(F1);
	fclose(F10);
	
	free(anneau);
	free(delta);
	free(N);
	free(cpt);
	free(Ta);

/*	
	for (i = 0; i < 150; i++)
	{
		anneau[i] = i;
		printf("anneau[%d] = %d\n", i, anneau[i]);
	}
	Decaler_Anneau();
	for (i = 0; i < 150; i++){
		printf("anneau[%d] = %d\n", i, anneau[i]);
	}
    
  */  
	return EXIT_SUCCESS;
}
