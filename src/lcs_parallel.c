#include <stdio.h>
#include "lcs_functions.h"

int main(int argc, char **argv) {
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
    if(rank > 0) {
        parallel_calc_P(str2, str2_len, alphabet, alp_len);
    } else { //rank=0
        //printf("rank = %d\n", rank);
        int i;
        int **P = malloc(sizeof(void*) * alp_len);
        for(i = 0; i < alp_len; i++)
            P[i] = malloc(sizeof(int) * (str2_len + 1));
        for(i = 0; i < alp_len; i++) {
            MPI_Recv(P[i], str2_len+1, MPI_INT, (i % size) + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        int j;
        for(i = 0; i < alp_len; i++) {
            for(j = 0; j <= str2_len; j++) {
                printf("%d\t", P[i][j]);
            }
            printf("\n");
        }
        for(i = 0; i < alp_len; i++)
            free(P[i]);
        free(P);
    }
    MPI_Finalize();
    return 0;
}
