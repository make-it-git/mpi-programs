#include "lcs_functions_openmp.h"

int *parallel_calc_P(char *string2, int string2_length, char *alphabet, int alphabet_length) {
    int i, j;
    int *P = (int*)malloc(sizeof(int) * alphabet_length * (string2_length + 1));
    #pragma omp parallel private(j)
    {
        #pragma omp for
        for(i = 0; i < alphabet_length; i++) {
            P[i * (string2_length + 1)] = 0;
            for(j = 1; j <= string2_length; j++) {
                if(string2[j - 1] == alphabet[i]) {
                    P[i * (string2_length + 1) + j] = j;
                } else {
                    P[i * (string2_length + 1) + j] = P[i * (string2_length + 1) + j - 1];
                }
            }
        }
    }
    return P;
}

int *parallel_calc_S(int *P, char *alphabet, char *string1, int string1_length, int string2_length) {
    int i, j;
    int *S = (int*)malloc(sizeof(int) * (string1_length + 1) * (string2_length + 1));
    int ch;
    for(i = 0; i <= string1_length; i++) {
        #pragma omp for schedule(static)
        for(j = 0; j <= string2_length; j++) {
            if(i == 0 || j == 0) {
                S[i * (string2_length + 1) + j] = 0;
            } else {
                ch = strchr(alphabet, string1[i - 1]) - alphabet;
                if(P[ch * (string2_length + 1) + j] == 0) {
                    S[i * (string2_length + 1) + j] = max(S[(i - 1) * (string2_length + 1) + j], 0);
                } else {
                    S[i * (string2_length + 1) + j] = max(S[(i - 1) * (string2_length + 1) + j], S[(i - 1) * (string2_length + 1) + P[ch * (string2_length + 1) + j] - 1] + 1);
                }
            }
        }
    }
    return S;
}

char* lcs_sequence(int *S, int lcs_length, char *string1, int string1_length, char *string2, int string2_length) {
    char *sequence = (char*)malloc(sizeof(char) * lcs_length + 1);
    int seq_index = lcs_length;
    sequence[seq_index] = '\0';
    int i, k;
    i = string1_length;
    k = string2_length;
    while(S[i * (string2_length + 1) + k] > 0) {
        if(string1[i - 1] == string2[k - 1]) {
            sequence[--seq_index] = string1[i - 1];
            i--; k--;
        } else if(S[(i - 1) * (string2_length + 1) + k] >= S[i * (string2_length + 1) + k - 1])
            i--;
        else
            k--;
    }
    return sequence;
}
