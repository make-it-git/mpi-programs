#include <stdio.h>
#include "lcs_functions.h"

#define BUF_SIZE (1024*1024)

int main(int argc, char **argv) {
    char *str1 = malloc(sizeof(char) * BUF_SIZE);
    char *str2 = malloc(sizeof(char) * BUF_SIZE);
    int str1_len;
    int str2_len;
    char *c;
    FILE *f1 = fopen(argv[1], "rb");
    FILE *f2 = fopen(argv[2], "rb");
    fgets(str1, BUF_SIZE, f1);
    fgets(str2, BUF_SIZE, f2);
    c = strchr(str1, '\n');
    if(c) *c = '\0';
    c = strchr(str2, '\n');
    if(c) *c = '\0';
    str1_len = strlen(str1);
    str2_len = strlen(str2);
    fclose(f1);
    fclose(f2);
    int **res_matr = malloc(sizeof(void*) * (str1_len + 1));
    int i;
    for(i = 0; i <= str1_len; i++)
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));
    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    printf("%d\n", lcs_len);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str1_len, str2, str2_len);
    printf("%d\n", lcs_len);
    free(seq);
    for(i = 0; i <= str1_len; i++)
        free(res_matr[i]);
    free(res_matr);
    free(str1);
    free(str2);
    return 0;
}
