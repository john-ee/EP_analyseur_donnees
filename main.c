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

	int i = 0;
	while((read=getline(&ligne, &len, trace)) != -1){
		line = parse_line(ligne);
		if (line->code == DEPART_SOURCE)
			liste = add_parcours_paquet(line, liste);
		else if (line->code == ARR_DEST)
			liste = del_parcours_paquet(line->pid, liste);
		printf("%d\n",i++);
	}

	i = 0;
	printf("%d\n",line->pid);
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