#include "limace.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char const *argv[]){
    Image image;
    unsigned char** imageR;
    unsigned char** imageG;
    unsigned char** imageB;
    unsigned char** imageI;
    Image nImage;
    int j,i;
    if(argc!=3){
        printf("Usage: %s image gray_image\n",argv[0]);
        return 1;
    }
    image= ImRead(argv[1]);
    if(!image){
        printf("Error reading image %s\n", argv[1]);
        return 2;
    }
    nImage=ImAlloc(GrayLevel,ImNbRow(image),ImNbCol(image));
    imageR=ImGetR(image);
    imageG=ImGetG(image);
    imageB=ImGetB(image);
    imageI=ImGetI(nImage);
    for(i=0; i<ImNbRow(image); i++)
        for(j=0; j<ImNbCol(image); j++){
            imageI[i][j]=ceil((imageR[i][j]+imageG[i][j]+imageB[i][j])/3.f-.5);
        }
    ImWrite(nImage,argv[2]);
    ImFree(&image);
    ImFree(&nImage);
    return 0;
}
