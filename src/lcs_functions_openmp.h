#ifndef _LCS_FUNCTIONS_OPENMP_H_
#define _LCS_FUNCTIONS_OPENMP_H

#include <stdlib.h>
#include <string.h>
#define max(x,y) ((x)>(y) ? (x) : (y))

int *parallel_calc_P(char *string2, int string2_length, char *alphabet, int alphabet_length);
int *parallel_calc_S(int *P, char *alphabet, char *string1, int string1_length, int string2_length);
char* lcs_sequence(int *S, int lcs_length, char *string1, int string1_length, char *string2, int string2_length);

#endif
