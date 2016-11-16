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
	FILE *result = NULL;
	result = fopen("flux_actifs.txt","w");

	char *ligne = NULL;
	size_t len = 0;
	ssize_t read;
	Line line;
	Parcours_Paquet *liste = NULL;
	Parcours_Paquet *tmp = NULL;
	float moyenne_trajet = 0;
	int nb_arrivees = 0, nb_departs = 0, taille_tab_fid = 0, nb_flux_actifs = 0;
	int *tab_fid = NULL;

	int i = 0;
	while((read=getline(&ligne, &len, trace)) != -1){
		//printf("%d ",i++);
		line = parse_line(ligne);
		switch(line->code)
		{
			case DEPART_SOURCE :
				liste = add_parcours_paquet(line, liste);
				nb_departs++;
				if (line->fid+1 > taille_tab_fid){
					tab_fid = malloc((line->fid+1)*sizeof(int));
					for (i=line->fid+1 - taille_tab_fid;i<line->fid;i++)
						tab_fid[i] = 0;
					taille_tab_fid = line->fid+1;
				}
				tab_fid[line->fid]++;
				break;
			case ARR_NOEUD : 
				//liste = set_duree(liste, line->pid, )
				break;
			case ARR_DEST :
				// factoriser en un parcours
				tmp = parcours(liste, line->pid);
				moyenne_trajet += (line->t - tmp->duree);
				liste = del_parcours_paquet(line->pid, liste);
				nb_arrivees++;
				tab_fid[line->fid]--;
				break;
			default : break;
		}
		for (i=0;i<taille_tab_fid;i++)
			if(tab_fid[i])
				nb_flux_actifs++;
		fprintf(result,"%f %d\n",line->t,nb_flux_actifs);
		free(line);
		nb_flux_actifs = 0;
	}

	i = 0;
	tmp = liste;
	while(tmp != NULL){
		tmp = tmp->next;
		i++;
	}
	printf("On %d paquets qui sont partis dans le réseaux\n",nb_departs);
	printf("On a %f %% pertes\n",(float)i/nb_departs*100);
	printf("La moyenne du trajet d'un paquet allant de sa source à sa destination est%f\n", moyenne_trajet/nb_arrivees);
	free_liste(liste);
	//free(tab_fid);
	fclose(trace);
	fclose(matrice);
	fclose(result);
	return 0;
}