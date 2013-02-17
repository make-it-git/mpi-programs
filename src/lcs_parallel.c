#include <stdio.h>
#include "lcs_functions.h"
#include <unistd.h>

#define BUF_SIZE (1024*1024)

int main(int argc, char **argv) {
    double start_t, end_t;
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char *str1 = malloc(sizeof(char) * BUF_SIZE);
    char *str2 = malloc(sizeof(char) * BUF_SIZE);
    int str1_len;
    int str2_len;
    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alp_len = strlen(alphabet);
    if(rank == 0) {
        start_t = MPI_Wtime();
        char *c;
        FILE *f1 = fopen(argv[1], "rb");
        FILE *f2 = fopen(argv[2], "rb");
        fgets(str1, BUF_SIZE, f1);
        fgets(str2, BUF_SIZE, f2);
        c = strchr(str1, '\n');
        if(c) *c = '\0';
        c = strchr(str2, '\n');
        if(c) *c = '\0';
        str1_len = strlen(str1);
        str2_len = strlen(str2);
        fclose(f1);
        fclose(f2);
        end_t = MPI_Wtime();
        printf("rank=%d: %f seconds for [reading from files]\n", rank, end_t-start_t);
    }
    if(rank==0) {
        start_t = MPI_Wtime();
    }
    MPI_Bcast(&str1_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&str2_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(str1, str1_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(str2, str2_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    if(rank==0) {
        end_t = MPI_Wtime();
        printf("rank=%d: %f seconds for [MPI_Bcast of data]\n", rank, end_t-start_t);
    }
    if(rank==0) {
        start_t = MPI_Wtime();
    }
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    size -= 1;
    int **P = malloc(sizeof(void*) * alp_len);
    int i;
    for(i = 0; i < alp_len; i++)
        P[i] = malloc(sizeof(int) * (str2_len + 1));
    if(rank > 0) {
        parallel_calc_P(str2, str2_len, alphabet, alp_len);
    } else { //rank=0
        //printf("rank = %d\n", rank);
        for(i = 0; i < alp_len; i++) {
            MPI_Recv(P[i], str2_len+1, MPI_INT, (i % size) + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        /*int j;
        for(i = 0; i < alp_len; i++) {
            for(j = 0; j <= str2_len; j++) {
                printf("%d\t", P[i][j]);
            }
            printf("\n");
        }
        for(i = 0; i < alp_len; i++)
            free(P[i]);
        free(P);*/
    }
    for(i = 0; i < alp_len; i++) {
        MPI_Bcast(P[i], str2_len+1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    if(rank==0) {
        end_t = MPI_Wtime();
        printf("rank=%d: %f seconds for [calculation of P]\n", rank, end_t-start_t);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) {
        int **S = malloc(sizeof(void*) * (str1_len + 1));
        int *rcvcounts = malloc(sizeof(int) * size+1);
        int *displs = malloc(sizeof(int) * size+1);
        int chunk_size = (str2_len + 1) / size;
        int start;
        int end;
        rcvcounts[0] = 0;
        displs[0] = 0;
        for(i = 1; i <= size; i++) {
            start = (i-1) * chunk_size;
            end = i * chunk_size;
            if(i == size)
                end = str2_len + 1;
            rcvcounts[i] = end - start;
            displs[i] = start;
            //printf("rcvcounts[%d]=%d,\tdispls[%d]=%d, end=%d\n", i, rcvcounts[i], i, displs[i], end);
        }
        for(i = 0; i <= str1_len; i++) {
            S[i] = malloc(sizeof(int) * (str2_len + 1));
        }
        start_t = MPI_Wtime();
        for(i = 0; i <= str1_len; i++) {
            MPI_Gatherv(NULL, 0, MPI_INT, S[i], rcvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
            if(i != str1_len) // this is not last row
                MPI_Bcast(S[i], str2_len+1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        end_t = MPI_Wtime();
        printf("rank=%d: %f seconds for [calculation of S]\n", rank, end_t-start_t);
        printf("%d\n", lcs_length(S, str1, str1_len, str2, str2_len));
        //printf("%s\n", lcs_sequence(S, S[str1_len][str2_len], str1, str1_len, str2, str2_len));
        /*int j;
        for(i = 0; i <= str1_len; i++) {
            for(j = 0; j <= str2_len; j++) {
                printf("%d  ", S[i][j]);
            }
            printf("\n");
        }*/
        for(i = 0; i <= str1_len; i++) {
            free(S[i]);
        }
        free(S);
        free(rcvcounts);
        free(displs);
    } else { //rank>0
        parallel_calc_S(P, alphabet, str1, str1_len, str2_len);
    }
    for(i = 0; i < alp_len; i++)
        free(P[i]);
    free(P);
    free(str1);
    free(str2);
    MPI_Finalize();
    return 0;
}
