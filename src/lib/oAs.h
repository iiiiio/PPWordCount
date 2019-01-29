

#include<stdio.h>
#include<stdlib.h>
/**
* @file oAs.h
* @mainpage Parallel word counting using pthreads
*
* @brief oAs header is where everything is centered (not really), it's used as a passage way to access the functions from other file.
*
*/

/**
 *@brief  the struct holds the parameters of pthreads
 *
 *@struct param
 */
typedef struct param{
    int p;
    size_t  split_segments, origin_part;
    FILE * file_to_read;
    int local_rank;
} POSIX;

/** for the following fucntions
*
* @see oAs.c for implementations
*/

extern void* pRead(void* ); /**< see @file pOpenFile.c for usage*/
extern void mpiRead(int , size_t , int, FILE* , size_t ); /**< see @file mpiRead.c for usage*/
extern FILE* open(char* , char* );
extern size_t fsize(char* );

//Test github
