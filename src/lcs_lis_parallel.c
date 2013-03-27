#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lcs_lis_functions.h"

#define DEBUG_TIME

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(argc != 4 && rank == 0) {
        fprintf(stderr, "Usage: %s str1_file str2_file out_file\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabet_len = strlen(alphabet);
    #ifdef DEBUG_TIME
    double start_t, end_t;
    if(rank == 0)
        start_t = MPI_Wtime();
    #endif
    // open files with str1 and str2 and read it's contents
    char *filename_str1 = argv[1];
    char *filename_str2 = argv[2];
    char *filename_out = argv[3];
    MPI_File F_str1, F_str2;
    MPI_File_open(MPI_COMM_WORLD, filename_str1, MPI_MODE_RDONLY, MPI_INFO_NULL, &F_str1);
    MPI_File_open(MPI_COMM_WORLD, filename_str2, MPI_MODE_RDONLY, MPI_INFO_NULL, &F_str2);

    MPI_Offset str1_len, str2_len;
    MPI_File_get_size(F_str1, &str1_len);
    MPI_File_get_size(F_str2, &str2_len);
    char *str1 = (char*)malloc(sizeof(char) * (str1_len + 1));
    char *str2 = (char*)malloc(sizeof(char) * (str2_len + 1));
    // now read str1 and str2 from files
    MPI_File_read_at(F_str1, (MPI_Offset)0, str1, str1_len, MPI_CHAR, MPI_STATUS_IGNORE);
    str1[str1_len] = '\0';
    MPI_File_read_at(F_str2, (MPI_Offset)0, str2, str2_len, MPI_CHAR, MPI_STATUS_IGNORE);
    str2[str2_len] = '\0';
    #ifdef DEBUG_TIME
    if(rank == 0) {
        end_t = MPI_Wtime();
        printf("%f seconds for reading from files\n", end_t - start_t);
    }
    #endif
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_File_close(&F_str1);
    MPI_File_close(&F_str2);
    MPI_Finalize();
    return 0;
}
