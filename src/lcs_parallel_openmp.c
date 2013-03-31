#include <stdio.h>
#include "lcs_functions_openmp.h"

#define BUF_SIZE (1024*1024)

int main(int argc, char **argv) {
    if(argc != 4) {
        fprintf(stderr, "Usage: %s str1_file str2_file out_file\n", argv[0]);
        return 1;
    }
    char *str1 = (char*)malloc(sizeof(char) * BUF_SIZE);
    char *str2 = (char*)malloc(sizeof(char) * BUF_SIZE);
    int str1_len;
    int str2_len;
    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alp_len = strlen(alphabet);
    FILE *f3;
    char *c;
    FILE *f1 = fopen(argv[1], "rb");
    FILE *f2 = fopen(argv[2], "rb");
    f3 = fopen(argv[3], "wb");
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
    int *P = parallel_calc_P(str2, str2_len, alphabet, alp_len);
    int *S = parallel_calc_S(P, alphabet, str1, str1_len, str2_len);
    char *sequence = lcs_sequence(S, S[(str1_len + 1) * (str2_len + 1) - 1], str1, str1_len, str2, str2_len);
    fprintf(f3, "lcs length = %d\n", S[(str1_len + 1) * (str2_len + 1) - 1]);
    fprintf(f3, "lcs sequence\n");
    fprintf(f3, "%s\n", sequence);
    free(sequence);
    free(P);
    free(S);
    fclose(f3);
    return 0;
}
