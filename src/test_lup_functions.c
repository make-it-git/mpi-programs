#include <check.h>
#include "lup_functions.h"
#include <string.h>

START_TEST(test_matrix_create)
    double **A = NULL;
    int **B = NULL;
    int N = 4;

    matrix_create((void***)&A, N, sizeof(double));
    ck_assert(A != NULL);
    matrix_free((void***)&A, N);
    ck_assert(A == NULL);

    matrix_create((void***)&B, N, sizeof(double));
    ck_assert(B != NULL);
    matrix_free((void***)&B, N);
    ck_assert(B == NULL);
END_TEST

START_TEST(test_matrix_create_identity_matrix)
    int **A = NULL;
    int N = 4;
    matrix_create((void***)&A, N, sizeof(int));
    matrix_make_identity_matrix(A, N);
    int expected[][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    int i;
    for(i = 0; i < N; i++) {
        ck_assert(memcmp(A[i], expected[i], N*sizeof(int)) == 0);
    }
    matrix_free((void***)&A, N);
END_TEST

START_TEST(test_find_pivot)
    int N = 4;
    double tmp[][4] = {
        {10.0, 20.0, 1.1, 20.1},
        {8.5,  4.3,  3.3, 0.5},
        {1.3,  9.6,  4.8, 0.1},
        {9.3,  18.9,  3.9, 5.0}
    };
    double *A[] = {tmp[0], tmp[1], tmp[2], tmp[3]};
    double delta = 1E-8;
    double pivot_value;
    int pivot_row;

    LUP_find_pivot(A, N, 0, &pivot_value, &pivot_row);
    ck_assert_int_eq(pivot_row, 0);
    ck_assert(fabs(pivot_value - A[pivot_row][0]) < delta);

    LUP_find_pivot(A, N, 1, &pivot_value, &pivot_row);
    ck_assert_int_eq(pivot_row, 3);
    ck_assert(fabs(pivot_value - A[pivot_row][1]) < delta);

    LUP_find_pivot(A, N, 2, &pivot_value, &pivot_row);
    ck_assert_int_eq(pivot_row, 2);
    ck_assert(fabs(pivot_value - A[pivot_row][2]) < delta);

    LUP_find_pivot(A, N, 3, &pivot_value, &pivot_row);
    ck_assert_int_eq(pivot_row, 3);
    ck_assert(fabs(pivot_value - A[pivot_row][3]) < delta);

END_TEST

START_TEST(test_swap_rows)
    double **A = NULL;
    double reference[4][4];
    int N = 4;
    matrix_create((void***)&A, N, sizeof(double));
    int i, k;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            A[i][k] = i*N + k;
            reference[i][k] = A[i][k];
        }
    }
    LUP_swap_rows((void**)A, 0, 1);
    ck_assert((memcmp(A[0], reference[1], sizeof(double) * N)) == 0);
    LUP_swap_rows((void**)A, 0, 1);
    ck_assert((memcmp(A[0], reference[0], sizeof(double) * N)) == 0);
    LUP_swap_rows((void**)A, 2, 3);
    ck_assert((memcmp(A[2], reference[3], sizeof(double) * N)) == 0);
    LUP_swap_rows((void**)A, 2, 3);
    ck_assert((memcmp(A[2], reference[2], sizeof(double) * N)) == 0);
    matrix_free((void***)&A, N);
END_TEST

START_TEST(test_C_to_LU)
    double **A = NULL;
    double **C = NULL;
    double **L = NULL;
    double **U = NULL;
    int **P = NULL;
    int N = 3;
    double tmp[][3] = {
        {2, 7, -6},
        {8, 2, 1},
        {7, 4, 2}
    };
    matrix_create((void***)&A, N, sizeof(double));
    int i, k;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            A[i][k] = tmp[i][k];
        }
    }
    matrix_create((void***)&C, N, sizeof(double));
    matrix_create((void***)&L, N, sizeof(double));
    matrix_create((void***)&U, N, sizeof(double));
    matrix_create((void***)&P, N, sizeof(int));

    matrix_copy(A, C, N);
    matrix_make_identity_matrix(P, N);
    double pivot_value;
    int pivot_row;
    int j;
    for(i = 0; i < N; i++) {
        LUP_find_pivot(C, N, i, &pivot_value, &pivot_row);
        LUP_swap_rows((void**)C, pivot_row, i);
        LUP_swap_rows((void**)P, pivot_row, i);
        for(j = i+1; j < N; j++) {
            C[j][i] /= C[i][i];
            for(k = i+1; k < N; k++) {
                C[j][k] -= C[j][i] * C[i][k];
            }
        }
    }
    LUP_C_to_LU(C, L, U, N);
    double expected_L[][3] = {
        {1, 0, 0},
        {0.25, 1, 0},
        {0.875, 0.346, 1}
    };
    double expected_U[][3] = {
        {8, 2, 1},
        {0, 6.5, -6.25},
        {0, 0, 3.288}
    };
    double delta = 1E-3;
    for(i = 0; i < N; i++) {
        for(k = 0; k < N; k++) {
            ck_assert(fabs(L[i][k] - expected_L[i][k]) < delta);
            ck_assert(fabs(U[i][k] - expected_U[i][k]) < delta);
        }
    }
    matrix_free((void***)&A, N);
    matrix_free((void***)&C, N);
    matrix_free((void***)&L, N);
    matrix_free((void***)&U, N);
    matrix_free((void***)&P, N);
END_TEST

Suite *lcs_suite(void) {
    Suite *s = suite_create("LUP_FUNCTIONS");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_matrix_create);
    tcase_add_test(tc_core, test_matrix_create_identity_matrix);
    tcase_add_test(tc_core, test_find_pivot);
    tcase_add_test(tc_core, test_swap_rows);
    tcase_add_test(tc_core, test_C_to_LU);
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
