#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "lcs_functions.h"

START_TEST(test_create_P)
    char *str2 = "ATTGCCA";
    int str2_len = strlen(str2);
    char *alphabet = "ATCG";
    int alp_len = strlen(alphabet);
    int i, j;
    int **P = malloc(sizeof(void*) * alp_len);
    for(i = 0; i < alp_len; i++)
        P[i] = malloc(sizeof(int) * (str2_len + 1));
    for(i = 0; i < alp_len; i++) {
        for(j = 0; j <= str2_len; j++) {
            calc_Pij(P, i, j, str2, alphabet);
        }
    }
    int expected[][8] = {
        {0, 1, 1, 1, 1, 1, 1, 7},
        {0, 0, 2, 3, 3, 3, 3, 3},
        {0, 0, 0, 0, 0, 5, 6, 6},
        {0, 0, 0, 0, 4, 4, 4, 4}
    };
    for(i = 0; i < 4; i++) {
        ck_assert(memcmp(P[i], expected[i], 8*sizeof(int)) == 0);
    }
    for(i = 0; i < alp_len; i++)
        free(P[i]);
    free(P);
END_TEST

START_TEST(test_create_S)
    char *str1 = "TAGCA";
    int str1_len = strlen(str1);
    char *str2 = "ATTGCCA";
    int str2_len = strlen(str2);
    char *alphabet = "ATCG";
    int alp_len = strlen(alphabet);
    int i, j;
    int **P = malloc(sizeof(void*) * alp_len);
    for(i = 0; i < alp_len; i++)
        P[i] = malloc(sizeof(int) * (str2_len + 1));
    for(i = 0; i < alp_len; i++) {
        for(j = 0; j <= str2_len; j++) {
            calc_Pij(P, i, j, str2, alphabet);
        }
    }
    int **S = malloc(sizeof(void*) * (str1_len + 1));
    for(i = 0; i <= str1_len; i++) {
        S[i] = malloc(sizeof(int) * (str2_len + 1));
    }
    for(i = 0; i <= str1_len; i++) {
        for(j = 0; j <= str2_len; j++) {
            calc_Sij(S, i, j, str1, P, alphabet);
        }
    }
    int expected[][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 2},
        {0, 1, 1, 1, 2, 2, 2, 2},
        {0, 1, 1, 1, 2, 3, 3, 3},
        {0, 1, 1, 1, 2, 3, 3, 4}
    };
    for(i = 0; i <= str1_len; i++) {
        ck_assert(memcmp(S[i], expected[i], 8*sizeof(int)) == 0);
    }
    for(i = 0; i <= str1_len; i++) {
        free(S[i]);
    }
    free(S);
    for(i = 0; i < alp_len; i++)
        free(P[i]);
    free(P);
END_TEST

Suite *lcs_suite(void) {
    Suite *s = suite_create("LCS");
    TCase *tc_core = tcase_create("MPI");
    tcase_add_test(tc_core, test_create_P);
    tcase_add_test(tc_core, test_create_S);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s = lcs_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
