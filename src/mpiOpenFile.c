#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<mpi.h>
#include<sys/stat.h>

#include "lib/oAs.h"

/**
 * @file mpiOpenFile.c
 * @date 24 Jan 2019
 * @mainpage Parallel word counting using MPI
 * @brief This program caluclates the number of words, in a file sequentially, using MPI.
 *
 * the mehtod used to read the words from the file was adapted from
 * https://github.com/giuseppeangri/mpi-parallel-words-count/blob/master/src/file_reader.c#L8
 * Later on pthread and MPI will be applied.
 * currently this is the only working file for MPI parallel processing, it can only run 1 porocessor.
 * further MPI implementation must be applied to use multiple processes
 *
 * @param char* argv[] - file input through terminal.
 *
 */


int main(int args, char* argv[]){

    printf("Check point 0");

    const char* path = argv[1];
    int p;
    int local_rank;
    MPI_Status* s;
    FILE* to_read;
    MPI_Init(&args, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);

    printf("Check point 1");
    // open file based on path.
    if(local_rank == 0){
        to_read = open((char*)path, "r"); /** the function open is from the file @file oAc.c th function initialization is in @file oAc.h (different function will be used for MPI, should be MPI_OPEN_FILE())*/
    }

    printf("Check point 2");
    // Get size of the file to return the size of the file by bytes.
    int segments = fsize((char*)path); /** calls fsize() function from @file oAc.c */

    printf("Check point 3");
    // Split the bytes among processes.
    int partitions = segments / p; /** splits the value returnd from fsize() and devides it among proesses (needs to change for POSIX) */

    printf("Check point 4");
	// Read the file
    mpiRead(p, partitions, local_rank, to_read, segments); /**Start reading the file*/

    if(local_rank == 0){
        fclose(to_read); // should be MPI_FILE_CLOSE()
    }
    MPI_Finalize();
    return 0;
}
