#ifndef _LUP_FUNCTIONS_MPI_H_
#define _LUP_FUNCTIONS_MPI_H_

#include "lup_functions.h"
#include <mpi.h>

void LUP_mpi_matrix_create(double **A, int N);
void LUP_mpi_matrix_free(double **A);
int LUP_mpi_find_pivot(double *C, int first_row, int last_row, int current_row, int row_length, double *pivot_value, int *pivot_row);
void LUP_mpi_swap_rows(double *C, int row1, int row2, int first_row, int N);
void LUP_find_first_last_rows(int rank, int size, int N, int rows_per_process, int *first_row, int *last_row);

#endif
