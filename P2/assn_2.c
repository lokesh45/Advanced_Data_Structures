#include<stdio.h>
#include<stdlib.h>
#include<string.h> 
#include<math.h>

typedef struct { int siz; /* Hole's size */ long off; /* Hole's offset in file */ } avail_S;
typedef struct { int key;/* Record's key */ long off;/* Record's offset in file */ } index_S; 
index_S prim[10000];
avail_S avail[10000];
int indexcount,availcount,fittype;
char *buf, inputline[2048],inputwords[3][2048];
FILE *fp,*indexfp,*out,*availfp; /* Input/output stream */

int qsortcompareD(const void *a,const void *b){
        int order;
        avail_S *availA=(avail_S*)a;
        avail_S *availB=(avail_S*)b;
        if(availA->siz==availB->siz)
            order = availA->off-availB->off;
        else
            order = availB->siz-availA->siz;
        return order;
}

int qsortcompareA(const void *a,const void *b){
        int order;
        avail_S *availA=(avail_S*)a;
        avail_S *availB=(avail_S*)b;
        if(availA->siz==availB->siz)
            order = availA->off-availB->off;
        else
            order = availA->siz-availB->siz;
        return order;
}

long searchAvailableSpace(int recordsize){
    long offset = -1;
    int i,j;
    avail_S newhole;
        for(i=0; i<availcount; i++){
            if(recordsize<=avail[i].siz){
                offset = avail[i].off;
                newhole.siz=avail[i].siz-recordsize;
                newhole.off=offset+sizeof(int)+strlen(inputwords[2]);
                for(j=i; j<availcount-1; j++) {
                    avail[j]=avail[j+1];
                }
                availcount--;
                if(newhole.siz>0) {
                    avail[availcount]=newhole;
                    availcount++;
                    if(fittype==2) {//bestfit
                            qsort(avail,availcount,sizeof(avail_S),qsortcompareA);
                    }
                    else if(fittype==3) {//worstfit
                            qsort(avail,availcount,sizeof(avail_S),qsortcompareD);
                    }
                }
                break;
            }
        }
    return offset;
}

int binarySearch(){
    int low = 0, high = indexcount-1, mid = indexcount/2;
    while(low<=high){
        mid = (low+high)/2;
        if(prim[mid].key == atoi(inputwords[1])){
            return mid;
        }
        else if(prim[mid].key < atoi(inputwords[1])){
            low = mid + 1;
        }
        else{
            high = mid - 1;
        }
    }
    return -1;
}

void addRecord(){
    int i;
    index_S newIndex;
    //Search if the id already exists in the primary index
    if(binarySearch()!=-1){
        printf("Record with SID=%s exists\n",inputwords[1]);
        return;
    }

    //Search the availability list for a hole that holds record and records size
    int recordsize = sizeof(int) + strlen(inputwords[2]);
    int rec_s  = strlen(inputwords[2]);
    long availspaceoffset = searchAvailableSpace(recordsize);
    if(availspaceoffset == -1){//append new record to end of student file
        fseek(fp,0,SEEK_END);
        availspaceoffset = ftell(fp);
    }
    else
        fseek(fp,availspaceoffset,SEEK_SET);
    
    fwrite(&rec_s,sizeof(int),1,fp);
    fwrite(inputwords[2],sizeof(char),strlen(inputwords[2]),fp);

    //index file record
    newIndex.off = availspaceoffset;
    newIndex.key = atoi(inputwords[1]);
    if(indexcount==0)
            prim[indexcount]=newIndex;
    else{
        int low=0,high=indexcount-1,mid=(high+low)/2;
        while(low<=high) {
                mid=(high+low)/2;
                if(prim[mid].key<newIndex.key)
                        low=mid+1;
                else
                    high=mid-1;
        }
        for(i=indexcount; low<i; --i) {
                prim[i]=prim[i-1];
        }
        prim[low]=newIndex;
    }
    indexcount++;
}

void findRecord(){

    //Search if the id exists in the primary index
    int flag = binarySearch();
    //printf("flag:%d %d %ld",flag,prim[flag].key,prim[flag].off);
    if(flag == -1){
        printf("No record with SID=%s exists\n",inputwords[1]);
    }
    else{
        int recordsize;
        char *record;
        fseek(fp, prim[flag].off, SEEK_SET);
        fread( &recordsize, sizeof( int ), 1, fp);
        //printf("recordsize: %d\n",recordsize);
        record = malloc(recordsize+1);
        //printf("record: %s\n",record);
        fread( record, 1, recordsize, fp);
        record[recordsize] = '\0';
        printf("%s\n",record);
    }

}

void delRecord(){
    int flag = binarySearch();
    if(flag == -1){
        printf("No record with SID=%s exists\n",inputwords[1]);
        return;
    }
    int recordsize,i;
    char *record;
    fseek(fp, prim[flag].off, SEEK_SET);
    fread( &recordsize, sizeof( int ), 1, fp);

    avail[availcount].siz = sizeof(int)+recordsize;
    avail[availcount].off = prim[flag].off;
    availcount++;
    if(fittype==2) {//bestfit
        qsort(avail,availcount,sizeof(avail_S),qsortcompareA);
    }
    else if(fittype==3) {//worstfit
        qsort(avail,availcount,sizeof(avail_S),qsortcompareD);
    }

    //delete entry from the primary index
    for(i=flag ; i<indexcount-1; i++){
        prim[i] = prim[i+1];
    }
    indexcount--;
}

int main( int argc, char *argv[] ){
    long rec_off; /* Record offset */ 
    int rec_siz,worditerator,written,count,i,iterator; /* Record size, in characters */ 

    if(strcmp(argv[1], "--best-fit")==0)
        fittype = 2;
    else if(strcmp(argv[1], "--worst-fit")==0)
        fittype = 3;
    else if(strcmp(argv[1], "--first-fit")==0)
        fittype = 1;
    else
        printf("Invalid FitType");

    if ( ( fp = fopen( argv[2], "r+b" ) ) != NULL ){//student.db exist, index and availability file are not empty
        indexfp = fopen( "index.bin", "r+b" ); 
        index_S indexiterator;
        while(fread(&indexiterator,sizeof(index_S),1,indexfp)) {
                prim[indexcount]=indexiterator;
                indexcount = indexcount+1;
                //printf("%d",indexiterator);
        }
        fclose(indexfp);
        availfp = fopen( "avail.bin", "r+b" ); 
        avail_S availiterator;
        while(fread(&availiterator,sizeof(index_S),1,availfp)) {
                avail[availcount]=availiterator;
                availcount = availcount+1;
        }
        fclose(availfp);
    }
    else{
        fp = fopen( argv[2], "w+b" );
    }
    //printf("%d %d %ld %d\n",prim[0].key,avail[0].off,prim[0].off,avail[0].siz);
    //printf("%d %d %ld %d",prim[0].key,prim[1].key,prim[1].off,prim[0].off);
    /*
    //dummy data
    prim[0].key=4141;
    avail[0].off=143;
    prim[0].off=314;
    avail[0].siz=12;
    indexcount++;
    availcount++;
    */
    gets(inputline);
    while(strcmp(inputline, "end") != 0 ){
        //printf("%s\n",inputline);
        worditerator =0;
        count = 0;
        for(i=0;i<=(strlen(inputline));i++)
        {
            if(inputline[i]==' '||inputline[i]=='\0')
            {
                inputwords[count][worditerator]='\0';
                count++;  
                worditerator=0;   
            }
            else
            {
                inputwords[count][worditerator]=inputline[i];
                worditerator++;
            }
        }
        if(strcmp(inputwords[0], "add") == 0 ){
            //add a record
            //printf("add\n");
            addRecord();
        }
        else if(strcmp(inputwords[0], "find") == 0 ){
            //find a record
            //printf("find\n");
            findRecord();
            //printf("find-end\n");
        }
        else if(strcmp(inputwords[0], "del") == 0 ){
            //delete a record
            //printf("del\n");
            delRecord();
        }

        gets(inputline);

    }
    //Writing index to index.bin file
    indexfp = fopen( "index.bin", "wb" ); 
    fseek(indexfp,0,SEEK_SET);
    written = fwrite(prim,sizeof(index_S),indexcount,indexfp);
    if (written == 0) {
        printf("Error during writing to index file !");
    }
    fclose(indexfp);
    //Writing index to avail.bin file
    availfp = fopen( "avail.bin", "wb" ); 
    fseek(availfp,0,SEEK_SET);
    written = fwrite(avail,sizeof(avail_S),availcount,availfp);
    if (written == 0) {
        printf("Error during writing to avail file !");
    }
    fclose(availfp);

    printf("Index:\n");
    for(i=0; i< indexcount ; i++){
        printf( "key=%d: offset=%ld\n", prim[i].key, prim[i].off );
    }
    printf("Availability:\n");
    int hole_siz=0;
    for(i=0; i< availcount ; i++){
        hole_siz = hole_siz + avail[i].siz;
        printf( "size=%d: offset=%ld\n", avail[i].siz, avail[i].off );
    }
    printf( "Number of holes: %d\n", availcount );
    printf( "Hole space: %d\n", hole_siz );
    return 0;
}