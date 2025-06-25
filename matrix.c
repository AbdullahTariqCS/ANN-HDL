#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
Improved matrix library where matrices are stored in arrays, instead of mallocs,
*/

typedef double (*Activation)(double);

typedef struct Matrix
{
    int rows;
    int columns;
    double *matrix;
} Matrix;

#define mat_alloc(O, _rows, _columns, arr_name) \
    O.rows = _rows;                             \
    O.columns = _columns;                       \
    double arr_name[O.rows * O.columns];        \
    O.matrix = (double *)arr_name;

#define mat_alloc_from_matrix(O, A, arr_name)    \
    O.rows = A.rows;                             \
    O.columns = A.columns;                       \
    double arr_name[A.rows * A.columns];         \
    O.matrix = arr_name;                         \
    for (int i = 0; i < O.rows * O.columns; i++) \
        O.matrix[i] = A.matrix[i];

#define mat_alloc_from_arr(A, _rows, _columns, arr) \
    A.rows = _rows;                                 \
    A.columns = _columns;                           \
    A.matrix = arr;

#define mat_copy(O, A)                                                           \
    mat_alloc(O, A.rows, A.columns) for (int i = 0; i < O.rows * O.columns; i++) \
        O.matrix[i] = A.matrix[i];

int mat_copy_vals(Matrix *O, Matrix *A)
{
    if (O->rows != A->rows || O->columns != A->columns)
    {
        printf("Cannot copy A(%d, %d) to O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < O->rows * O->columns; i++)
        O->matrix[i] = A->matrix[i];

    return EXIT_SUCCESS;
}

int mat_print(Matrix *A)
{
    // printf("Hello world");
    for (int i = 0; i < A->rows; i++)
    {
        for (int j = 0; j < A->columns; j++)
            printf("%f ", A->matrix[i * A->columns + j]);
        printf("\n");
    }
    return EXIT_SUCCESS;
}

int mat_write(Matrix *A, FILE *file)
{

    fprintf(file, "%d;%d\n", A->rows, A->columns);
    for (int i = 0; i < A->rows; i++)
    {
        for (int j = 0; j < A->columns; j++)
            fprintf(file, "%f;", A->matrix[i * A->columns + j]);
        fprintf(file, "\n");
    }
    return EXIT_SUCCESS;
}

int mat_read_size(Matrix *matrix, FILE *file)
{
    // TODO: add error handling
    fscanf(file, "%d;%d\n", &matrix->rows, &matrix->columns);
    return EXIT_SUCCESS;
}

int mat_read(Matrix *A, FILE *file)
{

    for (int i = 0; i < A->rows * A->columns; i++)
    {
        fscanf(file, "%lf;", &A->matrix[i]);
    }
    fscanf(file, "\n");
    return EXIT_SUCCESS;
}

int mat_mul(Matrix *O, Matrix *A, Matrix *B)
{
    if (A->columns != B->rows)
    {
        printf("Cannot Multiply (%d, %d) with (%d, %d)\n", A->rows, A->columns, B->rows, B->columns);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < A->rows; i++)
    {
        for (int j = 0; j < B->columns; j++)
        {
            O->matrix[i * O->columns + j] = 0;

            for (int k = 0; k < A->columns; k++)
                O->matrix[i * O->columns + j] += A->matrix[i * A->columns + k] * B->matrix[k * B->columns + j];
        }
    }
    return EXIT_SUCCESS;
}

int mat_haddard(Matrix *O, Matrix *A, Matrix *B)
{
    if (A->rows != B->rows || A->columns != B->columns)
    {
        printf("Cannot scalar multiply (%d, %d) with (%d, %d)\n", A->rows, A->columns, B->rows, B->columns);
        return EXIT_FAILURE;
    }

    // for(int i = 0; i < A->rows; i++)
    //     for(int j = 0; j<A->columns; j++)
    //         O->matrix[i*O->columns+j] = A->matrix[i*A->columns+j] * B->matrix[i*A->columns+j];

    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] = A->matrix[i] * B->matrix[i];

    return EXIT_SUCCESS;
}

int mat_haddard_inplace(Matrix *O, Matrix *A)
{
    if (A->rows != O->rows || A->columns != O->columns)
    {
        printf("Cannot scalar multiply A (%d, %d) with O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    // for(int i = 0; i < A->rows; i++)
    //     for(int j = 0; j<A->columns; j++)
    //         O->matrix[i*O->columns+j] *= A->matrix[i*A->columns+j];

    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] *= A->matrix[i];

    return EXIT_SUCCESS;
}

int mat_add(Matrix *O, Matrix *A, Matrix *B)
{
    if (A->rows != B->rows || A->columns != B->columns)
    {
        printf("Cannot add (%d, %d) with (%d, %d)\n", A->rows, A->columns, B->rows, B->columns);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] = A->matrix[i] + B->matrix[i];

    return EXIT_SUCCESS;
}

int mat_map_inplace(Matrix *A, double f(double))
{
    for (int i = 0; i < A->rows * A->columns; i++)
        A->matrix[i] = f(A->matrix[i]);

    return EXIT_SUCCESS;
}

int mat_map(Matrix *O, Matrix *A, double f(double))
{
    if (O->rows != A->rows || O->columns != A->columns)
    {
        printf("Cannot store result of map(A) (%d, %d) in O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] = f(A->matrix[i]);

    return EXIT_SUCCESS;
}

int mat_transpose(Matrix *O, Matrix *A)
{
    if (O->rows != A->columns || O->columns != A->rows)
    {
        printf("Cannot store result of T(A) (%d, %d) in O (%d, %d)\n", A->columns, A->rows, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    // for (int i = 0; i < O->rows; i++)
    //     for (int j = 0; j < O->columns; j++)
    //         O->matrix[i * O->rows + j] = A->matrix[i * A->rows + j];

    for (int i = 0; i < A->rows * A->columns; i++ )
    {
        int r = i % O->rows; 
        int c = i / O->rows; 

        O->matrix[r * O->columns + c] = A->matrix[i];
        // printf("%d: (%d, %d) = %.0f [%d]\n", i, r, c, O->matrix[r * O->columns + c], r*O->columns + c);
    }   
    // printf("\n");


    return EXIT_SUCCESS;
}

int arr_to_mat(Matrix *O, double *arr, int arr_len)
{
    if (O->columns * O->rows != arr_len)
    {
        printf("Cannot convert array of len (%d) to Matrix with dimensions (%d, %d)\n", arr_len, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < arr_len; i++)
        O->matrix[i] = arr[i];

    return EXIT_SUCCESS;
}

int mat_to_arr(double *O, Matrix *A, int arr_len)
{
    if (A->columns * A->rows != arr_len)
    {
        printf("Cannot convert array of len (%d) to Matrix with dimensions (%d, %d)\n", arr_len, A->rows, A->columns);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < arr_len; i++)
        O[i] = A->matrix[i];

    return EXIT_SUCCESS;
}

// void mat_rand(Matrix* A)
// {
//     double *normal_data = malloc(A->rows * A->columns * sizeof(double));
//     const double PI = 3.14;
//     for(int i = 0; i < A->rows; i++)
//         for(int j = 0; j < A->columns; j++)
//             A->matrix[i][j] = -1 + rand() % (2);
// }

int mat_zeros_inplace(Matrix *A)
{
    for (int i = 0; i < A->rows * A->columns; i++)
        A->matrix[i] = 0;
    return EXIT_SUCCESS;
}

int mat_rand_he_inplace(Matrix *A)
{
    double stddev = sqrt(2.0 / (A->columns)); // He initialization for ReLU
    for (int i = 0; i < A->rows; i++)
    {
        for (int j = 0; j < A->columns; j++)
        {
            double u1 = (rand() + 1.0) / (RAND_MAX + 1.0);
            double u2 = (rand() + 1.0) / (RAND_MAX + 1.0);
            double rand_normal = sqrt(-2 * log(u1)) * cos(2 * 3.14159 * u2);
            A->matrix[i * A->columns + j] = rand_normal * stddev;
        }
    }
    return EXIT_SUCCESS;
}

void mat_rand(Matrix *A)
{
    // double *normal_data = malloc(A->rows * A->columns * sizeof(double));
    // const double PI = 3.14;
    for (int i = 0; i < A->rows * A->columns; i++)
        A->matrix[i] = -1 + rand() % (2);
}

int mat_rand_xavier_inplace(Matrix *A)
{
    if (A->columns == 0 || A->rows == 0)
    {
        fprintf(stderr, "Invalid matrix dimensions for initialization\n");
        return EXIT_FAILURE;
    }

    double fan_in = A->columns;
    double fan_out = A->rows;
    double stddev = sqrt(2.0 / (fan_in + fan_out)); // Xavier/Glorot initialization

    for (int i = 0; i < A->rows; i++)
    {
        for (int j = 0; j < A->columns; j++)
        {
            double u1 = (rand() + 1.0) / (RAND_MAX + 1.0); // Avoid log(0)
            double u2 = (rand() + 1.0) / (RAND_MAX + 1.0);
            double rand_normal = sqrt(-2.0 * log(u1)) * cos(2.0 * 3.14159 * u2);
            A->matrix[i * A->columns + j] = rand_normal * stddev;
        }
    }
    return EXIT_SUCCESS;
}

int mat_sum(Matrix *O, Matrix *A, Matrix *B)
{
    if (B->rows != A->rows || B->columns != A->columns)
    {
        printf("Cannot add matrix A (%d, %d) with B (%d, %d)\n", A->rows, A->columns, B->rows, B->columns);
        return EXIT_FAILURE;
    }

    if (O->rows != A->rows || O->columns != A->columns)
    {
        printf("Cannot store result of A + B (%d, %d) in O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }

    // for(int i = 0; i < A->rows; i++)
    //     for(int j = 0; j<A->columns; j++)
    //         O->matrix[i*O->columns+j] = A->matrix[i*A->columns+j] + B->matrix[i*B->columns+j];
    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] = A->matrix[i] + B->matrix[i];

    return EXIT_SUCCESS;
}

int mat_scale(Matrix *O, Matrix *A, double n)
{
    if (O->rows != A->rows || O->columns != A->columns)
    {
        printf("Cannot store result of scale(A) (%d, %d) in O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < A->columns * A->rows; i++)
        O->matrix[i] = A->matrix[i] * n;

    return EXIT_SUCCESS;
}

int mat_scale_inplace(Matrix *O, double n)
{
    for (int i = 0; i < O->columns * O->rows; i++)
        O->matrix[i] = O->matrix[i] * n;

    return EXIT_SUCCESS;
}

int mat_add_scalar(Matrix *O, Matrix *A, double n)
{
    if (O->rows != A->rows || O->columns != A->columns)
    {
        printf("Cannot store result of A + n (%d, %d) in O (%d, %d)\n", A->rows, A->columns, O->rows, O->columns);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < A->rows * A->columns; i++)
        O->matrix[i] = A->matrix[i] + n;

    return EXIT_SUCCESS;
}

int mat_normalize_inplace(Matrix *A)
{
    int N = A->rows * A->columns;
    double mean = 0;
    double std_deviation = 0;
    for (int i = 0; i < N; i++)
        mean += A->matrix[i];
    mean /= N;

    for (int i = 0; i < N; i++)
        std_deviation += pow(A->matrix[i] - mean, 2);
    std_deviation = sqrt(std_deviation / N);

    mat_add_scalar(A, A, -mean);
    const double tolerance = 1e-12;

    if (fabs(std_deviation) > tolerance)
        mat_scale(A, A, 1 / std_deviation);
}
