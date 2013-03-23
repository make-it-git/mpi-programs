#include "lup_functions_mpi.h"

void LUP_mpi_matrix_create(double **A, int N) {
    *A = (double*)malloc(sizeof(double) * N * N);
}

void LUP_mpi_matrix_free(double **A) {
    free(*A);
    *A = NULL;
}

int LUP_mpi_find_pivot(double *C, int first_row, int last_row, int current_row, int row_length, double *pivot_value, int *pivot_row) {
    double l_pivot_value = 0;
    int l_pivot_row = -1;
    int row;
    int column = current_row;
    //int orig_last_row;
    int orig_first_row;
    orig_first_row = first_row;
    //orig_last_row = last_row;
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

void LUP_mpi_swap_rows(double *C, int row1, int row2, int first_row, int N) {
    if(row1 == row2)
        return;
    int i;
    double tmp;
    int r1N, r2N;
    row1 = row1 - first_row;
    row2 = row2 - first_row;
    for(i = 0; i < N; i++) {
        r1N = row1*N; r2N = row2*N;
        tmp = C[r1N + i];
        C[r1N + i] = C[r2N + i];
        C[r2N + i] = tmp;
    }
}

void LUP_find_first_last_rows(int rank, int size, int N, int rows_per_process, int *first_row, int *last_row) {
    if(rank == size - 1)
        *last_row = N-1;
    else
        *last_row = rank * rows_per_process - 1;
    *first_row = (rank - 1) * rows_per_process;
}
