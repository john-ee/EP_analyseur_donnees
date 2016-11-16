#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int taille = 10;
	int *tab = NULL;
	tab = malloc(taille * sizeof(int));
	int i;
	for (i=0;i<taille;i++)
		tab[i] = i;
	taille +=2;
	tab = malloc(taille * sizeof(int));
	tab[taille-2] = taille-2;
	tab[taille-1] = taille - 1;
	for (i=0;i<taille;i++)
		printf("%d ",i);
	printf("\n");
	return 0;
}