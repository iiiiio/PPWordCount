#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<pthread.h>
#include<sys/stat.h>

#include "lib/oAs.h"

/**
* @file pOpenFile.c
* @mainpage Parallel word counting using pthreads
*
* @brief This program caluclates the number of words, in a file sequentially,
* the mehtod used to read the words from the file was adapted from
* https://github.com/giuseppeangri/mpi-parallel-words-count/blob/master/src/file_reader.c#L8
* Later on pthread and MPI will be applied.
*
* @param char* argv[] - file & no. of processors input through terminal.
*/

// Test 3

int main(int args, char* argv[]){

    printf("Check point 0");
    // take file name and number of processes from  terminal input
    POSIX para; /** @file oAs.h */
    char* path = argv[1];
    // ZH Notes: atoi = char to integer
    para.p = atoi(argv[2]);
    pthread_t threads[para.p]; /** Declare threads */

    printf("Check point 1");
    // open file based on path.
    para.file_to_read = open(path, "r");  /** the function open is from the file @file oAc.c th function initialization is in @file oAc.h*/

    printf("Check point 2");
    // Get size of the file to return the size of the file by bytes.
    para.origin_part = fsize(path); /** calls fsize() function from @file oAc.c */
    // free(path);
    printf("Check point 3");
    // Split the bytes among processes.
    para.split_segments = para.origin_part / para.p;

    printf("Check point 4");
	// Read the file
    for(int i = 0; i < para.p; i++){
        para.local_rank = i;
        pthread_create(&threads[i], NULL, pRead, (void*)&para);
    }

    fclose(para.file_to_read);

    pthread_exit(NULL);
    return 0;
}
