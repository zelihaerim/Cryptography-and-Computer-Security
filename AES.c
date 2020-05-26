#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <getopt.h> /* for usage control */
#define MAX_ROW_SIZE 1024
#define MAX_COLUMN_SIZE 128/* the text must have maximum 128 char length */

/*********************/
/* compile command: gcc -Wall -ansi -pedantic-errors filename.c -o exe -lm and run : ./exe */
/*********************/

int asciiTable[128];
static const uint8_t myKey[4][4]={{0x54,0x68,0x61,0x74},{0x73,0x20,0x6D,0x79},{0x20,0x4B,0x75,0x6E},{0x67,0x20,0x46,0x75}};/*Thats my Kung Fu */

uint8_t roundKeys[4][44];
static const uint8_t RC[10]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36};
static const uint8_t circulantMD5[4][4]={{0x02,0x03,0x01,0x01},{0x01,0x02,0x03,0x01},{0x01,0x01,0x02,0x03},{0x03,0x01,0x01,0x02}};
static const uint8_t TersMD5[4][4]={{0x0e,0x0b,0x0d,0x09},{0x09,0x0e,0x0b,0x0d},{0x0d,0x09,0x0e,0x0b},{0x0b,0x0d,0x09,0x0e}};

static const uint8_t sbox[16][16] = {
  /* 0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F  */
  {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
  {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
  {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
  {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
  {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
  {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
  {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
  {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
  {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
  {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
  {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
  {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
  {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
  {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
  {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
  {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}};
static const uint8_t rsbox[16][16] = {
  {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
  {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
  {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
  {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
  {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
  {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
  {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
  {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
  {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
  {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
  {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
  {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
  {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
  {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
  {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
    {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}};
int argcArgv(int argc, char* const argv[]);
void fillAsciiTable();
void encryptLine(char dataToBeRead[],int size,FILE *writeFilePointer);
void DecryptLine(char dataToBeRead[],int size,FILE *writeFilePointer2);
void findSBoxIndexes(int character,int* x,int* y);
void findSBoxIndexesHex(uint8_t hex,int* x,int* y);
void TersmixColums(uint8_t mySbox[4][4]);
void shiftOneRightCircular(uint8_t array[4],int size);
void shiftOneLeftCircular(uint8_t aray[4],int size);
void shiftRows(uint8_t mySbox[4][4]);
void RightshiftRows(uint8_t mySbox[4][4]);
void fillRoundKeys(uint8_t roundKeys[4][44]);
void mixColums(uint8_t mySbox[4][4]);
const char *plainTextPath,*cipherTextPath;
int main(int argc, char* const argv[])
{
    int i=0;
    FILE *readFilePointer,/**filePointer2,*/*writeFilePointer,*writeFilePointer2;  /* Declare the file pointer  */
    char dataToBeRead[MAX_ROW_SIZE]; /* Declare the variable for the data to be read from file  */
    if (argcArgv(argc,argv)==0) /* if given usage is not compatible with the program, end program */
    	return 0;
    fillAsciiTable();
    fillRoundKeys(roundKeys);
    readFilePointer = fopen(plainTextPath, "r") ; /* plain text */
    writeFilePointer=fopen(cipherTextPath,"w");    /* encrypted text */
/*    writeFilePointer2=fopen("text3.txt","w");   /* decrypted text */
    if ( writeFilePointer == NULL ){
    	fprintf(stderr, "%s\n","cipherTextPath failed to open.Exitting program...");
        return 0;
    }
/*  if ( writeFilePointer2 == NULL ){
    	fprintf(stderr, "%s\n","text2.txt file failed to open.Exitting program...");
        return 0;
    }
*/
    /* Check if this readFilePointer is null which maybe if the file does not exist  */ 
    if ( readFilePointer == NULL ){
        printf( "plainTextPath failed to open.Exitting program..."); 
        return 0;
    }
    else{
        char ch;
        while(!feof(readFilePointer)){
            ch=fgetc(readFilePointer);
            dataToBeRead[i]=ch;
            i++;
            if (ch=='\n'){
                dataToBeRead[i-1]='\0';
                encryptLine(dataToBeRead,MAX_ROW_SIZE,writeFilePointer);
                i=0;
            }
        }
        dataToBeRead[i-1]='\0'; /* last line from plain text */
        encryptLine(dataToBeRead,MAX_ROW_SIZE,writeFilePointer);
    } 
    fclose(writeFilePointer); /* Closing the file using fclose()  */
    fclose(readFilePointer); /* Closing the file using fclose() */
  /*  fclose(writeFilePointer2); */
    /* lets decrypt file */
   /* filePointer2 = fopen("text2.txt", "r") ; 
    if ( filePointer2 == NULL ){
        printf( "text.txt file failed to open.Exitting program..."); 
        return 0;
    }
    else{
        char ch;
        while(!feof(filePointer2)){
            ch=fgetc(filePointer2);
            dataToBeRead[i]=ch;
            i++;
            if (ch=='\n'){
                dataToBeRead[i-1]='\0';
                DecryptLine(dataToBeRead,MAX_ROW_SIZE,writeFilePointer2);
                i=0;
            }
        }
        dataToBeRead[i-1]='\0';
        DecryptLine(dataToBeRead,MAX_ROW_SIZE,writeFilePointer2);
    }
	*/
    return 0;
}
/*
void DecryptLine(char dataToBeRead[],int size,FILE *writeFilePointer2){
    char cipherText[MAX_COLUMN_SIZE];
    char firstPlainSBox[4][4];
    uint8_t newPlainSBox[4][4];
    int i,j,k,x,y,round=9,until,index=0,roundColumn=43;
    for (i = 0; dataToBeRead[i]!='\0'; ++i){
        cipherText[i]=dataToBeRead[i];
    }
    cipherText[i]=='\0';
   /* for (k = 0; k <until/16 ; ++k){/* for one line encryption ,this for loop will loop num of blocks. */
/*        for (i = 0; i < 4; ++i){ /* first round, add roundkey */
/*        	for (j =roundColumn; j>roundColumn-4; j--){
               	cipherText[i]=cipherText[i]^roundKeys[i][roundColumn];
        	}
        }
        roundColumn=j;
        RightshiftRows(cipherText);

 /*   } */
void encryptLine(char dataToBeRead[],int size,FILE *writeFilePointer){
    char plainText[MAX_COLUMN_SIZE];
    char firstPlainSBox[4][4];
    uint8_t newPlainSBox[4][4];
    int i,j,k,n,x,y,roundColumn=0,remainder,until,index=0,roundNum=9;
    for (i = 0; dataToBeRead[i]!='\0'; i++)
        plainText[i]=dataToBeRead[i];
    plainText[i]=='\0';
    remainder=i%16;
    if (remainder != 0)/*line is not a multiple of 16, so i will add zero number */
    {
    	until=i+(16-remainder); /*I will start to fill the zeros in this index */
        for (j = i; j<until; j++)
            plainText[j]='0';
        plainText[j]=='\0';
    }
    else
        until=i;
    for (k = 0; k <until/16 ; ++k){/* for one line encryption ,this for loop will loop num of blocks. */
        for (i = 0; i < 4; ++i,index++) /* first round, add roundkey */
        	for (j = 0; j < 4; ++j)
               	plainText[index]=plainText[index]^roundKeys[i][j];
        index=k*16;
	    /* Byte Substition is starting */
	    for (i = 0; i < 4; ++i){
	        for (j = 0; j < 4; ++j){
	            x=0,y=0;
	            firstPlainSBox[i][j]=plainText[index++];
	            findSBoxIndexes(firstPlainSBox[i][j],&x,&y);
	            newPlainSBox[i][j]=sbox[x][y];
	        }
	    }
	    /*end of Byte Substition */
	    shiftRows(newPlainSBox);
	    mixColums(newPlainSBox);
        for (n = 0; n < roundNum; ++n)/* loop 9 times */
        {
        	for (i = 0; i < 4; ++i)
	            for (j = roundColumn+4; j < roundColumn+8; ++j)
	                newPlainSBox[i][j]=newPlainSBox[i][j]^roundKeys[i][j];
        	roundColumn+=4;
        	index=k*16;
	        /* Byte Substition is starting */
	        for (i = 0; i < 4; ++i){
	            for (j = 0; j < 4; ++j){
	                x=0,y=0;
	                firstPlainSBox[i][j]=plainText[index++];
	                findSBoxIndexes(firstPlainSBox[i][j],&x,&y);
	                newPlainSBox[i][j]=sbox[x][y];
	            }
	        }
	        /*end of Byte Substition */
	        shiftRows(newPlainSBox);
	        mixColums(newPlainSBox);
        }       
        index=k*16;
        /* Final Round, byte substition is starting,final round has not mixcolumn */
	    for (i = 0; i < 4; ++i){
	        for (j = 0; j < 4; ++j){
	            x=0,y=0;
	            firstPlainSBox[i][j]=plainText[index++];
	            findSBoxIndexes(firstPlainSBox[i][j],&x,&y);
	            newPlainSBox[i][j]=sbox[x][y];
	        }
	    }
	    /*end of Byte Substition */
	    shiftRows(newPlainSBox);
	    /* add round key */
	    for (i = 0; i < 4; ++i)
	        for (j = roundColumn+4; j < roundColumn+8; ++j)
	            newPlainSBox[i][j]=newPlainSBox[i][j]^roundKeys[i][j];
        /* write encrypted text  to the text2.txt file*/
        for (i = 0; i < 3; ++i)
            fprintf(writeFilePointer, "%s",newPlainSBox[i]);
        fprintf(writeFilePointer, "%s\n",newPlainSBox[3]);
       	roundColumn=0;
       	index=(k+1)*16;
    }  
}/*end of function */
void mixColums(uint8_t mySbox[4][4]){
    uint8_t resultColumn[4];
    uint8_t column[4];
    int i,j,z,l,k,index=0,sum=0;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
            column[j] = mySbox[j][i];
        for (k = 0; k < 4; ++k)
        {
            for (l = 0; l < 4; ++l)
                sum+=(circulantMD5[k][l]*column[l]);
            resultColumn[index]=sum;
            index++;
            sum=0;
        }
        index=0;
        sum=0;
        for (z = 0; z <4; ++z)
            mySbox[z][i]=resultColumn[z];
    }
}
void TersmixColums(uint8_t mySbox[4][4]){/* for decrypting */
    uint8_t resultColumn[4];
    uint8_t column[4];
    int i,j,k,l,z,index=0,sum=0;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
            column[j] = mySbox[j][i];
        for (k = 0; k < 4; ++k)
        {
            for (l = 0; l < 4; ++l)
                sum+=(TersMD5[k][l]*column[l]);
            resultColumn[index]=sum;
            index++;
            sum=0;
        }
        index=0;
        sum=0;
        for (z = 0; z <4; ++z)
            mySbox[z][i]=resultColumn[z];
    }
}
void RightshiftRows(uint8_t mySbox[4][4]){
	int i,j;
    for (i = 1; i < 4; ++i)
        for (j = 0; j < i; j++)
            shiftOneRightCircular(mySbox[i],4);
}
void shiftRows(uint8_t mySbox[4][4]){
	int i,j;
    for (i = 1; i < 4; ++i)
        for (j = 0; j < i; j++)
            shiftOneLeftCircular(mySbox[i],4);
}
void shiftOneLeftCircular(uint8_t array[4],int size){
    uint8_t temp;
    int i;
    /* shifting array elements */
    temp=array[0];
    for(i=0;i<size-1;i++)
    {
        array[i]=array[i+1];
    }
    array[size-1]=temp;
}
void fillAsciiTable(){
	int i;
    for (i = 0; i < 128; ++i)
        asciiTable[i]=i;
}
void findSBoxIndexes(int character,int* x,int* y){
    int temp[8],binary[8];
    int i,j,e,index;
    *x=0,*y=0;
    for (i = 0; i < 8; ++i){
        temp[i]=0;
        binary[i]=0;
    }
    i = 0; 
    while (character > 0) { 
        temp[i] = character % 2; /* storing remainder in temp array */
        character = character / 2; 
        i++; 
    }
    /* printing temp array in reverse order  */
    index=8-i;
    for (j = i - 1; j >= 0; j--){
        binary[index]=temp[j]; 
        index++;
    }
    e=7;
    for (i = 0; i < 8; i++)
    {
        if (i<4)
            *x =*x+(binary[i]*(pow(2,i)));
        else
            *y =*y+(binary[e--]*(pow(2,(i-4))));
    }
}
void findSBoxIndexesHex(uint8_t hex,int* x,int* y){
    *x=hex/0x10;
    *y=hex%0x10;
}
void fillRoundKeys(uint8_t roundKeys[4][44]){
    int columnIndex2=3,RCIndex=0,columnIndex=4,i,j,z,x,y;
    uint8_t column[4];/*round 0 */
    for (i = 0; i <4 ; ++i)
        for (j = 0; j < 4; ++j)
            roundKeys[i][j]=myKey[j][i];
    for (z = 0; z <10 ; ++z) /*round 1-10 */
    {
        for (i = 0; i < 4; ++i)
            column[i]=roundKeys[i][columnIndex2];
        shiftOneLeftCircular(column,4);
        for (i = 0; i < 4; ++i){
            x=0,y=0;
            findSBoxIndexesHex(column[i],&x,&y);
            column[i]=sbox[x][y];
        }
        column[0]=column[0]^RC[RCIndex];
        for (i = 0; i < 4; ++i){
            column[i]=column[i]^roundKeys[i][columnIndex2-3];
            roundKeys[i][columnIndex]=column[i];
        }
        for (j = 0; j < 3; ++j)
        {
            columnIndex++;
            columnIndex2++;
            for (i = 0; i < 4; ++i){
                column[i]=column[i]^roundKeys[i][columnIndex2-3];
                roundKeys[i][columnIndex]=column[i];
            }
        }
        columnIndex++;
        columnIndex2++;
    }
}
void shiftOneRightCircular(uint8_t array[4],int size){
    uint8_t temp;
    int i;
    /* shifting array elements */
    temp=array[size-1];
    for(i=size-1;i>0;i--)
    {
        array[i]=array[i-1];
    }
    array[0]=temp;
}
int argcArgv(int argc, char* const argv[]){
	const char* optstring="p:c:";
	int c,pflag=0,cflag=0; /* "Usage: %s \n", "./exe -p plainTextPath -c cipherTextPath" */
	if (argc < 5 || argc > 5)
    {
    	fprintf(stderr, "Usage: %s \n", "./exe -p plainTextPath -c cipherTextPath");
      	return 0;
   	}
   	else if (argc == 5)
   	{
    	while((c=getopt(argc,argv,optstring)) != -1){
			switch(c){
				case 'p':
					pflag=1;
					plainTextPath=optarg;
					break;
				case 'c':
					cflag=1;
					cipherTextPath=optarg;
					break;
				
				case '?':
					if(optopt == 'p' || optopt == 'c'){
						fprintf(stderr, "Option -%c needs argument\n",optopt);
	   					fprintf(stderr, "Usage: %s \n", "./exe -p plainTextPath -c cipherTextPath");
						return 0;
					}
					else{
						fprintf(stderr, "Unknown option -%c. \n",optopt);
	   					fprintf(stderr, "Usage: %s \n", "./exe -p plainTextPath -c cipherTextPath");
	   					return 0;
					}
					break;
				default: /* generally it will not enter this condition */
					fprintf(stderr,"getopt");
	   				fprintf(stderr, "Usage: %s \n", "./exe -p plainTextPath -c cipherTextPath");
	   				return 0;
	   				break;
			}
		}
  	}
  	return 1;
}
