#ifndef LCS_FUNCTIONS_H
#define LCS_FUNCTIONS_H

#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define max(x,y) ((x) > (y) ? (x) : (y))

int lcs_length(int **S, char *str1, int len1, char *str2, int len2);

char* lcs_sequence(int **L, int len, char *str1, int str1_len, char *str2, int str2_len);

void calc_Pij(int *P, int i, int j, char *str2, char *alphabet);

void parallel_calc_P(char *str2, int str2_len, char *alphabet, int alphabet_len);

void calc_Sij(int **S, int i, int j, char *str1, int **P, char *alphabet);
void calc_S_current(int *S_current, int *S_prev, int i, int j, int neg_displacement, char *str1, int **P, char *alphabet);

void parallel_calc_S(int **P, char *alphabet, char *str1, int str1_len, int str2_len);

void LCS_IO_calc_P(MPI_File f_str2, char *alphabet, MPI_File f_P, int first_row, int last_row);
void LCS_IO_calc_S(MPI_File f_str1, MPI_File f_str2, MPI_File f_P, MPI_File f_S, char *alphabet, int first_column, int last_column);
void LCS_IO_lcs_sequence(MPI_File f_S, MPI_File f_str1, MPI_File f_str2, char **s);

#endif
