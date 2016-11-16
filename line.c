#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "line.h"

Line parse_line(char *l)
{
	int i = 0;
	//char *token = NULL;
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

	line->source = strdup(strtok(NULL," "));
	//while (line->source[i])
	//	i++;
	//line->source[i] = '\0';
	line->destination = strdup(strtok(NULL," "));
	//i = 0;
	//while (line->destination[i])
	//	i++;
	//line->destination[i] = '\0';
	line->position = strdup(strtok(NULL," "));
	//i = 0;
	//while (line->position[i])
	//	i++;
	//line->position[i] = '\0';

	return line;
}

void print_line(Line l)
{
	printf("%f %d %d %d %d ",l->t,(int)l->code,l->pid,l->fid,l->tos);
	if(l->code==DESTRUCTION)
		printf("%d ",l->bif);
	printf("%s %s %s\n",l->source,l->destination,l->position);
}



/*void add_parcours_paquet(Line l, Parcours_Paquet liste)
{
	while(liste!=NULL && liste->pid == l->pid){
		liste = liste->next;
	}
	if (liste == NULL)
		liste = new_parcours_paquet(l);
}*/

Liste add_parcours_paquet(Line l, Liste liste)
{
	Parcours_Paquet* parcours = malloc(sizeof(Parcours_Paquet));
	parcours->pid = l->pid;
	parcours->source = strdup(l->source);
	parcours->destination = strdup(l->destination);
	parcours->duree = l->t;
	parcours->attente_file = 0;
	parcours->chemin = NULL;
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

float get_duree(Liste liste, int pid)
{
	if(liste->pid == pid)
		return liste->duree;
	else 
		return get_duree(liste->next, pid);
}

Liste add_duree(Liste liste, int pid, float t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		liste->duree += t;
		return liste;
	}
	else {
		liste->next = add_duree(liste, pid, t);
		return liste;
	}
}

Liste subtract_duree(Liste liste, int pid, float t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		float tmp = liste->duree;
		liste->duree = abs(liste->duree - tmp);
		return liste;
	}
	else {
		liste->next = subtract_duree(liste, pid, t);
		return liste;
	}
}

Liste set_attente(Liste liste, int pid, float t)
{
	if (liste == NULL)
		return NULL;

	else if(liste->pid == pid){
		liste->attente_file += t;
		return liste;
	}
	else {
		liste->next = set_attente(liste, pid, t);
		return liste;
	}
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

/*
void process_data(FILE *fichier, Parcours_Paquet tableau[], int info_tableau[3])
{
	char *ligne = NULL;
	size_t len = 0;
	ssize_t read;
	Line line;
	int pid_min = info_tableau[1], index = 0;
	//int pid_max = info_tableau[2];

	while((read=getline(&ligne, &len, fichier)) != -1){
		line = parse_line(ligne);
		index = line->pid - pid_min;
		//printf("%d ",read);

		if (tableau[index] == NULL)
			tableau[index] = new_parcours_paquet(line);
		switch(line->code){
			case DEPART_SOURCE : 
				set_duree(tableau[index],line->t);
				set_attente(tableau[index],tableau[index]->attente_file + line->t);
				break;
			case ARR_DEST :
				set_duree(tableau[index],line->t - tableau[index]->duree);
				switch_arrivee(tableau[index]);
				break;
			case ARR_NOEUD :
				if (line->position != line->destination)
					set_attente(tableau[index],tableau[index]->attente_file + line->t);
				break;
			case DEPART_FILE : set_attente(tableau[index],line->t - tableau[index]->attente_file);
				break;
			default : break;
		}
		free(line);
	}
}*/