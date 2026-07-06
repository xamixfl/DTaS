#ifndef _SPARSE_H_
#define _SPARSE_H_

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM_RUNS 1000

#define ERR_INVALID_DIM 1
#define ERR_MEM_ALLOC 2
#define ERR_DIM_MISMATCH 3
#define ERR_NULL_PTR 4
#define ERR_INPUT 5
#define ERR_INVALID_PERCENT 6

typedef struct
{
    int **data;
    int rows;
    int cols;
} StandardMatrix_t;

typedef struct
{
    int *A;
    int *IA;
    int *JA;
    int non_zeros;
    int rows;
    int cols;
} SparseMatrixCSC_t;

int create_standard_matrix(int rows, int cols, StandardMatrix_t *matrix);
void free_standard_matrix(StandardMatrix_t *matrix);
int input_standard_matrix(StandardMatrix_t *matrix);
int print_standard_matrix(const StandardMatrix_t *matrix);
int add_standard_matrices(const StandardMatrix_t *m1, const StandardMatrix_t *m2, StandardMatrix_t *result);
int create_sparse_matrix(int rows, int cols, int non_zeros, SparseMatrixCSC_t *matrix);
void free_sparse_matrix(SparseMatrixCSC_t *matrix);
int print_csc_matrix(const SparseMatrixCSC_t *matrix);
int convert_to_csc(const StandardMatrix_t *s_matrix, SparseMatrixCSC_t *csc_matrix);
int add_csc_matrices(const SparseMatrixCSC_t *m1, const SparseMatrixCSC_t *m2, SparseMatrixCSC_t *result);
int input_csc_matrix_coordinate(int rows, int cols, SparseMatrixCSC_t *matrix);
int input_standard_matrix_coordinate(int rows, int cols, StandardMatrix_t *matrix);
int generate_sparse_matrix(int rows, int cols, double percent, SparseMatrixCSC_t *csc_matrix, StandardMatrix_t *std_matrix);

#endif
