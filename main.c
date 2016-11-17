#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "line.h"

#define BUFSIZE 150
#define STRINGSIZE 50
#define SAUTER_LIGNE 100

int main(int argc, char **argv)
{
	if(argc != 4){
		fprintf(stderr,"./analyse <fichier de trace> <fichier de la matrice> 0 ou 1 <0: sans graphe / 1:avec graphe\n");
		exit(1);
	}

	FILE *trace = fopen(argv[1],"r");
	FILE *matrice = fopen(argv[2],"r");
	int plot_graph = atoi(argv[3]);
	FILE *result = NULL;
	if (plot_graph)
		result = fopen("flux_actifs.txt","w");

	int i = 0;
	char *ligne = NULL;
	size_t len = 0;
	ssize_t read;
	Line line;
	Parcours_Paquet *liste = NULL;
	Parcours_Paquet *tmp = NULL;
	double moyenne_trajet = 0;
	int nb_arrivees = 0;
	int nb_departs = 0, taille_tab_fid = 0, nb_flux_actifs = 0, nb_pertes = 0;
	int *tab_fid = NULL;
	char xy[BUFSIZE+STRINGSIZE];
	char str_tmp[STRINGSIZE];

	int nb_noeuds = compteur_noeuds(matrice);
	int paquets_perdus_par_noeuds[nb_noeuds];
	for (i=0;i<nb_noeuds;i++)
		paquets_perdus_par_noeuds[i] = 0;

	i = 0;
	while((read=getline(&ligne, &len, trace)) != -1)
	{
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

			case DESTRUCTION :
				liste = del_parcours_paquet(line->pid, liste);
				tab_fid[line->fid]--;
				nb_pertes++;
				paquets_perdus_par_noeuds[line->position]++;
			default : break;
		}
		for (i=0;i<taille_tab_fid;i++)
			if(tab_fid[i])
				nb_flux_actifs++;

		if(plot_graph)
		{
			if(i%SAUTER_LIGNE == 0)
			{
				sprintf(str_tmp,"%lf\t%d\n",line->t,nb_flux_actifs);
				strcat(xy, str_tmp);
				if (sizeof(xy) > BUFSIZE){
					fputs(xy,result);
					xy[0] = '\0';
				}
			}
		}
		free(line);
		nb_flux_actifs = 0;
		i++;
	}

	if(plot_graph)
		fputs(xy,result);

	printf("On a %d paquets qui sont partis dans le réseaux\n",nb_departs);
	printf("On a %.2lf %% pertes\n",(double)nb_pertes/nb_departs*100);
	printf("Les pertes se trouvent au niveau des noeuds :\n\t");
	for (i=0;i<nb_noeuds;i++)
		if(!paquets_perdus_par_noeuds[i])
			printf("%d ",i);
	printf("\n");
	printf("La moyenne du trajet est %lf\n", moyenne_trajet/nb_arrivees);

	if(plot_graph) 
	{
		FILE *pipe = popen("gnuplot -persist","w");
		//fprintf(pipe, "set style \n");
		fprintf(pipe, "plot 'flux_actifs.txt' using 1:2 with lines title \"Nb Flux Actifs en fonction du Temps\"\n");
		fflush(pipe);
		fclose(pipe);
	}


	free_liste(liste);
	free(tab_fid);
	fclose(trace);
	fclose(matrice);
	if (plot_graph)
		fclose(result);
	return 0;
}