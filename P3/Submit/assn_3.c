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
        int val = fread( &inputbuff, sizeof( int ), 1000, inputfp);
        if(!val) {
            break;
        }
        qsort(inputbuff, val, sizeof(int), comparator);

        snprintf(fileextension, 4, "%03d", j++);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        fp = fopen( filename, "wb" ); 
        fwrite( inputbuff, sizeof( int ), val, fp ); 
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
        int rval = fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
        if(rval == 1000/j) {
            interbuffer[i][1000/j] = 0;
        }
        else if(rval>0){
            interbuffer[i][rval] = -1;
            interbuffer[i][1000/j] = 0;
        }    
        else{
            interbuffer[i][1000/j] = -1;
       }
    }
    int outi = 0,c;
    FILE *outfp; /* Output file stream */ 
    int outputbuffer[ 1000 ]; /* Output buffer */ 
    outfp = fopen( outputfilename, "wb" ); 
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
    //outputfp = fopen( outputfilename, "w+b" );

    while(1){
        int val = fread( &inputbuff, sizeof( int ), 1000, inputfp);
        if(!val) {
            break;
        }

        qsort(inputbuff, val, sizeof(int), comparator);

        snprintf(fileextension, 4, "%03d", j++);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        fp = fopen( filename, "wb" ); 
        fwrite( inputbuff, sizeof( int ), val, fp ); 
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
            int rval = fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
            if(rval == 1000/j) {
                interbuffer[i][1000/j] = 0;
            }
            else if(rval>0){
                interbuffer[i][rval] = -1;
                interbuffer[i][1000/j] = 0;
            }    
            else{
                interbuffer[i][1000/j] = -1;
            }
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
        int ral = fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
        if(ral == 1000/j) {
            interbuffer[i][1000/j] = 0;
        }
        else if(ral>0){
            interbuffer[i][ral] = -1;
            interbuffer[i][1000/j] = 0;
        }    
        else{
            interbuffer[i][1000/j] = -1;
       }

    }
    int outi = 0,c;
    FILE *outfp; /* Output file stream */ 
    int outputbuffer[ 1000 ]; /* Output buffer */ 
    outfp = fopen( outputfilename, "wb" ); 
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

void sift(int *heapbuff, int i, int heapsize){
    int j,k,smallest,swapelement;
    while(i*2 + 1 < heapsize){
        j = i*2 + 1;
        k = j+1;
        if (k < heapsize && heapbuff[k] < heapbuff[j])
            smallest = k;
        else
            smallest = j;

        if(heapbuff[i]<heapbuff[smallest])
            return;
        //swap
        swapelement = heapbuff[i];
        heapbuff[i] = heapbuff[smallest];
        heapbuff[smallest] = swapelement;
        i = smallest;
    }
}

int heapify(int *heapbuf, int heapsize){
    int i = heapsize/2;
    while(i >= 0){
        sift(heapbuf, i, heapsize);
        i = i-1;
    }
}

void replacement(char *inputfilename, char *outputfilename){
    FILE *inputfp, *outputfp, *fp;
    int heapbuf[750], inputbuff[250],outputbuff[1000],outindex=0,bufferindex=0, flag = 0,i=0, j=0,l=0,k=0,iter = 0,heapempty=0,currentheapsize,indexfileempty=-1;
    char fileextension[4], filename[13];

    inputfp = fopen( inputfilename, "r+b" ); 
    outputfp = fopen( outputfilename, "w+b" );
    currentheapsize = 750;

    currentheapsize = fread( &heapbuf, sizeof( int ), currentheapsize, inputfp);
    int firstheapsize = currentheapsize;
    if(firstheapsize<750)
        flag = 1;
    int valr = fread( &inputbuff, sizeof( int ), 1000-currentheapsize, inputfp);
    if(valr == 1000-currentheapsize){
        bufferindex = 0;
    }
    else if (valr>0){
        bufferindex = 0;
        inputbuff[valr] = -1;
        }
    else{
        bufferindex = -1;
    }

    heapify(heapbuf, currentheapsize);

    snprintf(fileextension, 4, "%03d", j++);
    strcpy(filename,  "input.bin.");
    strcat(filename, fileextension);
    fp = fopen( filename, "wb" ); 
    //printf("enter while");
    while(1){
        if(currentheapsize <= 0){
            //heapempty = 0;
            currentheapsize = 750;
            heapify(heapbuf, currentheapsize);

            if(outindex!=1000 && outindex!=0){
                fwrite( outputbuff, sizeof( int ), outindex, fp ); 
                outindex = 0;
            }

            fclose(fp);
            snprintf(fileextension, 4, "%03d", j++);
            strcpy(filename,  "input.bin.");
            strcat(filename, fileextension);
            fp = fopen( filename, "wb" ); 
        }
        if(inputbuff[bufferindex]==-1 || bufferindex == -1)
            break;
        outputbuff[outindex++] =  heapbuf[0];
        if(outindex == 1000){
            outindex = 0;        
            fwrite( outputbuff, sizeof( int ), 1000, fp ); 
        }
        int B1 = inputbuff[bufferindex++];
        if(heapbuf[0] <= B1)
            heapbuf[0] = B1;
        else{
            heapbuf[0] = heapbuf[currentheapsize-1];
            heapbuf[currentheapsize-1] = B1;
            currentheapsize--;
        }
        if(bufferindex == 250){
            int re = fread( &inputbuff, sizeof( int ), 250, inputfp);
            if(re == 250){
                bufferindex = 0;
            }
            else if (re>0){
                bufferindex = 0;
                indexfileempty = re;
                inputbuff[re] = -1;
            }
            else{
                bufferindex = -1;
            }
        }
        heapify(heapbuf, currentheapsize);
    }
    //printf("out of while");
    if(outindex!=1000 && outindex!=0){
        fwrite( outputbuff, sizeof( int ), outindex, fp ); 
    }
    //qsort
    qsort(heapbuf, firstheapsize, sizeof(int), comparator);
    if(flag == 1)
        fwrite( &heapbuf[firstheapsize-currentheapsize], sizeof( int ), firstheapsize, fp );
    else
        fwrite( &heapbuf[firstheapsize-currentheapsize], sizeof( int ), currentheapsize, fp );
    fclose(fp);
    snprintf(fileextension, 4, "%03d", j++);
    strcpy(filename,  "input.bin.");
    strcat(filename, fileextension);
    fp = fopen( filename, "wb" ); 
    fwrite( heapbuf, sizeof( int ), 750-currentheapsize, fp );
    fclose(fp);

    //merge start
    int interbuffer[j][(1000/j)+1];
    FILE *filepointers[j];
    //storing intermediate buffer
    for(i=0; i< j; i++){
        snprintf(fileextension, 4, "%03d", i);
        strcpy(filename,  "input.bin.");
        strcat(filename, fileextension);
        filepointers[i] = fopen( filename, "rb" ); 
        int rval = fread( &interbuffer[i], sizeof( int ), 1000/j, filepointers[i]);
        if(rval == 1000/j) {
            interbuffer[i][1000/j] = 0;
        }
        else if(rval>0){
            interbuffer[i][rval] = -1;
            interbuffer[i][1000/j] = 0;
        }    
        else{
            interbuffer[i][1000/j] = -1;
       }
    }
    int outi = 0,c;
    FILE *outfp; /* Output file stream */ 
    int outputbuffer[ 1000 ]; /* Output buffer */ 
    outfp = fopen( outputfilename, "wb" ); 
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

    return 0;
}