/**
 * \file process_dat.h
 * \brief Header de process_data.c 
 * \author John-Nathan Hill
 */

#ifndef _PROCESS_DATA_H
#define __PROCESS_DATA_H


/*
 * Cet enum permet de gerer les differents cas en mettant un nom dessus.
 * Il est plus simple de reconnaitre le cas à traiter.
 * De plus, chaque nom dans l'enum correspond à un entier, allant de 0 à 4
 * 0 : départ de la source
 * 1 : arrivée dans un nœud intermédiaire
 * 2 : départ d’une file d’attente
 * 3 : arrivée à destination
 * 4 : destruction d’un paquet (placement dans une file pleine)
 */
typedef enum CODE {
DEPART_SOURCE, ARR_NOEUD, DEPART_FILE, ARR_DEST, DESTRUCTION
} CODE;


/*
 * Cette structure parse une ligne du ficher de trace
 */
typedef struct Line
{
	//instant t de l'evenmt
	double t;
	//Voir def de CODE
	CODE code;
	//id unique du paquet
	int pid;
	//id unique du flux
	int fid;
	//type de service
	int tos;
	//operateur de bifurcation : N'apparaît pas quand code == 4
	int bif;
	//source du paquet
	int source;
	//dest du paquet
	int destination;
	//pour code = 0,1,3,4 : position du paquet
	//pour code = 2 (depart file d'attente) : prochain saut
	int position;
} *Line;


/*
 * Cette structure permet de tracer les paquets pendant la lecture du fichier.
 * Grace au variable duree et attente_file, on peut faire des moyennes dessus.
 * En travaillant sur liste chaînee, on peut se debarrasser d'un paquet une fois arrivé a destination ou detruit
 */
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


/*
 * L'interêt de cette strcture est de calculer m'ecart-type sans connaitre la moyenne
 * Ainsi, en un parcours, on calcule la moyennet l'ecart-type
 */
typedef struct
{
	double moy;
	double q;
	double k;
} Calcul_Moy;

/**
 *param char *l : Une chaîne de charactère qui doit suivant la syntaxe suivante
 *					float int int int int int N'int' N'int' N'int'
 *return Line : Renvoie un objet du type struct (voir le header)
 */
Line parse_line(char *l);

/**
 *param Line l : objet de type Line dont on va recuperer les donnees pour creer un objet Parcours_Paquet
 *param Liste liste : liste de Parcours_Paquet. Si == NULL, on initialise avec le paquet qui sera creer
 *return Liste : La liste en param mis à jour, avec le nouveau paquet en tête de liste
 */
Liste add_parcours_paquet(Line l, Liste liste);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour retier lo'bjet Parcours_Paquet. Si absent, il renvoie la liste inchangée
 *return Liste : La liste en param mis à jour, avec le paquet retiré s'il était présent
 */
Liste del_parcours_paquet(int pid, Liste liste);

/**
 *param Liste liste : Liste que l'on veut détruire et désallouer l'espace
 */
void free_liste(Liste liste);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet.
 *return Liste : On renvoie le maillon identifié par le pid. /!\ A utiliser uniquement pour recuperer les données, pas pour le modifier. Si absent, renvoie NULL
 */
Liste parcours(Liste liste, int pid);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on rajoute t à la valeur actuelle de duree
 *return Liste : La liste mis à jour
 */
Liste add_duree(Liste liste, int pid, double t);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on retire t à duree
 *return Liste : La liste mis à jour
 */
Liste subtract_duree(Liste liste, int pid, double t);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on rajoute t à la valeur actuelle de attente
 *return Liste : La liste mis à jour
 */
Liste add_attente(Liste liste, int pid, double t);

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on retire t à attente
 *return Liste : La liste mis à jour
 */
Liste substract_attente(Liste liste, int pid, double t);

/**
 *param Calcul_Moy m : Objet que l'on met à jour
 *param double x : Valeur que l'on va rajouter à 
 *return Calcul_Moy : objet que l'on a mis a jour
 */
Calcul_Moy update_Calcul_Moy(Calcul_Moy m, double x);

/**
 *param Calcul_Moy m : Objet que l'on met à jour
 *return double : ecart_type correspondant a la moyenne
 */
double get_ecart_type(Calcul_Moy m);

/**
 *FILE *fichier : pointeur d'un fichier de type matrice
 *int : Le nombre de noeuds qui correspondant au fichier
 */
int compteur_noeuds(FILE *fichier);


#endif