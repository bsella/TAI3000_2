#include "limace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char const *argv[]){
    Image image;
    unsigned char** imageI;
    unsigned char** nImageI;
    Image nImage;
    int j,i,level;
    if(argc>4||argc<3){
        printf("Usage: %s image bin_image [level]\n",argv[0]);
        return 1;
    }
    level = argc==3 ? 128 : atoi(argv[3]);
    image= ImRead(argv[1]);
    if(!image){
        printf("Error reading image %s\n", argv[1]);
        return 2;
    }
    nImage=ImAlloc(BitMap,ImNbRow(image),ImNbCol(image));
    imageI=ImGetI(image);
    nImageI=ImGetI(nImage);
    for(i=0; i<ImNbRow(image); i++)
        for(j=0; j<ImNbCol(image); j++)
            nImageI[i][j]= imageI[i][j]<level ? 0 : 1;
    ImWrite(nImage,argv[2]);
    ImFree(&image);
    ImFree(&nImage);
    return 0;
}
