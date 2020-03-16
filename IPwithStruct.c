#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct{
    char type[3];
    int maxValue;
    int width;
    int length;
    unsigned char data[800][800];
}PGM;

void getValues(PGM *pgm, FILE *fp){
    
    fgets(pgm->type,3,fp);
    
    while (getc(fp) != '\n');
 
    while (getc(fp) == '#') {
        while (getc(fp) != '\n');
    }
 
    fseek(fp, -1, SEEK_CUR);

    fscanf(fp,"%d%d%d",&pgm->width,&pgm->length,&pgm->maxValue);

    int i, j, tmp;
    for(i=0;i<pgm->length;i++){
        for(j=0;j<pgm->width;j++){
            fscanf(fp,"%d",&tmp);
            pgm->data[i][j] = tmp;
        }
    }

    fclose(fp);
}

void reversePGM(PGM *input, PGM *output){

    int i, j;
    output->length = input->length;
    output->width = input->width;
    output->maxValue = input->maxValue;
    strcpy(output->type,input->type);

    for(i=0;i<input->length;i++){
        for(j = 0 ; j < input->width ; j++){
            output->data[i][j] = input->data[(input->length)-i-1][(input->width)-1-j];
        }
    }

}

void createOutputFile(PGM *pgm, FILE *fp){

    fprintf(fp,"P2\n%d %d\n%d\n",pgm->width, pgm->length, pgm->maxValue);
    
    int i, j;
    int tmp;
    for(i = 0 ; i < pgm->length ; i++){
        for( j = 0 ; j < pgm->width ; j++){
            tmp = pgm->data[i][j];
            fprintf(fp,"%d ",tmp);
        }
    }

    fclose(fp);

}

int averageOfNine(PGM *input, int i, int j){

    int average = 0;
    /*
    if(i == 0 && j == 0){
        average = (input->data[i][j] + input->data[i][j+1] + input->data[i+1][j] + input->data[i+1][j+1])/4;
    }
    else if(i == 0 && j == input->width-1){
        average = (input->data[i][j] + input->data[i+1][j] + input->data[i][j-1] + input->data[i+1][j-1])/4;
    }
    else if()
    */
    if(i > 0 && j > 0 && i < input->length-1 && j < input->width-1){
        average = (input->data[i][j] + input->data[i][j+1] + input->data[i+1][j] + input->data[i+1][j+1] + input->data[i-1][j] + input->data[i][j-1] + input->data[i-1][j-1] + input->data[i-1][j+1] + input->data[i+1][j-1])/9;
    }

    return average;
}

int averageOf25(PGM *input, int i, int j){

    int average = 0, x, y;

    if(i > 4 && j > 4 && i < input->length-5 && j < input->width-5){
        for(x = i-2; x < i+3 ; x++){
            for(y = j-2 ; y < j+3 ; y++){
                average += input->data[x][y];
            }
        }
    }

    return average/25;
}

void lowPassFilter25(PGM *input, PGM *output){

    int i, j;
    for(i = 0 ; i < input->length ; i++){
        for(j = 0 ; j < input->width ; j++){
            output->data[i][j] = averageOf25(input,i,j);
        }
    }    
}

void lowPassFilter9(PGM *input, PGM *output){

    int i, j;
    for(i = 0 ; i < input->length ; i++){
        for(j = 0 ; j < input->width ; j++){
            output->data[i][j] = averageOfNine(input,i,j);
        }
    }    
}

void edgeDetectionWithSobel(PGM *input, PGM *output){

    int i, j, G, sumX, sumY;
    for(i = 1 ; i < input->length -1 ; i++){
        for(j = 0 ; j < input->width -1 ; j++){
            sumX = input->data[i+1][j-1]*-1 + input->data[i][j-1]*-2 + input->data[i-1][j-1]*-1 + input->data[i+1][j+1] + input->data[i][j+1]*2 + input->data[i-1][j+1];
            sumY = input->data[i+1][j+1]*-1 + input->data[i+1][j]*-2 + input->data[i+1][j-1]*-1 + input->data[i-1][j-1] + input->data[i-1][j]*2 + input->data[i-1][j+1];
            G = sumX*2 + sumY*2;
            if(G > 50){
                output->data[i][j] = 0;
            }
            else{
                output->data[i][j] = 255;
            }
        }
    }

}


int main(){
    
    PGM *inputPGM;
    inputPGM = (PGM*)malloc(sizeof(PGM));

    char name[20];
    printf("Name of File (with .pgm): ");
    scanf("%s",name);
    
    FILE *inputFile = fopen(name,"r");

    if(inputFile == NULL){
        printf("File does not exist!");
        exit(0);
    }

    getValues(inputPGM,inputFile);
    printf("FOR INPUT: \nWidth: %d\nLength: %d\nMax Value: %d\nType: %s\n",inputPGM->width,inputPGM->length,inputPGM->maxValue,inputPGM->type);
    
    PGM *outputPGM;
    outputPGM = (PGM*)malloc(sizeof(PGM));

    reversePGM(inputPGM,outputPGM);
    printf("\nFOR OUTPUT \nWidth: %d\nLength: %d\nMax Value: %d\nType: %s\n",outputPGM->width,outputPGM->length,outputPGM->maxValue,outputPGM->type);

    FILE *outputFile;
    outputFile = fopen("reversed.pgm","w");
    createOutputFile(outputPGM,outputFile);
    outputFile = fopen("lowPassFiltered9.pgm","w");
    lowPassFilter9(inputPGM,outputPGM);
    createOutputFile(outputPGM,outputFile);
    lowPassFilter25(inputPGM,outputPGM);
    outputFile = fopen("lowPassFiltered25.pgm","w");
    createOutputFile(outputPGM,outputFile);

    edgeDetectionWithSobel(inputPGM,outputPGM);
    outputFile = fopen("edgeDetectionWithSobel.pgm","w");
    createOutputFile(outputPGM,outputFile);



    return 0;
}