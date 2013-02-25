#include <stdio.h>
#include "lup_functions.h"

#define BUF_SIZE (1024*1024)
#define N 4

int main(int argc, char **argv) {
    double **A;
    double **C;
    int **P;
    matrix_create((void***)&A, N, sizeof(double));
    matrix_create((void***)&C, N, sizeof(double));
    matrix_create((void***)&P, N, sizeof(int));
    matrix_make_identity_matrix(P, N);

    int i, j, k;
    printf("A\n");
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            A[i][k] = i*N + k;
            printf("%.2f\t", A[i][k]);
        }
        printf("\n");
    }
    matrix_copy(A, C, N);

    double pivot_value;
    int pivot_row;
    for(i = 0; i < N; i++) {
        LUP_find_pivot(C, N, i, &pivot_value, &pivot_row);
        if(pivot_value == 0) {
            fprintf(stderr, "Matrix is singular\n");
            break;
        } else {
            LUP_swap_rows((void**)C, pivot_row, i);
            LUP_swap_rows((void**)P, pivot_row, i);
            for(j = i+1; j < N; j++) {
                C[j][i] /= C[i][i];
                for(k = i+1; k < N; k++) {
                    C[j][k] -= C[j][i] * C[i][k];
                }
            }
        }
    }
    printf("C\n");
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            printf("%.2f\t", C[i][k]);
        }
        printf("\n");
    }
    printf("P\n");
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            printf("%d\t", P[i][k]);
        }
        printf("\n");
    }

    matrix_free((void***)&P, N);
    matrix_free((void***)&C, N);
    matrix_free((void***)&A, N);
    return 0;
}
