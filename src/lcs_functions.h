#ifndef _LCS_FUNCTIONS_H_
#define _LCS_FUNCTIONS_H_

#include <stdlib.h>
#include <string.h>

#define max(x,y) ((x) > (y) ? (x) : (y))

int lcs_length(int **S, char *str1, int len1, char *str2, int len2);
char* lcs_sequence(int **L, int len, char *str1, int str1_len, char *str2, int str2_len);

void calc_Pij(int *P, int i, int j, char *str2, char *alphabet);
void calc_Sij(int **S, int i, int j, char *str1, int **P, char *alphabet);

#endif
