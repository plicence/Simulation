#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "file.h"

#define EPSILON 10
#define MAXEVENT 1000000
#define MAXTEMPS 50000

int Haut1 = 15;
int Bas1 = 0;
int Haut2 = 15;
int Bas2 = 0;
int temps = 0;
int Nc = 0;
int * anneau;//structure de l'anneau
int * delta;//tableau de délais d'attente des stations
int * N;//tableau des remplissages de stations
int *iteration;//compteur pour indiquer le nombre de valeurs de suite vérifiant la condtion d'arrêt
file * file1;//file des délais pour la station 1
file * file10;//file des délais pour la station 10

typedef struct Event{
	int type; // 0 pour arrivée_client, 1 pour Traitement_Station, 2 pour décalage_anneau
	int n;//temps d'arrivée
	int etat; //0 pour non traité et 1 pour traité
}event;

typedef struct Echeancier
{
	event Tab[MAXEVENT];
	int taille;
}echeancier;

echeancier Ech;

long double * Lecture_Fichier() { //Lit le fichier d'interarrivées et renvoit le tableau associé
	FILE * f = fopen("Interarrivees.txt","r");
	long double * file = calloc(109 , sizeof(long double));
	if(file == NULL)
	{
		printf("Memory allocation failed");
		exit(EXIT_FAILURE);
	} else {
		if( f!= NULL){
			int cmp;
			long double proba; 
			for(int i = 0; i < 109; i++){
				fscanf(f,"%d %Le", &cmp, &proba);
				file[cmp] = proba;
			}
			fclose(f);
			return file;
		}
		else return NULL;
	}
}

long double * Fct_Repart() {//renvoit la probabilité suivant la distribution du fichier

	long double * proba = calloc(109, sizeof(long double));
	long double * probaFichier = Lecture_Fichier();
	if(proba == NULL)
	{
		printf("Memory allocation failed");
		exit(EXIT_FAILURE);
	} else {
		proba[0] = probaFichier[0];
		for(int i = 1; i <= 108;i++){
			proba[i] = proba[i-1] + probaFichier[i];
		}
		free(probaFichier);
		return proba;
	}
}

long double Generer_duree() {//Retourne une durée par rapport à la fonction de répartition
	
	int i = 0;
	long double nbr_alea = 0;
	nbr_alea = (long double)rand() / (long double)RAND_MAX;
	long double * proba;
	
	proba = Fct_Repart(); 
	
	while(proba[i] < nbr_alea){
		i++;
	}
	
	free(proba);
	return i-1;
}

void Ajouter_Evenement(event e) {//Ajoute un événement de l'échéancier
	if(Ech.taille < MAXEVENT){
		
		Ech.Tab[Ech.taille] = e;
		Ech.taille++;
		printf("Taille = %d \n",Ech.taille);
	} else {
		
		printf("Echeancier plein ! \n");
		exit(0);
	}
}


void Arrive_Conteneur(event e, int K) {// Evenement d'arrivée de conteneurs

	printf("J'execute AC ! \n");
	for(int i = 0; i < K; i++){
		N[i] ++;
	}
	ajoutFile(file1, temps);
	ajoutFile(file10, temps);
	e.etat = 1;
	event e1;
	e1.n = temps + Generer_duree();
	e1.type = 0;
	e1.etat = 0;
	Ajouter_Evenement(e1);
}

void Decale_Anneau() { //fonction qui fait décaler tous les éléments de la courbe

	int precedent = anneau[149];
	int courant = 0;
	for(int i = 0; i < 150; i ++) {

		courant = anneau[i];
		anneau[i] = precedent;
		precedent = courant;
	}

}

void Condition_Arret(int R) { //Vérifie si les stations sont stables
	
	int res;
	if(R < Bas1 || R > Haut1 ){

		Bas1 = R - (EPSILON/2);
		Haut1 = R + (EPSILON/2);
		iteration[0] = 0;
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


int Traitement_Station(event e,  FILE* F1,FILE* F10, int K) { //Evenement du traitement de station

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
			
			if (delta[i] == 0 && anneau[150/K *i] == -1) { //vérifie si on peut ajouter un élément
				anneau[150/K * i] = ((150/K) * i);
				if(i == 0) { 

					ta1 = defiler(file1);
					attente1 = temps -ta1;
					fprintf(F1,"%d  %d \n", attente1, temps);
					Condition_Arret(attente1);
				}
				if(i == 9) {
					ta10 = defiler(file10);
					attente10 = temps-ta10;
					fprintf(F10,"%d  %d \n",attente10, temps);
					Condition_Arret(attente10);
				}

				Nc++;
				N[i]--;
				delta[i] = 10;
			}
			else {
				if(delta[i] > 0)
					delta[i] --;

			}
		}

	}
	e.etat = 1;
	event e1;
	e1.n = temps + 1;
	e1.type = 1;
	e1.etat = 0;
	Ajouter_Evenement(e1);
	return arret;
}

void Decalage_Anneau(event e) { //Evenement de décalage de l'anneau

	Decale_Anneau();
	e.etat = 1;
	event e1;
	e1.n = temps + 1;
	e1.type = 2;
	e1.etat = 0;
	Ajouter_Evenement(e1);
	temps++;

}


event Extraire() { //Récupère l'événement
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

void Init_Anneau(int i, int K) {//Initialise la simulation

	printf("test");
	anneau = malloc(i * sizeof(int));
	if(anneau == NULL) {
		printf("Memory failure in main.c\n");
		exit(EXIT_FAILURE);

	}
	for(int j = 0; j < 150; j++) {
		anneau[j] = -1;
	}
	printf("test2");
	delta =calloc(K, sizeof(int));
	if(delta == NULL) {
		printf("Memory failure in main.c\n");
		free(anneau);
		exit(EXIT_FAILURE);

	}
	printf("test3");
	N = calloc(K, sizeof(int));
	if(N == NULL) {
		printf("Memory failure in main.c\n");
		free(anneau);
		free(delta);
		exit(EXIT_FAILURE);

	}
	printf("test4");
	iteration = calloc(2, sizeof(int));
	if(iteration == NULL) {
		printf("Memory failure in main.c Init_Anneau\n");
		free(anneau);
		free(delta);
		free(N);
		exit(EXIT_FAILURE);

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

void Simulateur(FILE* f1,FILE* F1,FILE* F10, int i, int K) {// Effectue la simulation

	Init_Anneau(i, K);
	event e;
	int arret =0;
	while(iteration[0] < 100 && iteration[1] < 100 && temps < MAXTEMPS){
		
		e = Extraire();
		if (temps == 0)
		{
			fprintf(f1, " 0   0\n");
		}else {
			fprintf(f1, "%d  %d \n", temps, Nc);
		}

		if(e.type == 0){
			Arrive_Conteneur(e, K);}
		else if(e.type == 1){
			Traitement_Station(e, F1, F10, K);}
		else{
			Decalage_Anneau(e);}
	}
}

int main(int argc, char *argv[]) {

	int K = atoi(argv[1]);
	printf("K = %d\n",K);
		
    srand(time(NULL) + getpid());
	int i = 150;
	FILE *f1 = fopen("Simulation_MM2.data","w");
	FILE * F1  = fopen("STATION1.data","w");
	FILE * F10 = fopen("STATION10.data","w");
	Simulateur(f1, F1, F10, i , K);

	fclose(f1);
	fclose(F1);
	fclose(F10);
	
	free(anneau);
	free(delta);
	free(N);
	free(iteration);
	effacerFile(file1);
	effacerFile(file10);

	printf("haut:%d, bas:%d\n",Haut1,Bas1);

	return EXIT_SUCCESS;
}
