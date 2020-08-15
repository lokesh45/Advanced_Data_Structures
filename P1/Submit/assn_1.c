#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<string.h> 
#include<math.h>

struct timeval start, end;  
FILE *seekfp, *keyfp;
int k1, k2, k3,i,j,flag,seekfilelength,keyfilelength,low,high,mid;
int *k,*s,*hit;

void mem_linear(){
    k = (int*)malloc(keyfilelength * sizeof(int));
    j=0;
    fseek(keyfp, 0, SEEK_SET);
    while(1){
        if(!fread( &k1, sizeof( int ), 1, keyfp)) {
            break;
        }
        k[j] = k1; 
        //printf("%d: %d\n",j,k[j]);
        j++;
    }
    hit = (int*)malloc(seekfilelength * sizeof(int));
    for(i=0; i<seekfilelength; i++){
        flag = 0;
        for(j=0; j<keyfilelength; j++){
            if(s[i] == k[j]){
                flag = 1;
                break;
            }
        }
        if(flag == 1){
            hit[i] = 1;
            printf( "%12d: Yes\n", s[i] );
        }
        else{
            hit[i] = 0;
            printf( "%12d: No\n", s[i] );
        }
        
    }
}
void mem_binary(){
    k = (int*)malloc(keyfilelength * sizeof(int));
    j=0;
    fseek(keyfp, 0, SEEK_SET);
    while(1){
        if(!fread( &k1, sizeof( int ), 1, keyfp)) {
            break;
        }
        k[j] = k1; 
        j++;
    }
    hit = (int*)malloc(seekfilelength * sizeof(int));
    for(i=0; i<seekfilelength; i++){
        flag = 0;
        low = 0;
        high = keyfilelength-1;
        while(low <= high){
            mid = (low + high) / 2;
            if(k[mid] == s[i]){
                flag = 1;
                break;
            }
            else if(k[mid] > s[i]){
                high = mid -1;
            }
            else{
                low = mid + 1;
            }
        }
        if(flag == 1){
            hit[i] = 1;
            printf( "%12d: Yes\n", s[i] );
        }
        else{
            hit[i] = 0;
            printf( "%12d: No\n", s[i] );
        }
        
    }
}
void disk_linear(){
    hit = (int*)malloc(seekfilelength * sizeof(int));
    for(i=0; i<seekfilelength; i++){
        flag = 0;
        fseek(keyfp, 0, SEEK_SET);
        while(1){
            if(!fread( &k1, sizeof( int ), 1, keyfp)) {
                break;
            }
            if(s[i] == k1){
                flag = 1;
                break;
            }
        }
        if(flag == 1){
            hit[i] = 1;
            printf( "%12d: Yes\n", s[i] );
        }
        else{
            hit[i] = 0;
            printf( "%12d: No\n", s[i] );
        }
        
    }
}
void disk_binary(){
    hit = (int*)malloc(seekfilelength * sizeof(int));
    for(i=0; i<seekfilelength; i++){
        flag = 0;
        fseek(keyfp, 0, SEEK_SET);
        low = 0;
        high = keyfilelength-1;
        while(low <= high){
            mid = (low + high) / 2;
            fseek(keyfp, mid * sizeof( int ), SEEK_SET);
            fread( &k1, sizeof( int ), 1, keyfp);
            if(k1 == s[i]){
                flag = 1;
                break;
            }
            else if(k1 > s[i]){
                high = mid -1;
            }
            else{
                low = mid + 1;
            }
        }
        if(flag == 1){
            hit[i] = 1;
            printf( "%12d: Yes\n", s[i] );
        }
        else{
            hit[i] = 0;
            printf( "%12d: No\n", s[i] );
        }
        
    }
}

int main( int argc, char *argv[] ){
    seekfp = fopen( argv[3], "rb" ); 
    keyfp = fopen( argv[2], "rb" );
    fseek(seekfp, 0, SEEK_END);  
    seekfilelength = ftell(seekfp)/4; 
    fseek(seekfp, 0, SEEK_SET);
    fseek(keyfp, 0, SEEK_END);  
    keyfilelength = ftell(keyfp)/4; 
    fseek(keyfp, 0, SEEK_SET);
    //printf("%d %d",seekfilelength,keyfilelength);
    //reading seek file into s array
    s = (int*)malloc(seekfilelength * sizeof(int));
    j=0;
    fseek(seekfp, 0, SEEK_SET);
    while(1){
        if(!fread( &k1, sizeof( int ), 1, seekfp)) {
            break;
        }
        s[j] = k1; 
        j++;
    }

    gettimeofday( &start, NULL ); 

    if(strcmp(argv[1], "--mem-lin") == 0){
        mem_linear();
    }
    else if(strcmp(argv[1], "--mem-bin") == 0){
        mem_binary();
    }
    else if(strcmp(argv[1], "--disk-lin") == 0){
        disk_linear();
    }
    else if(strcmp(argv[1], "--disk-bin") == 0){
        disk_binary();
        //printf("disk binary");
    }
    else{
        printf( "Invalid search type");
    }

    gettimeofday( &end, NULL ); 
    printf( "Time: %ld.%06ld\n", end.tv_sec - start.tv_sec, abs(end.tv_usec - start.tv_usec) );
    return 0;
}
