#include "lup_functions.h"

void matrix_copy(double **src, double **dst, int N) {
    int i, k;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            dst[i][k] = src[i][k];
        }
    }
}

void matrix_create(void ***A, int N, int sizeof_type) {
    int i;
    *A = (void**)malloc(sizeof(void*) * N);
    for(i = 0; i < N; i++) {
        (*A)[i] = (void*)malloc(sizeof_type * N);   
    }
}

void matrix_free(void ***A, int N) {
    int i;
    for(i = 0; i < N; i++) {
        free((*A)[i]);
    }
    free(*A);
    *A = NULL;
}

void matrix_make_identity_matrix(int **A, int N) {
    int i, k;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            if(i == k) {
                A[i][k] = 1;
            } else {
                A[i][k] = 0;
            }
        }
    }
}

void LUP_find_pivot(double **C, int N, int current_row, double *pivot_value, int *pivot_row) {
    double l_pivot_value = 0;
    int l_pivot_row = -1;
    int row;
    int column = current_row;
    for(row = current_row; row < N; row++) {
        if (fabs(C[row][column]) > l_pivot_value) {
            l_pivot_value = fabs(C[row][column]);
            l_pivot_row = row;
        }
    }
    *pivot_value = l_pivot_value;
    *pivot_row = l_pivot_row;
}

void LUP_swap_rows(void **P, int row1, int row2) {
    void *tmp = P[row1];
    P[row1] = P[row2];
    P[row2] = tmp;
}

void LUP_C_to_LU(double **C, double **L, double **U, int N) {
    int i, k;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            if(i == k) { //diagonal
                L[i][k] = 1;
                U[i][k] = C[i][k];
            }
            else if((i >= 1) && (i <= N-1) && (k >= 0) && (k <= i-1)) { //below diagonal
                L[i][k] = C[i][k];
                U[i][k] = 0;
            }
            else { //above diagonal
                L[i][k] = 0;
                U[i][k] = C[i][k];
            }
        }
    }
}

void LUP_mpi_matrix_create(double **A, int N) {
    *A = (double*)malloc(sizeof(double) * N * N);
}

void LUP_mpi_matrix_free(double **A) {
    free(*A);
    *A = NULL;
}

int LUP_mpi_find_pivot(double *C, int first_row, int last_row, int current_row, int row_length, double *pivot_value, int *pivot_row, int rows_per_proc) {
    double l_pivot_value = 0;
    int l_pivot_row = -1;
    int row;
    int column = current_row;
    int orig_last_row, orig_first_row;
    orig_first_row = first_row;
    orig_last_row = last_row;
    if(current_row <= last_row) {
        last_row = last_row - first_row;
        if(current_row >= first_row)
            first_row = current_row - first_row;
        else
            first_row = 0;
        for(row = first_row; row <= last_row; row++) {
            if (fabs(C[row_length*row + column]) > l_pivot_value) {
                l_pivot_value = fabs(C[row_length*row + column]);
                l_pivot_row = orig_first_row + row; //original row number
            }
        }
    } else {
        return 1;
    }
    *pivot_value = l_pivot_value;
    *pivot_row = l_pivot_row;
    return 0;
}
