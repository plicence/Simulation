
typedef struct elem{

	struct elem * next;
	int val;


}elem;

typedef struct file {

	elem * tete;
	elem * queue;

}file;

file * creerFile(void);

void ajoutFile(file * f, int valeur);

int vide(file * f);

int defiler(file * f);

void effacerFile(file * f);


