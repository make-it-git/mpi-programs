#include <stdio.h>
#include "lcs_functions.h"

int main(int argc, char **argv) {
    char *str1 = "TAGCA";
    int str1_len = strlen(str1);
    char *str2 = "ATTGCCA";
    int str2_len = strlen(str2);
    char *alphabet = "ATCG";
    int alp_len = strlen(alphabet);
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    size -= 1;
    int **P = malloc(sizeof(void*) * alp_len);
    int i, j;
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
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) {
        int **S = malloc(sizeof(void*) * (str1_len + 1));
        int *s_buf = malloc(sizeof(int) * (str2_len + 1));
        for(i = 0; i <= str1_len; i++) {
            S[i] = malloc(sizeof(int) * (str2_len + 1));
        }
        int k;
        for(i = 0; i <= str1_len; i++) {
            for(j = 1; j <= size; j++) {
                MPI_Recv(s_buf, str2_len+1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for(k = 0; k <= str2_len; k++) {
                    if((k % size) + 1 == j) { // j = rank
                        S[i][k] = s_buf[k];
                    }
                }
            }
            MPI_Bcast(S[i], str2_len+1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        for(i = 0; i <= str1_len; i++) {
            for(j = 0; j <= str2_len; j++) {
                printf("%d\t", S[i][j]);
            }
            printf("\n");
        }
        for(i = 0; i <= str1_len; i++) {
            free(S[i]);
        }
        free(S);
        free(s_buf);
    } else { //rank>0
        parallel_calc_S(P, alphabet, str1, str1_len, str2_len);
    }
    for(i = 0; i < alp_len; i++)
        free(P[i]);
    free(P);
    MPI_Finalize();
    return 0;
}
