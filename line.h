#ifndef _LINE_H
#define __LINE_H

typedef enum CODE {
DEPART_SOURCE, ARR_NOEUD, DEPART_FILE, ARR_DEST, DESTRUCTION
} CODE;

typedef struct Line
{
	//moment t de l'evenmt
	double t;
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

typedef struct Parcours_Paquet Parcours_Paquet;
struct Parcours_Paquet
{
	int pid;
	int source;
	int destination;
	double duree;
	double attente_file;
	Parcours_Paquet *next;
};
typedef Parcours_Paquet* Liste;

Line parse_line(char *l);
void print_line(Line l);
Liste add_parcours_paquet(Line l, Liste liste);
Liste del_parcours_paquet(int pid, Liste liste);
void free_liste(Liste liste);
Liste parcours(Liste liste, int pid);
double get_duree(Liste liste, int pid);
Liste add_duree(Liste liste, int pid, double t);
Liste subtract_duree(Liste liste, int pid, double t);
Liste set_attente(Liste liste, int pid, double t);
int compteur_noeuds(FILE *fichier);
//void process_data(FILE *fichier, Parcours_Paquet tableau[], int taille_tableau[3]);

#endif