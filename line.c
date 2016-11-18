#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "line.h"

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

void print_line(Line l)
{
	printf("%lf %d %d %d %d ",l->t,(int)l->code,l->pid,l->fid,l->tos);
	if(l->code==DESTRUCTION)
		printf("%d ",l->bif);
	printf("%d %d %d\n",l->source,l->destination,l->position);
}

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

Liste del_parcours_paquet(int pid, Liste liste)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		Parcours_Paquet *tmp = liste->next;
		free(liste);
		//tmp = del_parcours_paquet(pid,tmp);
		return tmp;
	}
	else {
		liste->next = del_parcours_paquet(pid,liste->next);
		return liste;
	}
}

void free_liste(Liste liste)
{
	Parcours_Paquet *tmp;
	while(liste!=NULL){
		tmp = liste->next;
		free(liste);
		liste = tmp;
	}
}

Liste parcours(Liste liste, int pid)
{
	if (liste == NULL)
		return NULL;
	if(liste->pid == pid)
		return liste;
	else 
		return parcours(liste->next,pid);
}

double get_duree(Liste liste, int pid)
{
	if(liste->pid == pid)
		return liste->duree;
	else 
		return get_duree(liste->next, pid);
}

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

double get_variance(Calcul_Moy m)
{
	return(m.q/(m.k-1));
}

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