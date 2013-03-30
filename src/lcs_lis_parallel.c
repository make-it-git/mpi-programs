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
    int chunk_size = str2_len / (size - 1);
    if(rank > 0) {
        // every rank>0 processes part of str2
        // then rank=0 reduces results
        start_index = (rank - 1) * chunk_size;
        end_index = rank * chunk_size - 1;
        if(rank == size - 1) // last rank processes everything, what is left
            end_index = str2_len - 1;
        int *appearances = (int*)malloc(sizeof(int) * alphabet_len);
        // appearances[0] corresponds to 'A'
        // appearances[25] - 'Z'
        memset(appearances, 0, sizeof(int) * alphabet_len);
        int letter;
        for(i = start_index; i <= end_index; i++) {
            letter = str2[i] - 'A';
            appearances[letter] += 1;
        }
        MPI_Gatherv(appearances, alphabet_len, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(appearances, alphabet_len, MPI_INT, 0, MPI_COMM_WORLD);
        // now every rank>0 contains _global_ count of how many times every letter from alphabet appears in str2
        int *ds[alphabet_len], ds_length[alphabet_len]; // ds=decreasing sequence
        for(i = 0; i < alphabet_len; i++) {
            ds[i] = (int*)malloc(sizeof(int) * appearances[i]);
            // ds[0] corresponds to letter 'A'
            // if str2='bacdeafa', then ds[0]={8, 6, 2}
            memset(ds[i], 0, sizeof(int) * appearances[i]);
            ds_length[i] = 0;
        }
        // every rank>0 processes it's part of str2
        // but now rank=1 processes _last_ part of str2
        // for example, if str2='abcdefghj', size=4 (including rank=0)
        // then rank=1 processes 'ghj', rank=2 - 'def', rank=3 - 'abc'
        start_index = (size - (rank + 1)) * chunk_size;
        end_index = (size - rank) * chunk_size - 1;
        if(rank == 1) {
            end_index = str2_len - 1;
        }
        for(i = end_index; i >= start_index; i--) {
           letter = str2[i] - 'A';
           ds[letter][ds_length[letter]] = i;
           ds_length[letter] += 1;
        }
        // decreasing sequence is done, send results to rank=0
        MPI_Gather(ds_length, alphabet_len, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
        // now send all ds's
        for(i = 0; i < alphabet_len; i++) {
            MPI_Gatherv(ds[i], ds_length[i], MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
        }
        // cleanup
        for(i = 0; i < alphabet_len; i++) {
            free(ds[i]);
        }
        free(appearances);
    }
    if(rank == 0) {
        #ifdef DEBUG_TIME
        start_t = MPI_Wtime();
        #endif
        int *appearances = (int*)malloc(sizeof(int) * ((size - 1) * alphabet_len + 1)); // '+ 1' to receive value from rank=0
                                                                                        // but it is unused
        int *recvcounts = (int*)malloc(sizeof(int) * size);
        int *displs = (int*)malloc(sizeof(int) * size);
        recvcounts[0] = 1;
        displs[0] = 0;
        for(i = 1; i < size; i++) {
            start_index = (i - 1) * chunk_size;
            //end_index = i * chunk_size - 1;
            //if(i == size - 1)
            //    end_index = str2_len - 1;
            recvcounts[i] = alphabet_len;
            displs[i] = alphabet_len * (i - 1) + 1;
        }
        MPI_Gatherv(MPI_IN_PLACE, 1, MPI_INT, appearances, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
        // reduce data from all processes
        int k;
        int letter_displacement;
        for(i = 1; i <= alphabet_len; i++) {
            letter_displacement = i - 1;
            for(k = 2; k < size; k++) {
                appearances[i] += appearances[displs[k] + letter_displacement];
            }
        }
        #ifdef DEBUG_TIME
        end_t = MPI_Wtime();
        printf("%f seconds for calculation of how many times every letter appears in str2\n", end_t - start_t);
        start_t = MPI_Wtime();
        #endif
        // now appearances[1] through appearances[26] contain _global_ count of how many times every letter appears in str2
        // broadcast it to all processes
        MPI_Bcast(appearances + 1, alphabet_len, MPI_INT, 0, MPI_COMM_WORLD);
        // receive decreasing sequences' lengths
        int ds_lengths[alphabet_len * size];
        // rank=0 sends 26 MPI_INTs, so ds_lengths[0] through ds_lengths[25] are not significant
        MPI_Gather(MPI_IN_PLACE, alphabet_len, MPI_INT, ds_lengths, alphabet_len, MPI_INT, 0, MPI_COMM_WORLD);
        int *ds[alphabet_len];
        recvcounts[0] = 1;
        displs[0] = 0;
        for(i = 0; i < alphabet_len; i++) {
            // only appearances[1] through appearances[26] contain _global_ count
            ds[i] = (int*)malloc(sizeof(int) * (appearances[i + 1] + 1)); // '+1' to receive value from rank=0
            for(k = 1; k < size; k++) {
                recvcounts[k] = ds_lengths[alphabet_len * k + i];
                //k=1 - rank=1. ds_length[26] - 'A', ds_lengths[27] - 'B'
                //k=2 - rank=2, ds_length[52] - 'A', ds_lengths[53] - 'B'
                if(k == 1) {
                    displs[k] = 1;
                } else {
                    displs[k] = ds_lengths[alphabet_len * (k - 1) + i] + displs[k - 1];
                }
            }
            MPI_Gatherv(MPI_IN_PLACE, 1, MPI_INT, ds[i], recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
        }
        #ifdef DEBUG_TIME
        end_t = MPI_Wtime();
        printf("%f seconds for creation of decreasing sequences\n", end_t - start_t);
        start_t = MPI_Wtime();
        #endif
        int letter_appearances;
        int dec_sequences_count = 1;
        int **dec_sequences = (int**)malloc(sizeof(int**) * 1);
        dec_sequences[0] = (int*)malloc(sizeof(int) * appearances[str1[0] - 'A' + 1]);
        int *dec_sequences_lengths = (int*)malloc(sizeof(int) * 1);
        dec_sequences_lengths[0] = appearances[str1[0] - 'A' + 1];
        for(i = 0; i < dec_sequences_lengths[0]; i++) {
            dec_sequences[0][i] = ds[(int)(str1[0] - 'A')][i + 1];
        }
        int seq;
        int letter;
        for(i = 1; i < str1_len; i++) {
            letter = str1[i] - 'A';
            letter_appearances = appearances[letter + 1]; // '+1' because appearances[0] is not valid
            for(k = 1; k <= letter_appearances; k++) {
                seq = lower_bound(dec_sequences, dec_sequences_lengths, dec_sequences_count, ds[letter][k]);
                if(seq >= 0) {
                    dec_sequences_lengths[seq] += 1;
                    dec_sequences[seq] = (int*)realloc(dec_sequences[seq], sizeof(int) * dec_sequences_lengths[seq]);
                    dec_sequences[seq][dec_sequences_lengths[seq] - 1] = ds[letter][k];
                } else {
                    dec_sequences_count += 1;
                    dec_sequences_lengths = (int*)realloc(dec_sequences_lengths, sizeof(int) * dec_sequences_count);
                    dec_sequences_lengths[dec_sequences_count - 1] = 1;
                    dec_sequences = (int**)realloc(dec_sequences, sizeof(int**) * dec_sequences_count);
                    dec_sequences[dec_sequences_count - 1] = (int*)malloc(sizeof(int) * 1);
                    dec_sequences[dec_sequences_count - 1][0] = ds[letter][k];
                }
            }
        }
        #ifdef DEBUG_TIME
        end_t = MPI_Wtime();
        printf("%f seconds for LCS creation\n", end_t - start_t);
        #endif
        FILE *F_out = fopen(filename_out, "w");
        if(!F_out) {
            fprintf(stderr, "Unable to open file '%s' for writing\nOutput to console...\n", filename_out);
            F_out = stdout;
        }
        fprintf(F_out, "lcs length = %d\n", dec_sequences_count);
        fprintf(F_out, "lcs sequence\n");
        for(i = 0; i < dec_sequences_count; i++) {
            fprintf(F_out, "%c", str2[dec_sequences[i][dec_sequences_lengths[i] - 1]]);
        }
        fclose(F_out);
        // cleanup
        free(dec_sequences_lengths);
        for(i = 0; i < dec_sequences_count; i++) {
            free(dec_sequences[i]);
        }
        free(dec_sequences);
        for(i = 0; i < alphabet_len; i++) {
            free(ds[i]);
        }
        free(appearances);
        free(recvcounts);
        free(displs);
    }

    free(str1);
    free(str2);
    MPI_Finalize();
    return 0;
}
