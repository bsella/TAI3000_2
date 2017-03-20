#include "limace.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAB_SIZE 1024
/* -------------------------------------------------------------------
 * Fonctions utiles
 -------------------------------------------------------------------  */
typedef enum {
	V4=0,
	V8=1
}TypeVoisinage;

int max(int x,int y){
	return x>y?x:y;
}
int min(int x,int y){
	return x<y?x:y;
}


void saveResult(Matrix res,const char* fileName){
	int** data=MatGetInt(res);
	int x,y;
	int labelMax=0;
	int newLabel;
	int *newLabels;

	/*find max value of labels */
	for(y=0;y<MatNbRow(res);y++)
		for(x=0;x<MatNbCol(res);x++)
			labelMax=max(data[y][x],labelMax);

	labelMax++;
	newLabels=(int*)malloc(sizeof(int)*labelMax);
	for(x=0;x<labelMax;x++){
		newLabels[x]=-1;
	}
	/*create continious labels*/
	newLabels[0]=0;
	newLabel=1;

	for(y=0;y<MatNbRow(res);y++){
		for(x=0;x<MatNbCol(res);x++){
			if(newLabels[data[y][x]]==-1){
				newLabels[data[y][x]]=newLabel;
				newLabel++;
			}
		}
	}
	for(y=0;y<MatNbRow(res);y++){
		for(x=0;x<MatNbCol(res);x++){
			data[y][x]=newLabels[data[y][x]];
		}
	}

	/*save result as image*/
	{
		Image image=ImCAlloc(Color,MatNbRow(res),MatNbCol(res));
		unsigned char** red=ImGetR(image);
		unsigned char** green=ImGetG(image);
		unsigned char** blue=ImGetB(image);
		long step=(256*256*256)/newLabel;
		for(y=0;y<MatNbRow(res);y++){
			for(x=0;x<MatNbCol(res);x++){
				int etiquette=data[y][x];
				long idxColor=step*etiquette;
				blue[y][x]=idxColor/(256*256);
				idxColor=idxColor%(256*256);
				green[y][x]=idxColor/256;
				red[y][x]=idxColor%256;
			}
		}
		ImWrite(image,fileName);

		ImFree(&image);
	}
	free(newLabels);
}

/*create array to save equivalence*/
int* creerTableau(){
	int i;
	int* tab=(int*)malloc(sizeof(int)*TAB_SIZE);

	for( i=0;i<TAB_SIZE;i++)
		tab[i] = -1;

	return tab;
}

/* add label and increment next label value*/
void ajoutEtiquette(int* equivalences, int* nbEtiquettes){
	equivalences[*nbEtiquettes]=*nbEtiquettes;
	*nbEtiquettes=(*nbEtiquettes)+1;
}


/* -------------------------------------------------------------------
 * To do
 -------------------------------------------------------------------  */

Matrix coordonneesVoisins(Image image,int u,int v, TypeVoisinage voisinage){
	Matrix coords;
	int row=ImNbRow(image)-1, col=ImNbCol(image)-1;
	int **m;
	int i,size;
	if(voisinage==V4){
		size=4;
		if(u==0)size--;
		if(v==0)size--;
		if(u==row)size--;
		if(v==col)size--;
	}else{
		size=8;
		for(i=0;i<9;i++)
			if(((i%3)-1)+u<0 || ((i%3)-1)+u>row || ((i/3)-1)+v<0 || ((i/3)-1)+v>col)
				size--;
	}
	coords=MatCAlloc(Int,size,2);
	m=MatGetInt(coords);
	i=0;
	if(u>0){
		m[i][0]=u-1;
		m[i][1]=v;
		i++;
	}
	if(v>0){
		m[i][0]=u;
		m[i][1]=v-1;
		i++;
	}
	if(u<row){
		m[i][0]=u+1;
		m[i][1]=v;
		i++;
	}
	if(v<col){
		m[i][0]=u;
		m[i][1]=v+1;
		i++;
	}
	if(voisinage==V8){
		if(u<row&&v<col){
			m[i][0]=u+1;
			m[i][1]=v+1;
			i++;
		}
		if(u<row&&v>0){
			m[i][0]=u+1;
			m[i][1]=v-1;
			i++;
		}
		if(u>0&&v<col){
			m[i][0]=u-1;
			m[i][1]=v+1;
			i++;
		}
		if(u>0&&v>0){
			m[i][0]=u-1;
			m[i][1]=v-1;
		}
	}
	return coords;
}

int findCC(int* equivalences, int nbEtiquettes, int etiquette){
	if(equivalences[etiquette]==etiquette)
		return etiquette;
	return findCC(equivalences,nbEtiquettes,equivalences[etiquette]);
}


void unionCC(int* equivalences, int nbEtiquettes, int etiquette1, int etiquette2){
	int ref1=findCC(equivalences,nbEtiquettes,etiquette1);
	int ref2=findCC(equivalences,nbEtiquettes,etiquette2);
	equivalences[ref2]=ref1;
}

int main(int argc, char *argv[]){
	Image image;
	unsigned char** imageI;
	int nbEtiquettes,i,j;
	int* eq;
	int** m;
	Matrix res;
	if(argc!=4){
		printf("Usage: %s 4|8 image res\n",argv[0]);
		return 1;
	}
	if(atoi(argv[1])!=4 && atoi(argv[1])!=8){
		printf("Usage: %s 4|8 image res\n",argv[0]);
		return 1;
	}
	image= ImRead(argv[2]);
	if(!image){
		printf("Error reading image %s\n", argv[2]);
		return 2;
	}
	eq=creerTableau();
	imageI=ImGetI(image);
	res=MatCAlloc(Int,ImNbRow(image),ImNbCol(image));
	m=MatGetInt(res);

	nbEtiquettes=0;
	ajoutEtiquette(eq, &nbEtiquettes);
	for(i=0; i<ImNbRow(image); i++)
		for(j=0; j<ImNbCol(image); j++)
			if(imageI[i][j]!=0){
				Matrix voisins=coordonneesVoisins(image,i,j,atoi(argv[1])==4?V4:V8);
				int**v=MatGetInt(voisins);
				int x, etiq=-1;
				for(x=0; x<MatNbRow(voisins); x++)
					if(m[v[x][0]][v[x][1]]!=0){
						etiq=x; break;
					}
				if(etiq==-1){
					m[i][j]=nbEtiquettes;
					ajoutEtiquette(eq,&nbEtiquettes);
				}else{
					int etiquette=m[v[etiq][0]][v[etiq][1]];
					for(x=0; x<MatNbRow(voisins); x++)
						if(m[v[x][0]][v[x][1]]!=0 && m[v[x][0]][v[x][1]]!=etiquette){
							etiq=-1; break;
						}
					if(etiq==-1){
						m[i][j]=etiquette;
						for(x=0; x<MatNbRow(voisins); x++)
							unionCC(eq,nbEtiquettes,etiquette,m[v[x][0]][v[x][1]]);
					}else
						m[i][j]=etiquette;
				}
			}
	for(i=0; i<ImNbRow(image); i++)
		for(j=0; j<ImNbCol(image); j++)
			if(imageI[i][j]!=0)
				m[i][j]=findCC(eq,nbEtiquettes,m[i][j]);
	saveResult(res,argv[3]);
	return 0;
}
