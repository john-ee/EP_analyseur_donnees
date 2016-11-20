/**
 * \file main.c
 * \brief Permet de lancer l'analyseur. Voir le rapport pour un savoir plus 
 * \author John-Nathan Hill
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "process_data.h"

// BUFSIZE est utilisé pour le buffer, afin de minimiser le nombre d'écriture dans le fichier
#define BUFSIZE 300

// STRINGSIZE sert pour les chaînes de caractères que l'on ajoute au buffer
#define STRINGSIZE 50

// SAUTER_LIGNE permet de sauter 1 lignes sur SAUTER_LIGNE à écrire dans le buffer pour éviter de surcharger le graphe
// En sautant des lgines, on perd en informations mais on gagne en lisibilité et précision du graphe
// Note :  SAUTER_LIGNE = 1, on ne saute pas de lignes
#define SAUTER_LIGNE 1
// /!\ : Lorsque SAUTER_LIGNE > 1, le graphe s'arrête à t = 10.


int main(int argc, char **argv)
{
	if(argc != 4){
		fprintf(stderr,"./analyse <fichier de trace> <fichier de la matrice> 0 ou 1 <0: sans graphe / 1:avec graphe\n");
		exit(1);
	}

	// Initialisation des pointeurs de fichiers que l'on a mis en entrée
	FILE *trace = fopen(argv[1],"r");
	FILE *matrice = fopen(argv[2],"r");
	int plot_graph = atoi(argv[3]);
	FILE *result = NULL;
	if (plot_graph)
		result = fopen("flux_actifs.txt","w");

	// Initialisations des entiers et pointeurs d'entiers que l'on utilisera
	int i = 0;
	int nb_departs = 0, nb_arrivees = 0, nb_destruction = 0, nb_arr_inter = 0, nb_depart_file = 0;
	int  taille_tab_fid = 0, nb_flux_actifs = 0;
	int nb_char = 0;
	// Ce tableau permettra de compter le nb de paquets dans un flux donc reperer les flux actifs à la fin de chaque boucle while. Il sera alloué dynamiquement
	int *tab_fid = NULL;
	// Grace au fichier matrice, on peut trouver le nombre de noeuds
	int nb_noeuds = compteur_noeuds(matrice);
	// Dans ce tableau, on compte les paquets perdues et on saura où les trouver. La case i correspond au noeuds i
	int paquets_perdus_par_noeuds[nb_noeuds];
	for (i=0;i<nb_noeuds;i++)
		paquets_perdus_par_noeuds[i] = 0;

	// Initialisations des chaines de caractères et des variables utilisés pour les remplir
	char *ligne = NULL;
	char xy[BUFSIZE+STRINGSIZE];
	char str_tmp[STRINGSIZE];
	size_t len = 0;
	ssize_t read;

	// Initialisations des objets à manipuler
	Line line;
	Parcours_Paquet *liste = NULL;
	Parcours_Paquet *tmp = NULL;
	Calcul_Moy moy_trajet;
	moy_trajet.moy = 0; moy_trajet.q = 0; moy_trajet.k = 0;
	Calcul_Moy moy_attente;
	moy_attente.moy = 0; moy_attente.q = 0; moy_attente.k = 0;
	

	i = 0;
	// A chaque itération de while, on lit une ligne du fichier, tant que l'on n'a pas encore atteint la fin
	while((read=getline(&ligne, &len, trace)) != -1)
	{
		line = parse_line(ligne);
		// Le traitement de la ligne dépend du code
		switch(line->code)
		{
			case DEPART_SOURCE :
			// Le paquet est neuf et vient d'être créé, donc on l'ajoute et on incrémente nb_departs
				liste = add_parcours_paquet(line, liste);
				nb_departs++;
				// Cette condition permet d'agrandir la longeur du tableau. La variable taille_tab_fid permet de connaitre sa taille a tout instant
				if (line->fid+1 > taille_tab_fid){
					tab_fid = malloc((line->fid+1)*sizeof(int));
					for (i=line->fid+1 - taille_tab_fid;i<line->fid+1;i++)
						tab_fid[i] = 0;
					taille_tab_fid = line->fid+1;
				}
				tab_fid[line->fid]++;
				break;

			case ARR_NOEUD : 
				liste = substract_attente(liste, line->pid, line->t);
				nb_arr_inter++;
				break;

			case DEPART_FILE : 
				liste = add_attente(liste, line->pid, line->t);
				nb_depart_file++;
				break;

			case ARR_DEST :
			// Arriver a destination, on desincremente la case du tableau correspondant, on met a jour les moyennes et on libere l'espace alloué par le paquet dans la liste
				tmp = parcours(liste, line->pid);
				tmp->attente_file += line->t;
				moy_trajet = update_Calcul_Moy(moy_trajet, line->t - tmp->duree);
				moy_attente = update_Calcul_Moy(moy_attente, tmp->attente_file);
				liste = del_parcours_paquet(line->pid, liste);
				nb_arrivees++;
				tab_fid[line->fid]--;
				break;

			case DESTRUCTION :
			// operation similaire à ARR_DEST avec pour difference on incremente la case correspondant au compteur de paquets
				tmp = parcours(liste, line->pid);
				tmp->attente_file += line->t;
				moy_attente = update_Calcul_Moy(moy_attente, tmp->attente_file);
				liste = del_parcours_paquet(line->pid, liste);
				tab_fid[line->fid]--;
				nb_destruction++;
				paquets_perdus_par_noeuds[line->position]++;
			default : break;
		}
		nb_flux_actifs = 0;
		for (i=0;i<taille_tab_fid;i++)
				if(tab_fid[i]>0)
					nb_flux_actifs++;

		// Dans le cas où l'utilisateur veut faire le graphe
		if(plot_graph)
		{
			if(i%SAUTER_LIGNE == 0)
			{
				nb_char += sprintf(str_tmp,"%lf\t%d\n",line->t,nb_flux_actifs);
				strcat(xy, str_tmp);
				if (nb_char > BUFSIZE){
					fputs(xy,result);
					xy[0] = '\0';
					nb_char = 0;
				}
			}
		}

		free(line);
		nb_flux_actifs = 0;
		i++;
	}

	if(plot_graph)
		fputs(xy,result);

	// On imprime sur la console les résultats du traitement de données
	printf("Nb de départ de la source %d\n", nb_departs);
	printf("Nb d'arrivées à destination %d\n", nb_arrivees);
	printf("Nb d'arrivées à un noeud intermédiaire %d\n", nb_arr_inter);
	printf("Nb de paquets détruits %d\n", nb_destruction);
	printf("Nb de départ de file d'attente %d\n", nb_depart_file);
	printf("%.2lf %% des paquets sont perdus\n",(double)nb_destruction/nb_departs*100);
	printf("Les pertes se trouvent au niveau des noeuds :\n");
	for (i=0;i<nb_noeuds;i++)
		if(paquets_perdus_par_noeuds[i])
			printf("\t%d avec %0.4lf %%\n",i, (double)paquets_perdus_par_noeuds[i]/nb_destruction*100);
	printf("La moyenne du trajet de bout en bout est %lf avec un ecart-type de %lf\n", moy_trajet.moy, get_ecart_type(moy_trajet));
	printf("La moyenne d'attente en file d'attente est %lf avec un ecart-type de %lf\n", moy_attente.moy, get_ecart_type(moy_attente));

	// On ouvre gnuplot pour tracer le graphe à partir du fichier que l'on a écrit
	if(plot_graph) 
	{	
		fclose(result);
		FILE *pipe = popen("gnuplot -persist","w");
		fprintf(pipe, "plot 'flux_actifs.txt' using 1:2 with lines title \"Nb Flux Actifs en fonction du Temps\"\n");
		fflush(pipe);
		fclose(pipe);
	}

	// On libère l'espace alloué et on ferme les fichiers ouverts
	free_liste(liste);
	free(tab_fid);
	fclose(trace);
	fclose(matrice);
	return 0;
}