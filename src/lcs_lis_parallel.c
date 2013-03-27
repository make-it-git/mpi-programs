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
    if(size == 1) {
        fprintf(stderr, "At least 2 processes expected\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabet_len = strlen(alphabet);
    #ifdef DEBUG_TIME
    double start_t, end_t;
    start_t = end_t = 0; // suppress warnings about uninitialized variables
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
    // get lengths of str1 and str2
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
    MPI_File_close(&F_str1);
    MPI_File_close(&F_str2);
    #ifdef DEBUG_TIME
    if(rank == 0) {
        end_t = MPI_Wtime();
        printf("%f seconds for reading from files\n", end_t - start_t);
    }
    #endif

    int i;
    int start_index, end_index;
    int chunk_size = alphabet_len / (size - 1);
    if(rank > 0) {
        // count how many times every letter from alphabet appears in str2
        // if start_index = 2, end_index = 4, then this rank processes letters 'CDE'
        // start_index = 10, end_index = 15, this rank processes letters 'KLMNOP'
        // indexes start at zero (start_index = 0 corresponds to letter 'A');
        start_index = (rank - 1) * chunk_size;
        end_index = rank * chunk_size - 1;
        if(rank == size - 1) // last rank processes everything, what is left
            end_index = alphabet_len - 1;
        int *appearances = (int*)malloc(sizeof(int) * (end_index - start_index + 1));
        memset(appearances, 0, sizeof(int) * (end_index - start_index + 1));
        char letter;
        for(i = 0; i < str2_len; i++) {
            letter = str2[i] - 'A';
            if(letter >= start_index && letter <= end_index)
                appearances[letter - start_index] += 1;
        }
        MPI_Gatherv(appearances, end_index - start_index + 1, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
        free(appearances);
    }
    if(rank == 0) {
        int *appearances = (int*)malloc(sizeof(int) * (alphabet_len + 1)); // '+ 1' to receive value from rank=0
                                                                           // but it is unused
        int *recvcounts = (int*)malloc(sizeof(int) * size);
        int *displs = (int*)malloc(sizeof(int) * size);
        recvcounts[0] = 1;
        displs[0] = 0;
        for(i = 1; i < size; i++) {
            start_index = (i - 1) * chunk_size;
            end_index = i * chunk_size - 1;
            if(i == size - 1)
                end_index = alphabet_len - 1;
            recvcounts[i] = end_index - start_index + 1;
            displs[i] = start_index + 1;
        }
        MPI_Gatherv(MPI_IN_PLACE, 1, MPI_INT, appearances, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
        // appearances[0] is not valid
        //for(i = 1; i <= alphabet_len; i++) {
        //    printf("%c - ", 'A' + i - 1);
        //    printf("%d\n", appearances[i]);
        //}
        free(appearances);
        free(recvcounts);
        free(displs);
    }
    // now rank=0 has information about how many times every letter from alphabet appears in str2

    free(str1);
    free(str2);
    MPI_Finalize();
    return 0;
}
