/**
 * \file process_data.c
 * \brief Permet de traiter les lignes d'un fichier de trace suivant une syntaxe spécifique
 * \author John-Nathan Hill
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "process_data.h"

/**
 *param char *l : Une chaîne de charactère qui doit suivant la syntaxe suivante
 *					float int int int int int N'int' N'int' N'int'
 *return Line : Renvoie un objet du type struct (voir le header)
 */
Line parse_line(char *l)
{
	char *token = NULL;
	Line line = malloc(sizeof(struct Line));;
	line->t = atof(strtok(l," "));

	int tmp = atoi(strtok(NULL," "));
	switch (tmp){
		case 0 : line->code = 0; break;
		case 1 : line->code = 1; break;
		case 2 : line->code = 2; break;
		case 3 : line->code = 3; break;
		case 4 : line->code = 4; break;
		default : line->code = 0; break;
	}
	line->pid = atoi(strtok(NULL," "));
	line->fid = atoi(strtok(NULL," "));
	line->tos = atoi(strtok(NULL," "));

	if (line->code != 4)
		line->bif = atoi(strtok(NULL," "));
	else
		line->bif = -1;

	token = strtok(NULL," ");
	token++;
	line->source = atoi(token);
	token = strtok(NULL," ");
	token++;
	line->destination = atoi(token);
	token = strtok(NULL," ");
	token++;
	line->position = atoi(token);

	return line;
}

/**
 *param Line l : objet de type Line dont on va recuperer les donnees pour creer un objet Parcours_Paquet
 *param Liste liste : liste de Parcours_Paquet. Si == NULL, on initialise avec le paquet qui sera creer
 *return Liste : La liste en param mis à jour, avec le nouveau paquet en tête de liste
 */
Liste add_parcours_paquet(Line l, Liste liste)
{
	Parcours_Paquet* parcours = malloc(sizeof(Parcours_Paquet));
	parcours->pid = l->pid;
	parcours->source = l->source;
	parcours->destination = l->destination;
	parcours->duree = l->t;
	parcours->attente_file = 0;
	parcours->next = liste;
	return parcours;
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour retier lo'bjet Parcours_Paquet. Si absent, il renvoie la liste inchangée
 *return Liste : La liste en param mis à jour, avec le paquet retiré s'il était présent
 */
Liste del_parcours_paquet(int pid, Liste liste)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		Parcours_Paquet *tmp = liste->next;
		free(liste);
		return tmp;
	}
	else {
		liste->next = del_parcours_paquet(pid,liste->next);
		return liste;
	}
}

/**
 *param Liste liste : Liste que l'on veut détruire et désallouer l'espace
 */
void free_liste(Liste liste)
{
	Parcours_Paquet *tmp;
	while(liste!=NULL){
		tmp = liste->next;
		free(liste);
		liste = tmp;
	}
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet.
 *return Liste : On renvoie le maillon identifié par le pid. /!\ A utiliser uniquement pour recuperer les données, pas pour le modifier. Si absent, renvoie NULL
 */
Liste parcours(Liste liste, int pid)
{
	if (liste == NULL)
		return NULL;
	if(liste->pid == pid)
		return liste;
	else 
		return parcours(liste->next,pid);
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on rajoute t à la valeur actuelle de duree
 *return Liste : La liste mis à jour
 */
Liste add_duree(Liste liste, int pid, double t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		liste->duree += t;
		return liste;
	}
	else {
		liste->next = add_duree(liste->next, pid, t);
		return liste;
	}
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on retire t à duree
 *return Liste : La liste mis à jour
 */
Liste subtract_duree(Liste liste, int pid, double t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		double tmp = liste->duree;
		liste->duree = abs(liste->duree - tmp);
		return liste;
	}
	else {
		liste->next = subtract_duree(liste->next, pid, t);
		return liste;
	}
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on rajoute t à la valeur actuelle de attente
 *return Liste : La liste mis à jour
 */
Liste add_attente(Liste liste, int pid, double t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		liste->attente_file += t;
		return liste;
	}
	else {
		liste->next = add_attente(liste->next, pid, t);
		return liste;
	}
}

/**
 *param int pid : Un entier unique identifiant le paquet
 *param Liste liste : Liste que l'on va parcourir pour trouver le paquet et modifier sa duree.
 *param double t : on retire t à attente
 *return Liste : La liste mis à jour
 */
Liste substract_attente(Liste liste, int pid, double t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		liste->attente_file = abs(liste->duree - t);
		return liste;
	}
	else {
		liste->next = substract_attente(liste->next, pid, t);
		return liste;
	}
}

/**
 *param Calcul_Moy m : Objet que l'on met à jour
 *param double x : Valeur que l'on va rajouter à la moyenne
 *return Calcul_Moy : objet que l'on a mis a jour
 */
Calcul_Moy update_Calcul_Moy(Calcul_Moy m, double x)
{
	m.k++;
	double old_a=m.moy;
	if(m.k==1){
		m.moy=x;
		m.q=0;
		return(m);
	}
	m.moy=m.moy+((x-m.moy)/m.k);
	m.q=m.q+((x-old_a)*(x-m.moy));
	return(m);
}

/**
 *param Calcul_Moy m : Objet que l'on met à jour
 *return double : ecart_type correspondant a la moyenne
 */
double get_ecart_type(Calcul_Moy m)
{
	return(sqrt(m.q/(m.k)));
}

/**
 *FILE *fichier : pointeur d'un fichier de type matrice
 *int : Le nombre de noeuds qui correspondant au fichier
 */
int compteur_noeuds(FILE *fichier)
{
	int n = 0, i = 0;
	char *ligne = NULL;
	size_t len = 0;
	getline(&ligne,&len,fichier);
	while (ligne[i]){
		if (ligne[i] == '\t')
			n++;
		i++;
	}
	return n+1;
}