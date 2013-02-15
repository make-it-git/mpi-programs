#include "lcs_functions.h"

int lcs_length(int **L, char *str1, int len1, char *str2, int len2) {
    int i, k;
    for(i = 0; i <= len1; i++) {
        for(k = 0; k <= len2; k++) {
            if(i == 0 || k == 0)
                L[i][k] = 0;
            else if (str1[i-1] == str2[k-1])
                L[i][k] = 1 + L[i-1][k-1];
            else
                L[i][k] = max(L[i-1][k], L[i][k-1]);
        }
    }
    return L[len1][len2];
}

char* lcs_sequence(int **L, int len, char *str1, int str1_len, char *str2, int str2_len) {
    char *sequence = malloc(sizeof(char) * len + 1);
    int seq_index = len;
    sequence[seq_index] = '\0';
    int i, k;
    i = str1_len;
    k = str2_len ;
    while(L[i][k] > 0) {
        if(str1[i-1] == str2[k-1]) {
            sequence[--seq_index] = str1[i-1];
            i--; k--;
        } else if(L[i-1][k] >= L[i][k-1])
            i--;
        else
            k--;
    }
    return sequence;
}

//inline void calc_Pij(int **P, int i, int j, char *str2, char *alphabet) {
inline void calc_Pij(int *P, int i, int j, char *str2, char *alphabet) {
    if(j == 0) {
        P[j] = 0;
    } else if(str2[j-1] == alphabet[i]) {
        P[j] = j;
    } else {
        P[j] = P[j-1];
    }
}

inline void calc_Sij(int **S, int i, int j, char *str1, int **P, char *alphabet) {
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
inline void calc_S_current(int *S_current, int *S_prev, int i, int j, char *str1, int **P, char *alphabet) {
    if(i == 0 || j == 0) {
        S_current[j] = 0;
    } else {
        int c = strchr(alphabet, str1[i-1]) - alphabet;
        if(P[c][j] == 0) {
            S_current[j] = max(S_prev[j], 0);
        } else {
            S_current[j] = max(S_prev[j], S_prev[P[c][j] - 1] + 1);
        }
    }
}

void parallel_calc_P(char *str2, int str2_len, char *alphabet, int alphabet_len) {
    int i,j;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    size -= 1; // rank=0 does not work here
    //int **P = malloc(sizeof(void*) * alphabet_len);
    for(i = 0; i < alphabet_len; i++) {
        if(((i % size) + 1) == rank) {
            //printf("rank = %d calculating P[%d]\n", rank, i);
            int *P = malloc(sizeof(void*) * (str2_len + 1));
            for(j = 0; j <= str2_len; j++) {
                calc_Pij(P, i, j, str2, alphabet);
            }
            MPI_Send(P, str2_len+1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            free(P);
        }
    }
}

void parallel_calc_S(int **P, char *alphabet, char *str1, int str1_len, int str2_len) {
    int i, j, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    size -= 1; // rank=0 does not work here
    int *S_current = malloc(sizeof(int) * (str2_len + 1));
    int *S_prev = malloc(sizeof(int) * (str2_len + 1));
    for(i = 0; i <= str1_len; i++) {
        if(i > 0) {
            // get previous row
            MPI_Bcast(S_prev, str2_len+1, MPI_INT, 0, MPI_COMM_WORLD);
        }
        for(j = 0; j <= str2_len; j++) {
            if(((j % size) + 1) == rank) {
                //printf("parallel_cals_S: rank=%d calculating S[%d][%d]\n", rank, i, j);
                calc_S_current(S_current, S_prev, i, j, str1, P, alphabet);
            }
        }
        MPI_Send(S_current, str2_len+1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    free(S_current);
    free(S_prev);
}
