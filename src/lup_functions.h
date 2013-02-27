#ifndef _LUP_FUNCTIONS_H_
#define _LUP_FUNCTIONS_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

void matrix_copy(double **src, double **dst, int N);

void matrix_create(void ***A, int N, int sizeof_type);
void matrix_free(void ***A, int N);

void matrix_make_identity_matrix(int **A, int N);

void LUP_find_pivot(double **C, int N, int current_row, double *pivot_value, int *pivot_row);
void LUP_swap_rows(void **P, int row1, int row2);
void LUP_C_to_LU(double **C, double **L, double **U, int N);

void LUP_mpi_matrix_create(double **A, int N);
void LUP_mpi_matrix_free(double **A);
int LUP_mpi_find_pivot(double *C, int first_row, int last_row, int current_row, int row_length, double *pivot_value, int *pivot_row);
void LUP_mpi_swap_rows(double *C, int row1, int row2, int first_row, int N);

#endif
