#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "file.h"

#define Lambda 9
#define Mu 10

#define EPSILON 10
#define MAXEVENT 1000000
#define MAXTEMPS 100000

int Haut1 = 15;
int Bas1 = 0;
int Haut2 = 15;
int Bas2 = 0;
int temps = 0;
int compteur = 0;
double cumule = 0;
int Nc = 0;
int * anneau;
int * delta;
int * N;
int * cpt;
int * Ta;
int *ancien;//tableau enregistrant les temps d'attente ancien et nouveau
int *nouveau;
int *iteration;//compteur pour indiquer le nombre de valeurs de suite vérifiant la condtion d'arrêt
file * file1;
file * file10;

typedef struct Event{
	int type; // 0 pour arrivée_client, 1 pour Traitement_Station, 2 pour décalage_anneau
	int n;
	int etat; //0 pour non traité et 1 pour traité
}event;

typedef struct Echeancier
{
	event Tab[MAXEVENT];
	int taille;
}echeancier;

echeancier Ech;

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


void Arrive_Conteneur(event e, int K){

	printf("J'execute AC ! \n");
	for(int i = 0; i < K; i++){
		N[i] ++;
	}
	//printf("FIle = %d\n",N[1]); 
	ajoutFile(file1, temps);
	ajoutFile(file10, temps);
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

void Condition_Arret(int R){
	
	int res;
	printf("acien:%d, nouveau:%d",ancien[0],nouveau[0]);
	if(R < Bas1 || R > Haut1 ){

		Bas1 = R - (EPSILON/2);
		Haut1 = R + (EPSILON/2);
		iteration[0] = 0;
		printf("haut:%d, bas:%d",Haut1,Bas1);
	}
	else {

		iteration[0]++;
	}
	if(R < Bas2 || R > Haut2 ){

			Bas2 = R - (EPSILON/2);
			Haut2 = R + (EPSILON/2);
			iteration[1] = 0;
	}
	else {

		iteration[1]++;
	}
}


int Traitement_Station(event e,  FILE* F1,FILE* F10, int K) {

	//printf("Traitement\n");
	int arret = 0;
	int ta1;
	int ta10;
	int attente1;
	int attente10;
	for(int i = 0; i < K; i++) {

		if (anneau[150/K * i] == 150/K*i) { // Verifie si on est a la station de depart du conteneur
			anneau[150/K * i] = -1;
				Nc--;
		}
		if(N[i] > 0) {
			
			if (delta[i] == 0 && anneau[150/K *i] == -1) {
				anneau[150/K * i] = ((150/K) * i);
				if(i == 1) { 

					ta1 = defiler(file1);
					attente1 = temps -ta1;
					fprintf(F1,"%d  %d \n", attente1, temps);
					Condition_Arret(attente1);
				}
				if(i == 10) {
					ta10 = defiler(file10);
					attente10 = temps-ta10;
					fprintf(F10,"%d  %d \n",attente10, temps);
					Condition_Arret(attente10);
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

	printf("--> temps %d et N = %d taille : %d [",temps,Nc,Ech.taille);
	for (int i = 0; i < Ech.taille; i++)
	{
		e = Ech.Tab[i];

		if(e.type == 0)
			printf(" (AC, %d, %d),",e.n,e.etat);
		if(e.type == 1)
			printf(" (FS, %d, %d),",e.n,e.etat);
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

	file1 = creerFile();
	file10 = creerFile();
}

void Simulation(FILE* f1,FILE* F1,FILE* F10, int i, int K){
	
	long double OldNmoyen;
	long double Nmoyen;
	Initialisation(i);
	event e;
	int arret =0;
	while( iteration[0] < 100 && iteration[1] < 100 && temps < 10000){ //(Condition_Arret(OldNmoyen, Nmoyen) == 0)
		
		e = Extraire();
		OldNmoyen = Nmoyen;
		if (temps == 0)
		{
			//printf("temps = 0 et N = 0 et Nmoyen = 0\n");
			fprintf(f1, " 0   0\n");
		}else {
			Nmoyen = cumule/temps;
			printf("temps = %d  N = %d \n", temps, Nc);
			fprintf(f1, "%d  %d \n", temps, Nc);
			//fprintf(f1, "%f   %Lf\n", temps, Nmoyen);
		}

		if(e.type == 0){
			Arrive_Conteneur(e, K);printf("Arrive\n");}
		else if(e.type == 1){
			Traitement_Station(e, F1, F10, K);printf("Traitement\n");}
		else{
			Decalage_Anneau(e);printf("Decalage\n");}
	}
}

int main(int argc, char *argv[]) {

	int K = atoi(argv[1]);
	printf("%d",K);
		
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
	Simulation(f1, F1, F10, i , K);
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
	free(nouveau);
	free(ancien);
	free(iteration);
	effacerFile(file1);
	effacerFile(file10);

	printf("haut:%d, bas:%d",Haut1,Bas1);
/*
	file* test = creerFile();
	int k;
	for(int i = 0; i < 10; i++) {
		ajoutFile(test, i);

	}

	for(int i = 0; i < 10; i++) {
		k = defiler(test);
		printf("%d\n", k);

	}

	effacerFile(test);*/
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
