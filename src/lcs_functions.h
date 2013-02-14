#ifndef LCS_FUNCTIONS_H
#define LCS_FUNCTIONS_H

#include <stdlib.h>

#define max(x,y) ((x) > (y) ? (x) : (y))

int lcs_length(int **L, char *str1, int len1, char *str2, int len2);

char* lcs_sequence(int **L, int len, char *str1, char *str2);

void calc_Pij(int **P, int i, int j, char *str2, char *alphabet);

#endif
