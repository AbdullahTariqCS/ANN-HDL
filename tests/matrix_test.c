#include "unity.h"
#include "../matrix.c"


void TEST_ASSERT_MATRIX(Matrix A, Matrix B)
{
    TEST_ASSERT_EQUAL(A.rows, B.rows); 
    TEST_ASSERT_EQUAL(A.columns, B.columns); 
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(A.matrix, B.matrix, A.rows * A.columns);
}

void test_mat_mul()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double b_matrix[] = {
        1, 4, 
        2, 5,
        3, 6
    };

    double actual_res_matrix[] = {
        14, 32, 
        32, 77, 
        50, 122
    };

    Matrix A, B, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(B, 3, 2, b_matrix);
    mat_alloc_from_arr(AR, 3, 2, actual_res_matrix);
    mat_alloc(R, 3, 2, res_matrix);
    mat_mul(&R, &A, &B);

    TEST_ASSERT_MATRIX(R, AR);
}


void test_mat_haddard()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double b_matrix[] = {
        1, 2, 3,
        4, 5, 6, 
        7, 8, 9
    };

    double actual_result_matrix[] = {
        2, 4, 9,
        16, 25, 36,
        49, 64, 81
    };

    Matrix A, B, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(B, 3, 3, b_matrix);
    mat_alloc_from_arr(AR, 3, 3, actual_result_matrix);
    mat_alloc(R, 3, 3, res_matrix);
    mat_haddard(&R, &A, &B);

    TEST_ASSERT_MATRIX(R, AR);
}



void test_mat_haddard_inplace()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double b_matrix[] = {
        1, 2, 3,
        4, 5, 6, 
        7, 8, 9
    };

    double actual_result_matrix[] = {
        2, 4, 9,
        16, 25, 36,
        49, 64, 81
    };

    Matrix A, B, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(B, 3, 3, b_matrix);
    mat_alloc_from_arr(AR, 3, 3, actual_result_matrix);
    mat_haddard_inplace(&A, &B);

    TEST_ASSERT_MATRIX(A, AR);
}

double square(double x)
{
    return x * x; 
}

void test_mat_map()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double actual_result_matrix[] = {
        2, 4, 9,
        16, 25, 36,
        49, 64, 81
    };


    Matrix A, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(AR, 3, 3, actual_result_matrix);
    mat_alloc(R, 3, 3, result_matrix);

    mat_map(&R, &A, square);

    TEST_ASSERT_MATRIX(R, AR);
}

void test_mat_map_inplace()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double actual_result_matrix[] = {
        2, 4, 9,
        16, 25, 36,
        49, 64, 81
    };


    Matrix A, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(AR, 3, 3, actual_result_matrix);

    mat_map_inplace(&A, square);

    TEST_ASSERT_MATRIX(A, AR);
}

void test_mat_tranpose()
{
    double a_matrix[] = {
        1, 2, 3, 10, 
        4, 5, 6, 11,
        7, 8, 9, 12
    };

    double actual_result_matrix[] = {
        1, 4, 7,
        2, 5, 8, 
        3, 6, 9, 
        10, 11, 12
    };


    Matrix A, R, AR; 
    mat_alloc_from_arr(A, 3, 4, a_matrix);
    mat_alloc_from_arr(AR, 4, 3, actual_result_matrix);
    mat_alloc(R, 4, 3, result_matrix);

    mat_transpose(&R, &A);

    TEST_ASSERT_MATRIX(R, AR);
}

void test_arr_to_mat()
{
    double arr[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };
     
    Matrix A, R, AR; 
    mat_alloc(A, 3, 3, a_matrix);
    arr_to_mat(&A, arr, 9);
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(A.matrix, arr, 9);
}

void test_arr_to_mat()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };
    double arr[9];
     
    Matrix A; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_to_arr(arr, &A, 9);
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(A.matrix, arr, 9);
}


#define GARBAGE_PATTERN 0xCD
void test_mat_rand_he_inplace_should_fill_matrix_with_non_garbage_random_values(void)
{
    const int rows = 3, cols = 3;
    Matrix A;
    double a_matrix[9];
    for(int i = 0; i < 9; i++)
        a_matrix[i] = -999999.9999;

    mat_alloc_from_arr(A, 3, 3, a_matrix)


    int result = mat_rand_he_inplace(&A);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    for (int i = 0; i < rows * cols; i++) {
        double val = A.matrix[i];

        TEST_ASSERT_FALSE_MESSAGE(isnan(val), "Found NaN in matrix");
        TEST_ASSERT_FALSE_MESSAGE(isinf(val), "Found Inf in matrix");

        double garbage_val = -999999.9999;
        TEST_ASSERT_NOT_EQUAL(garbage_val, val);

        TEST_ASSERT_NOT_EQUAL(0.0, val);
    }
}

void test_mat_rand_xavier_inplace_should_fill_matrix_with_non_garbage_random_values(void)
{
    const int rows = 3, cols = 3;
    Matrix A;
    double a_matrix[9];
    for(int i = 0; i < 9; i++)
        a_matrix[i] = -999999.9999;

    mat_alloc_from_arr(A, 3, 3, a_matrix)


    int result = mat_rand_xavier_inplace(&A);
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    for (int i = 0; i < rows * cols; i++) {
        double val = A.matrix[i];

        TEST_ASSERT_FALSE_MESSAGE(isnan(val), "Found NaN in matrix");
        TEST_ASSERT_FALSE_MESSAGE(isinf(val), "Found Inf in matrix");

        double garbage_val = -999999.9999;
        TEST_ASSERT_NOT_EQUAL(garbage_val, val);

        TEST_ASSERT_NOT_EQUAL(0.0, val);
    }
}

void test_mat_sum()
{
    double a_matrix[] = {
        1, 2, 3, 
        4, 5, 6, 
        7, 8, 9
    };

    double b_matrix[] = {
        1, 2, 3,
        4, 5, 6, 
        7, 8, 9
    };

    double actual_result_matrix[] = {
        2, 4, 6,
        8, 10, 12,
        14, 16, 18,
    };

    Matrix A, B, R, AR; 
    mat_alloc_from_arr(A, 3, 3, a_matrix);
    mat_alloc_from_arr(B, 3, 3, b_matrix);
    mat_alloc_from_arr(AR, 3, 3, actual_result_matrix);
    mat_alloc(R, 3, 3, res_matrix);
    mat_sum(&R, &A, &B);

    TEST_ASSERT_MATRIX(R, AR);
}

2