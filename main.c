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

#define EPSILON 1e-5
#define MAXEVENT 100000
#define MAXTEMPS 5000

double temps = 0;
long int n = 0; //Etats du systeme
int compteur = 0;
double cumule = 0;
int K = 0;
int Nc = 0;
int * anneau;
int * delta;
int * N;
int * cpt;
int * Ta;

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

mpf_t * Lecture_Fichier_Grand(){
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
}

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

mpfr_t * Lecture_Fichier_Grandm(){
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
	/*for(int i =0; i < 109; i++){
		mpfr_out_str(stdout, 10,0,probaFichier[i], MPFR_RNDD);
		printf("\n");
	}*/
	
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
	/*for(int i =0; i < 109; i++)
		gmp_printf("probafile[%d] = %FE\n",i,probaFichier[i]);
	*/
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

int Fct_Repart() {

	long double * proba = calloc(Ech.taille, sizeof(long double));
	long double * probaFichier = Lecture_Fichier();
	
	proba[0] = probaFichier[0];
	for(int i = 1; i <= 108;i++){
		proba[i] = proba[i-1] + probaFichier[i];
		printf("proba[%d] = %.50Le, probafile[%d] = %.50Le\n", i-1,proba[i-1],i,probaFichier[i]);	
		printf("proba[%d] = %.50Le\n", i,proba[i]);
		printf("----------------------------------------\n");
	}
	return 0;
}

int Generer_duree() {
	return 0;
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

	e.etat = 1;
	event e1;
	e1.n = temps + Generer_duree();
	e1.type = 0;
	e1.etat = 0;
	Ajouter_Evenement(e1);


}

void Decaler_Anneau() {

	int precedent = anneau[K-1];
	int courant = 0;
	for(int i = 0; i < K; i ++) {

		courant = anneau[i];
		anneau[i] = precedent;

	}

}

void Traitement_Station(event e) {

	printf("Traitement\n");
	for(int i = 0; i < K; i++) {

		if (anneau[150/K * i] == 150/K*i) { // Verifie si on est a la station de depart du conteneur

			anneau[150/K * i] = -1;
				n--;
		}

		if(N[i] > 0) {

			if (delta[i] == 0 && anneau[150/K *i] == -1) {
				anneau[150/K * i] = ((150/K) * i);
				//ajouter_point(Ta[i], n); //Sauvegarde le temps d’attente afin de pouvoir tracer une courbe
				Ta[i] = 0;
				n++;
				delta[i] = 10;

			}

			else {
				delta[i] --;
				Ta[i] ++;

				if (anneau[150/K * i] >= 0) {
					cpt[i]++ ;
				}
			}


		}


		else {

			Ta[i] ++;
		}

	}
	e.etat = 1;
	event e1;
	e1.n = temps + 1;
	e1.type = 1;
	e1.etat = 0;
	Ajouter_Evenement(e1);
}

void Decalage_Anneau(event e) {

	printf("Rotation\n");
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

	printf("--> temps %f et N = %ld taille : %d [",temps,n,Ech.taille);
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

int Condition_Arret(long double Old, long double New){
	if(fabs(Old-New) < EPSILON && temps > 1000){
		compteur++;
		if(compteur > 1e3)
			return 1;
		}
		return 0;
}

void Initialisation(int i) {

	for(int j = 0; j < i; j++) {
		delta[j] = 0;
		N[i] = 0;
		cpt[i] = 0;
		Ta[i] = 0;
	}
	for(int j = 0; j < K; j++) {
		anneau[j] = -1;
	}
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



void Simulation(FILE* f1, int i){
	long double OldNmoyen;
	long double Nmoyen;

	Initialisation(i);
	event e;

	while(Condition_Arret(OldNmoyen, Nmoyen) == 0){ //(temps<MAXTEMPS)
		e = Extraire();
		cumule += (e.n - temps) * n;
		OldNmoyen = Nmoyen;

		if (temps == 0)
		{
			printf("temps = 0 et N = 0 et Nmoyen = 0\n");
			fprintf(f1, "0    0\n");
		}else {
			Nmoyen = cumule/temps;
			printf("temps = %f et N = %ld et Nmoyen = %Lf\n", temps, n, Nmoyen);
			fprintf(f1, "%f   %Lf\n", temps, Nmoyen);
		}

		if(e.type == 0)
			Arrive_Conteneur(e);
		else if(e.type == 1)
			Traitement_Station(e);
		else
			Decalage_Anneau(e);

	}

}



int main(void) {

	Fct_Repart_mpfr();
/*
	int i = 0;
	anneau = malloc(K * sizeof(int));
	delta = malloc(i * sizeof(int));
	N = malloc(i * sizeof(int));
	cpt = malloc(i * sizeof(int));
	Ta = malloc(i * sizeof(int));
	FILE *f1 = fopen("Simulation_MM2.data","w");
	Simulation(f1, i);
	fclose(f1);
*/
	return EXIT_SUCCESS;
}
