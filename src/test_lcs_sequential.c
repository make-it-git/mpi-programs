#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "lcs_functions.h"

START_TEST(test_lcs1)
    char *str1 = "empty bottle";
    char *str2 = "nematode knowledge";
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    int **res_matr = malloc(sizeof(void*) * (str1_len + 1));
    int i;
    for(i = 0; i <= str1_len; i++)
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));
    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    ck_assert(lcs_len == 7);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str1_len, str2, str2_len);
    ck_assert_str_eq(seq, "emt ole");
    for(i = 0; i <= str1_len; i++)
        free(res_matr[i]);
    free(res_matr);
END_TEST

START_TEST(test_lcs2)
    char *str1 = "jgufia";
    char *str2 = "kgdeila";
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    int **res_matr = malloc(sizeof(void*) * (str1_len + 1));
    int i;
    for(i = 0; i <= str1_len; i++)
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));
    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    ck_assert(lcs_len == 3);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str1_len, str2, str2_len);
    ck_assert_str_eq(seq, "gia");
    for(i = 0; i <= str1_len; i++)
        free(res_matr[i]);
    free(res_matr);
END_TEST

START_TEST(test_lcs3)
    char *str1 = "bajkrewlopa";
    char *str2 = "klakwoz";
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    int **res_matr = malloc(sizeof(void*) * (str1_len + 1));
    int i;
    for(i = 0; i <= str1_len; i++)
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));
    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    ck_assert(lcs_len == 4);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str1_len, str2, str2_len);
    ck_assert_str_eq(seq, "akwo");
    for(i = 0; i <= str1_len; i++)
        free(res_matr[i]);
    free(res_matr);
END_TEST

START_TEST(test_no_lcs)
    char *str1 = "jgwa";
    char *str2 = "lopxcz";
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    int **res_matr = malloc(sizeof(void*) * (str1_len + 1));
    int i;
    for(i = 0; i <= str1_len; i++)
        res_matr[i] = malloc(sizeof(int) * (str2_len + 1));
    int lcs_len = lcs_length(res_matr, str1, str1_len, str2, str2_len);
    ck_assert(lcs_len == 0);
    char *seq = lcs_sequence(res_matr, lcs_len, str1, str1_len, str2, str2_len);
    ck_assert_str_eq(seq, "");
    for(i = 0; i <= str1_len; i++)
        free(res_matr[i]);
    free(res_matr);
END_TEST

Suite *lcs_suite(void) {
    Suite *s = suite_create("LCS");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_lcs1);
    tcase_add_test(tc_core, test_lcs2);
    tcase_add_test(tc_core, test_lcs3);
    tcase_add_test(tc_core, test_no_lcs);
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
