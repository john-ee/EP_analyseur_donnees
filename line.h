#ifndef _LINE_H
#define __LINE_H

typedef enum CODE {
DEPART_SOURCE, ARR_NOEUD, DEPART_FILE, ARR_DEST, DESTRUCTION
} CODE;

typedef struct Line
{
	//moment t de l'evenmt
	float t;
	//Voir def de CODE
	CODE code;
	//id unique du paquet
	int pid;
	//id unique du flux
	int fid;
	//type de service
	int tos;
	//operateur de bifurcation
	int bif;
	//source du paquet
	int source;
	//dest du paquet
	int destination;
	//pour code = 0,1,3,4 : position du paquet
	//pour code = 2 (depart file d'attente) : prochain saut
	int position;
} *Line;

typedef struct Parcours_Paquet *Parcours_Paquet;

struct Parcours_Paquet
{
	int pid;
	int source;
	int destination;
	float duree;
	float attente_file;
	int arrivee;
	Parcours_Paquet next;
};

Line parse_line(char *l);
void print_line(Line l);
Parcours_Paquet new_parcours_paquet(Line l);
Parcours_Paquet add_parcours_paquet(Line l, Parcours_Paquet liste);
void del_parcours_paquet(int pid, Parcours_Paquet liste);
void free_liste(Parcours_Paquet liste);
void set_duree(Parcours_Paquet parcours, float t);
int compteur_noeuds(FILE *fichier);
//void process_data(FILE *fichier, Parcours_Paquet tableau[], int taille_tableau[3]);

#endif