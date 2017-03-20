#include "limace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char const *argv[]){
	Image image;
	unsigned char** imageI;
	int i,j;
	int **index;
	Matrix hist=MatAlloc(Int,256,1);
	index=MatGetInt(hist);
	if(argc!=2){
		printf("Usage: %s image\n",argv[0]);
		return 1;
	}
	image= ImRead(argv[1]);
	if(!image){
		printf("Error reading image %s\n", argv[1]);
		return 2;
	}
	imageI=ImGetI(image);

	for(i=0; i<ImNbRow(image); i++)
		for(j=0; j<ImNbCol(image); j++)
			index[imageI[i][j]][0]++;
	for(i=0; i<256; i++)
		printf("%d(%d) ",i, index[i][0]);
	printf("\n");
	ImFree(&image);
	return 0;
}
