#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "line.h"


int main(int argc, char **argv)
{
	if(argc != 3){
		fprintf(stderr,"./analyse <fichier de trace> <fichier de la matrice>\n");
		exit(1);
	}

	FILE *trace = fopen(argv[1],"r");
	FILE *matrice = fopen(argv[2],"r");

	char *ligne = NULL;
	size_t len = 0;
	ssize_t read;
	Line line;
	Parcours_Paquet *liste = NULL;
	float moyenne_trajet = 0;

	int i = 0;
	while((read=getline(&ligne, &len, trace)) != -1){
		line = parse_line(ligne);
		switch(line->code)
		{
			case DEPART_SOURCE :
				liste = add_parcours_paquet(line, liste);

				break;
			case ARR_NOEUD : 
				//liste = set_duree(liste, line->pid, )
				break;
			case ARR_DEST :
				// factoriser en un parcours
				moyenne_trajet += (line->t - get_duree(liste,line->pid));
				liste = del_parcours_paquet(line->pid, liste);
				break;
			default : break;
		}
		free(line);
	}

	i = 0;
	while(liste != NULL){
		liste = liste->next;
		i++;
	}
	printf("On a %d elts\n",i);
	free_liste(liste);
	fclose(trace);
	fclose(matrice);
	return 0;
}