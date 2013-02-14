#include "lcs_functions.h"

int lcs_length(int **L, char *str1, int len1, char *str2, int len2) {
    int i, k;
    for(i = len1; i >= 0; i--) {
        for(k = len2; k >= 0; k--) {
            if(i == len1 || k == len2)
                L[i][k] = 0;
            else if (str1[i] == str2[k])
                L[i][k] = 1 + L[i+1][k+1];
            else
                L[i][k] = max(L[i+1][k], L[i][k+1]);
        }
    }
    return L[0][0];
}

char* lcs_sequence(int **L, int len, char *str1, char *str2) {
    char *sequence = malloc(sizeof(char) * len + 1);
    int seq_index = 0;
    int i, k;
    i = k = 0;
    while(L[i][k] > 0) {
        if(str1[i] == str2[k]) {
            sequence[seq_index++] = str1[i];
            i++; k++;
        } else if(L[i+1][k] >= L[i][k+1])
            i++;
        else
            k++;
    }
    sequence[seq_index] = '\0';
    return sequence;
}

inline void calc_Pij(int **P, int i, int j, char *str2, char *alphabet) {
    if(j == 0) {
        P[i][j] = 0;
    } else if(str2[j-1] == alphabet[i]) {
        P[i][j] = j;
    } else {
        P[i][j] = P[i][j-1];
    }
}

void calc_Sij(int **S, int i, int j, char *str1, int **P, char *alphabet) {
    if(i == 0 || j == 0) {
        S[i][j] = 0;
    } else {
        int c = strchr(alphabet, str1[i-1]) - alphabet;
        if(P[c][j] == 0) {
            S[i][j] = max(S[i-1][j], 0);
        } else {
            S[i][j] = max(S[i-1][j], S[i-1][P[c][j] - 1] + 1);
        }
    }
}
