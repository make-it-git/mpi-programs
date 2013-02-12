#include <stdio.h>
#include <stdlib.h>

#define max(x,y) ((x) > (y) ? (x) : (y))

/*char *str1 = "jgufia";
char *str2 = "kgdeila";
#define str1_len 6
#define str2_len 7*/
/*char *str1 = "bajkrewlopa";
char *str2 = "klakwoz";
#define str1_len 11
#define str2_len 7*/
char *str1 = "empty bottle";
char *str2 = "nematode knowledge";
#define str1_len 12
#define str2_len 18

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
    return sequence;
}

int main(int argc, char *argv[]) {
    int *res_matr[str1_len + 1];
    int i, k;
    for(i = 0; i <= str1_len; i++) 
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));

    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str2);

    for(i = 0; i <= str1_len; i++) {
        for(k = 0; k <= str2_len; k++) {
            printf("%d   ", res_matr[i][k]);
        }
        printf("\n");
    }

    for(i = 0; i <= str1_len; i++) 
        free(res_matr[i]);
    return 0;
}
