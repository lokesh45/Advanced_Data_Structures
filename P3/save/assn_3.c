#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<string.h> 
#include<math.h>
#include <limits.h>

struct timeval start, end; 
int comparator(const void *a, const void *b) 
{ 
   return ( *(int*)a - *(int*)b );
} 

void basic(char *inputfilename, char *outputfilename){
    FILE *inputfp, *outputfp, *fp;
    int inputbuff[1000], i=0, j=0,l,k,iter = 0;
    char fileextension[4], filename[13];

    inputfp = fopen( inputfilename, "r+b" ); 
    outputfp = fopen( outputfilename, "w+b" );

    while(1){
        if(!fread( &inputbuff, sizeof( int ), 1000, inputfp)) {
            break;
        }

        qsort(inputbuff, 1000, sizeof(int), comparator);

        snprintf(fileextension, 4, "%03d", j++);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        fp = fopen( filename, "wb" ); 
        fwrite( inputbuff, sizeof( int ), 1000, fp ); 
        fclose( fp );
    }
    int interbuffer[j][(1000/j)+1];
    FILE *filepointers[j];
    //storing intermediate buffer
    for(i=0; i< j; i++){
        snprintf(fileextension, 4, "%03d", i);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        filepointers[i] = fopen( filename, "rb" ); 
        fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
        interbuffer[i][1000/j] = 0;
    }
    int outi = 0,c;
    FILE *outfp; /* Output file stream */ 
    int outputbuffer[ 1000 ]; /* Output buffer */ 
    outfp = fopen( "sort.bin", "wb" ); 
    while(1){
        int min = INT_MAX, minindex = -1;
        
        for(k=0; k<j; k++){
            if(min > interbuffer[k][interbuffer[k][1000/j]] && interbuffer[k][1000/j] != -1 && interbuffer[k][interbuffer[k][1000/j]] != -1){
                min = interbuffer[k][interbuffer[k][1000/j]];
                minindex = k;
            }
        }
        
        if(minindex == -1){
            break;
        }
        else{
            if(interbuffer[minindex][1000/j]+1 == 1000/j){
                int re = fread( &interbuffer[minindex], sizeof( int ), 1000/j, filepointers[minindex]);
                if(re == 1000/j) {
                        interbuffer[minindex][1000/j] = 0;
                    }
                else if(re>0){
                        interbuffer[minindex][re] = -1;
                        interbuffer[minindex][1000/j] = 0;
                    }    
                else{
                    interbuffer[minindex][1000/j] = -1;
                }
            }
            else{
                interbuffer[minindex][1000/j]++;
            }
            outputbuffer[outi++] = min;
            if(outi == 1000){
                outi = 0;
                fwrite( outputbuffer, sizeof( int ), 1000, outfp ); 
            }
        }
    }
    if(outi < 1000){
    fwrite( outputbuffer, sizeof( int ), outi, outfp ); 
    }
    fclose( outfp );
}

int checksort(int num[], int len){
    int i;
    for(i=0; i<len-1; i++){
        if(num[i]>num[i+1]){
            return 1;
        }
    }
    return 0;
}

void multistep(char *inputfilename, char *outputfilename){
    FILE *inputfp, *outputfp, *fp, *outsuperfp;
    int inputbuff[1000], i=0, j=0,l,k,iter = 0;
    char fileextension[4], filename[19];

    inputfp = fopen( inputfilename, "r+b" ); 
    outputfp = fopen( outputfilename, "w+b" );

    while(1){
        if(!fread( &inputbuff, sizeof( int ), 1000, inputfp)) {
            break;
        }

        qsort(inputbuff, 1000, sizeof(int), comparator);

        snprintf(fileextension, 4, "%03d", j++);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        fp = fopen( filename, "wb" ); 
        fwrite( inputbuff, sizeof( int ), 1000, fp ); 
        fclose( fp );
    }


    //15 files
    int superfiles = 0, runfiles = j;
    while((runfiles-(superfiles*15)) > 0){
        if((runfiles-(superfiles*15)) >= 15)
            j = 15;
        else
            j = (runfiles-(superfiles*15));
        int interbuffer[j][(1000/j)+1];
        FILE *filepointers[j];
        //storing intermediate buffer
        for(i=0; i< j; i++){
            snprintf(fileextension, 4, "%03d", i+(superfiles*15));
            strcpy(filename,  "input.bin.");
            strcat(filename, fileextension);
            filepointers[i] = fopen( filename, "rb" ); 
            fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
            interbuffer[i][1000/j] = 0;
        }
        int outi = 0,c,ivalue;
        FILE *outfp; /* Output file stream */ 
        int outputbuffer[ 1000 ]; /* Output buffer */ 
        snprintf(fileextension, 4, "%03d", superfiles++);
        strcpy(filename,  "input.bin.super.");
        strcat(filename, fileextension);
        outfp = fopen( filename, "wb" );
        while(1){
            int min = INT_MAX, minindex = -1;
            
            for(k=0; k<j; k++){
                if(min > interbuffer[k][interbuffer[k][1000/j]] && interbuffer[k][1000/j] != -1 && interbuffer[k][interbuffer[k][1000/j]] != -1){
                    min = interbuffer[k][interbuffer[k][1000/j]];
                    minindex = k;
                    ivalue = interbuffer[k][1000/j];
                }
            }
            
            if(minindex == -1){
                break;
            }
            else{
                if(interbuffer[minindex][1000/j]+1 == 1000/j){
                    int re = fread( &interbuffer[minindex], sizeof( int ), 1000/j, filepointers[minindex]);
                    if(re==1000/j) {
                            interbuffer[minindex][1000/j] = 0;
                    }
                    else if(re>0){
                        interbuffer[minindex][re] = -1;
                        interbuffer[minindex][1000/j] = 0;
                    }    
                    else{
                        interbuffer[minindex][1000/j] = -1;
                    }
                }
                else{
                    interbuffer[minindex][1000/j]++;
                }
                //interbuffer[minindex][ivalue] = -1;
                outputbuffer[outi++] = min;
                if(outi == 1000){
                    outi = 0;
                    fwrite( outputbuffer, sizeof( int ), 1000, outfp ); 
                }
            }
        }
        if(outi < 1000){
        fwrite( outputbuffer, sizeof( int ), outi, outfp ); 
        }
        fclose( outfp );

    }

    
    //super files merge
    j = superfiles;
    int interbuffer[j][(1000/j)+1];
    FILE *filepointers[j];
    //storing intermediate buffer
    for(i=0; i< j; i++){
        snprintf(fileextension, 4, "%03d", i);
        strcpy(filename,  "input.bin.super.");
        strcat(filename, fileextension);
        filepointers[i] = fopen( filename, "rb" ); 
        fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
        interbuffer[i][1000/j] = 0;
    }
    int outi = 0,c;
    FILE *outfp; /* Output file stream */ 
    int outputbuffer[ 1000 ]; /* Output buffer */ 
    outfp = fopen( "sort.bin", "wb" ); 
    while(1){
        int min = INT_MAX, minindex = -1;
        
        for(k=0; k<j; k++){
            if(min > interbuffer[k][interbuffer[k][1000/j]] && interbuffer[k][1000/j] != -1 && interbuffer[k][interbuffer[k][1000/j]] != -1){
                min = interbuffer[k][interbuffer[k][1000/j]];
                minindex = k;
            }
        }
        
        if(minindex == -1){
            break;
        }
        else{
            if(interbuffer[minindex][1000/j]+1 == 1000/j){
                int re = fread( &interbuffer[minindex], sizeof( int ), 1000/j, filepointers[minindex]);
                if(re == 1000/j) {
                        interbuffer[minindex][1000/j] = 0;
                    }
                else if(re>0){
                        interbuffer[minindex][re] = -1;
                        interbuffer[minindex][1000/j] = 0;
                    }    
                else{
                    interbuffer[minindex][1000/j] = -1;
                }
            }
            else{
                interbuffer[minindex][1000/j]++;
            }
            outputbuffer[outi++] = min;
            if(outi == 1000){
                outi = 0;
                fwrite( outputbuffer, sizeof( int ), 1000, outfp ); 
            }
        }
    }
    if(outi < 1000){
    fwrite( outputbuffer, sizeof( int ), outi, outfp ); 
    }
    fclose( outfp );
    
}

void replacement(char *inputfilename, char *outputfilename){


    printf(" %s   %s",inputfilename,outputfilename);
}

int main(int argc, char *argv[]) {
    gettimeofday( &start, NULL ); 
    if(strcmp(argv[1], "--basic")==0)
        basic(argv[2], argv[3]);
    else if(strcmp(argv[1], "--multistep")==0)
        multistep(argv[2], argv[3]);
    else if(strcmp(argv[1], "--replacement")==0)
        replacement(argv[2], argv[3]);
    else
        printf("Invalid sort type");
    
    gettimeofday( &end, NULL ); 

    printf( "Time: %ld.%06ld", end.tv_sec - start.tv_sec, end.tv_sec - start.tv_usec  );

    FILE *op;
    op = fopen( "sort.bin", "r+b" );
    int inputbuff[1000],c,co=0;

    while(1){
        if(!fread( &inputbuff, sizeof( int ), 1000, op)) {
            break;
        }    
        for(c=0; c< 1000;c++){
            //printf("%d ",inputbuff[c]);
        }
        co++;
    }
    //printf("\n%d ",co);
    fclose(op);
    return 0;
}