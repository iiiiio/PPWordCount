#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<pthread.h>
#include<sys/stat.h>
#include<mpi.h>
#include<sys/syscall.h>

#include "lib/oAs.h"

/**
* @file oAs.c
* @mainpage Parallel word counting using pthreads and MPI
*
* @brief this is the file where the functions in @file oAs.h are implemented
*
*/

/**
* @brief Opens file at given path
*
* @param char* path, char* reading_mode
* @return FILE* to_read
* @retval after the file is opened. FILE* to_read will be returned.
*/
FILE * open(char* path, char* reading_mode){
    printf("Check point 1.1");
    // ZH Notes: "r" is for setting fopen mode to read mode
    FILE * to_read = fopen(path, "r");
    if(!to_read){
        printf("ERROR: Failed to open file %s\n", path);
        exit(EXIT_FAILURE);
    }
    printf("Check point 1.2");
    return to_read;
}

/**
* @brief Caluclates file size in bytes
*
* @param char* file
* @return size_t segments
* @retval returns the inputted file size in size_t datatype.
*/
size_t fsize(char* file){
    printf("Check point 2.1");
    // ZH Query: How does struct stat work?
    struct stat file_stat;
    int status = stat(file, &file_stat);
    int segments = (status == 0) ? file_stat.st_size : -1;
    if(segments < 0){
        printf("ERROR: Failed to get file size\n");
        exit(EXIT_FAILURE);
    }

    printf("Check point 2.2");
    return (size_t)segments;
}

// MPI
/**
* @brief Reads file using MPI
*
* @param int p, size_t split_segment, int local_rank, FILE* file_to_read, size_t origin_part.
*
*/
void mpiRead(int p, size_t split_segment, int local_rank, FILE* file_to_read, size_t origin_part){

    printf("Check point 4, %d l here 1\n", local_rank);
    // ZH Query: What does pfm stand for? part for me
    size_t pfm = (local_rank == 0) ? 1 : (local_rank * split_segment)+1; // beginning segment for the current process
    // ZH Query: origin_part-1? why? because there's an EOF character there
    size_t limit = (local_rank < p) ? (local_rank+1) * split_segment : origin_part-1; // reading limit for current process

    fseek(file_to_read, pfm, SEEK_SET); /**< start reading bytes*/
    char* word = (char*)malloc(100*sizeof(char)); /**< temporary pointer to store characters and form words (reader)*/
    //char* words = (char**)malloc(sizeof(word)); /**< add to storage (memorise) */ // storge. will be added by @jasper
    char cur_char;
    size_t track = 0;
    size_t count = 0;

    printf("Check point 4, %d here 2\n", local_rank);
    while(pfm < limit){ /**< Read until the limit of the current process*/
        printf("Check point 4, %d local_rank %zu partform %zu count\n", local_rank, pfm, count);
        cur_char = fgetc(file_to_read); // get the next character in starting from fseek()
        if(isupper(cur_char)){ // low case all upper case characters
            cur_char = tolower(cur_char);
        }else if(cur_char == EOF){ // if end of file is reached
            printf("%c Hola!\n", cur_char);
            break;
        }

        *(word+track) = cur_char;

        if(!isalnum(cur_char)){ // if empty space or new line
            //*(words+count) = word; // not right
            count++;
            // use the same memory address
            // memset(word, 0x00, limit);
            // track = 0;
        }else if(isalnum(cur_char)){ // if character
            track++;
        }
        printf("Check point 4, %d l here \"%s \" 3\n", local_rank, word);

    }

    printf("%s\n", word);
    printf("wack\n");
    // free(words);
    free(word);

}

/**
* @brief under construction
* used to read a given file using pthreads
*
* @param void* parameters
*/
// POSIX
// void* pRead(void* parameters){
//
//     POSIX param_passed = *((POSIX*)parameters);
//     printf("Check point 4, %d\n", local_rank);
//     size_t pfm = (local_rank == 0) ? 1 : (local_rank * param_passed.split_segments)+1; // beginning segment for the current process
//     size_t limit = (local_rank < param_passed.p-1) ? (local_rank+1) * param_passed.split_segments : param_passed.origin_part; // reading limit for current process
//
//     printf("Check point 4, %d\n", local_rank);
//     fseek(param_passed.file_to_read, pfm, SEEK_SET);
//     char* word = (char*)malloc(param_passed.split_segments * sizeof(char));
//     char* words = (char*)malloc(param_passed.origin_part * sizeof(char));
//     char* origin = word;
//     char cur_char = ' ';
//     size_t count = 0;
//     printf("Check point 4, %d\n", local_rank);
//     while(pfm <= limit){
//         printf("Check point %c, %d\n", cur_char, local_rank);
//         cur_char = fgetc(param_passed.file_to_read);
//         if(!isalnum(cur_char)){
//             if(word != origin){
//                 count++;
//                 words = word;
//                 words++;
//                 printf("Check point 4, %d 4\n", local_rank);
//                 free(word);
//                 word = origin;
//             }
//         }else{
//             *word = &cur_char;
//             word++;
//         }
//     }
//     printf("Check point 4, %d\n", local_rank);
//     printf("The file has %ld words in it.\n", count);
//     free(word);
//     free(words);
//
//     printf("Check point 4, %d\n", local_rank);
//     pthread_exit(NULL);
//     return 0;
// }
