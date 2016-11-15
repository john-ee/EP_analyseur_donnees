#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
	line->tos = atoi(strtok(NULL," "));
	line->bif = atoi(strtok(NULL," "));

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
	printf("%f %d %d %d %d ",l->t,(int)l->code,l->pid,l->fid,l->tos);
	if(l->code==DESTRUCTION)
		printf("%d ",l->bif);
	printf("%d %d %d\n",l->source,l->destination,l->position);
}

Parcours_Paquet new_parcours_paquet(Line l)
{
	Parcours_Paquet parcours = malloc(sizeof(struct Parcours_Paquet));
	parcours->pid = l->pid;
	parcours->source = l->source;
	parcours->destination = l->destination;
	parcours->duree = 0;
	parcours->attente_file = 0;
	parcours->arrivee = 0;
	parcours->next = NULL;
	return parcours;
}

void add_parcours_paquet(Line l, Parcours_Paquet *liste)
{
	while(liste!=NULL && liste->pid == l->pid){
		liste = liste->next;
	}
	if (liste == NULL)
		liste = new_parcours_paquet(l);
}

void set_duree(Parcours_Paquet parcours, float t)
{
	parcours->duree = t;
}

void set_attente(Parcours_Paquet parcours, float t)
{
	parcours->attente_file = t;
}

void switch_arrivee(Parcours_Paquet parcours)
{
	if(parcours->arrivee)
		parcours->arrivee = 0;
	else
		parcours->arrivee = 1;
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

void compteur_paquets(FILE *fichier, int tab[3])
{
	char *ligne = NULL;
	size_t len = 0;
	Line line;
	int departs = 0;
	int pid_min = 0, pid_max = 0;

	while(getline(&ligne, &len, fichier) != -1){
		line = parse_line(ligne);

		switch(line->code){
			case DEPART_SOURCE : departs++;
				break;
			default : break;
		}

		if (line->pid < pid_min)
			pid_min = line->pid;
		else if (line->pid > pid_max)
			pid_max = line->pid;

		free(line);
	}
	tab[0] = departs; tab[1] = pid_min; tab[2] = pid_max;
}


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
}