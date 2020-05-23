#include <stdlib.h>
#include <stdio.h>
#include "file.h"


file * creerFile() {

	file * f;
	f = malloc (sizeof(file));
	f->tete = 0;
	f->queue = 0;
	return f;

}

void ajoutFile(file * f, int valeur) {

	elem * e;
	e = malloc(sizeof(elem ));
	e->val = valeur;
	e->next = 0;

	if(f->tete == 0) {
		f->tete = e;
	}
	else {
		f->queue->next = e;
	}

	f->queue = e;

}

int vide(file * f) {

	return (f->tete == 0);
}

int defiler(file * f) {

	int res = -1;
	struct elem * e;
	if(!vide(f)) {
		res = f->tete->val;
		e = f->tete;
		f->tete = e->next;
		free(e);

	}

	return res;

}

void effacerFile(file * f) {

	while(!vide(f)) {
		defiler(f);
	}

	free(f);
}
