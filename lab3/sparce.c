#include <stdio.h>
#include <stdlib.h>

#include "sparse.h"

// Создание стандартной матрицы.
int create_standard_matrix(int rows, int cols, StandardMatrix_t *matrix)
{
    if (matrix == NULL)
        return ERR_NULL_PTR;
    if (rows <= 0 || cols <= 0)
        return ERR_INVALID_DIM;

    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(int *));
    if (matrix->data == NULL)
        return ERR_MEM_ALLOC;

    for (int i = 0; i < rows; i++)
    {
        matrix->data[i] = (int *) calloc(cols, sizeof(int));
        if (matrix->data[i] == NULL)
        {
            for (int j = 0; j < i; j++)
                free(matrix->data[j]);
            free(matrix->data);
            matrix->data = NULL;
            return ERR_MEM_ALLOC;
        }
    }
    return 0;
}

// Освобождение памяти стандартной матрицы.
void free_standard_matrix(StandardMatrix_t *matrix)
{
    if (matrix == NULL || matrix->data == NULL)
        return;
    for (int i = 0; i < matrix->rows; i++)
        free(matrix->data[i]);
    free(matrix->data);
    matrix->data = NULL;
}

// Ввод элементов стандартной матрицы.
int input_standard_matrix(StandardMatrix_t *matrix)
{
    if (matrix == NULL || matrix->data == NULL)
        return ERR_NULL_PTR;
    printf("Введите элементы матрицы (%d x %d):\n", matrix->rows, matrix->cols);
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->cols; j++)
        {
            printf("Элемент [%d][%d]: ", i, j);
            if (scanf("%d", &matrix->data[i][j]) != 1)
                return ERR_INPUT;
        }
    }
    return 0;
}

// Печать стандартной матрицы.
int print_standard_matrix(const StandardMatrix_t *matrix)
{
    if (matrix == NULL || matrix->data == NULL)
        return ERR_NULL_PTR;
    printf("Матрица в стандартном виде:\n");
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->cols; j++)
            printf("%4d ", matrix->data[i][j]);
        printf("\n");
    }
    return 0;
}

// Сложение двух стандартных матриц.
int add_standard_matrices(const StandardMatrix_t *m1, const StandardMatrix_t *m2, StandardMatrix_t *result)
{
    if (m1 == NULL || m2 == NULL || result == NULL || m1->data == NULL || m2->data == NULL)
        return ERR_NULL_PTR;
    if (m1->rows != m2->rows || m1->cols != m2->cols)
        return ERR_DIM_MISMATCH;

    int rc = create_standard_matrix(m1->rows, m1->cols, result);
    if (rc) return rc;

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m1->cols; j++)
            result->data[i][j] = m1->data[i][j] + m2->data[i][j];
    }
    return 0;
}

// Создание разреженной матрицы CSC.
int create_sparse_matrix(int rows, int cols, int non_zeros, SparseMatrixCSC_t *matrix)
{
    if (matrix == NULL)
        return ERR_NULL_PTR;
    if (rows <= 0 || cols <= 0 || non_zeros < 0)
        return ERR_INVALID_DIM;

    matrix->rows = rows;
    matrix->cols = cols;
    matrix->non_zeros = non_zeros;
    matrix->A = malloc(non_zeros * sizeof(int));
    if (non_zeros > 0 && matrix->A == NULL)
        return ERR_MEM_ALLOC;
    matrix->IA = malloc(non_zeros * sizeof(int));
    if (non_zeros > 0 && matrix->IA == NULL)
    {
        free(matrix->A);
        return ERR_MEM_ALLOC;
    }
    matrix->JA = malloc((cols + 1) * sizeof(int));
    if (matrix->JA == NULL)
    {
        free(matrix->IA);
        free(matrix->A);
        return ERR_MEM_ALLOC;
    }
    return 0;
}

// Освобождение памяти разреженной матрицы.
void free_sparse_matrix(SparseMatrixCSC_t *matrix)
{
    if (matrix == NULL)
        return;
    free(matrix->A);
    free(matrix->IA);
    free(matrix->JA);
    matrix->A = NULL;
    matrix->IA = NULL;
    matrix->JA = NULL;
}

// Печать разреженной матрицы CSC.
int print_csc_matrix(const SparseMatrixCSC_t *matrix)
{
    if (matrix == NULL || matrix->JA == NULL)
        return ERR_NULL_PTR;

    printf("Разреженная матрица в формате CSC:\n");
    printf("A (значения):  ");
    for (int i = 0; i < matrix->non_zeros; i++) 
        printf("%d ", matrix->A[i]);
    printf("\n");

    printf("IA (строки):   ");
    for (int i = 0; i < matrix->non_zeros; i++) 
        printf("%d ", matrix->IA[i]);
    printf("\n");

    printf("JA (указатели): ");
    for (int i = 0; i < matrix->cols + 1; i++)
        printf("%d ", matrix->JA[i]);
    printf("\n");

    return 0;
}

// Конвертация стандартной матрицы в CSC.
int convert_to_csc(const StandardMatrix_t *s_matrix, SparseMatrixCSC_t *csc_matrix)
{
    if (s_matrix == NULL || csc_matrix == NULL || s_matrix->data == NULL)
        return ERR_NULL_PTR;

    int non_zeros = 0;
    for (int j = 0; j < s_matrix->cols; j++)
    {
        for (int i = 0; i < s_matrix->rows; i++)
        {
            if (s_matrix->data[i][j] != 0)
                non_zeros++;
        }
    }

    int rc = create_sparse_matrix(s_matrix->rows, s_matrix->cols, non_zeros, csc_matrix);
    if (rc) return rc;

    int k = 0; 
    csc_matrix->JA[0] = 0;

    for (int j = 0; j < s_matrix->cols; j++)
    {
        for (int i = 0; i < s_matrix->rows; i++)
        {
            if (s_matrix->data[i][j] != 0)
            {
                csc_matrix->A[k] = s_matrix->data[i][j];
                csc_matrix->IA[k] = i; 
                k++;
            }
        }
        csc_matrix->JA[j + 1] = k; 
    }
    
    return 0;
}

// Сложение двух CSC матриц.
int add_csc_matrices(const SparseMatrixCSC_t *m1, const SparseMatrixCSC_t *m2, SparseMatrixCSC_t *result)
{
    if (m1 == NULL || m2 == NULL || result == NULL || m1->JA == NULL || m2->JA == NULL)
        return ERR_NULL_PTR;
    if (m1->rows != m2->rows || m1->cols != m2->cols)
        return ERR_DIM_MISMATCH;

    int max_non_zeros = m1->non_zeros + m2->non_zeros;
    int *res_A = malloc(max_non_zeros * sizeof(int));
    if (max_non_zeros > 0 && res_A == NULL)
        return ERR_MEM_ALLOC;
    int *res_IA = malloc(max_non_zeros * sizeof(int));
    if (max_non_zeros > 0 && res_IA == NULL)  
    {
        free(res_A);
        return ERR_MEM_ALLOC;
    }
    int *res_JA = malloc((m1->cols + 1) * sizeof(int));
    if (res_JA == NULL)
    {
        free(res_IA);
        free(res_A);
        return ERR_MEM_ALLOC;
    }

    int count = 0;
    res_JA[0] = 0;

    for (int col = 0; col < m1->cols; col++)
    {
        int ptr1 = m1->JA[col];
        int end1 = m1->JA[col + 1];
        int ptr2 = m2->JA[col];
        int end2 = m2->JA[col + 1];

        while (ptr1 < end1 && ptr2 < end2)
        {
            int row1 = m1->IA[ptr1];
            int row2 = m2->IA[ptr2];

            if (row1 < row2)
            {
                res_A[count] = m1->A[ptr1];
                res_IA[count] = row1;
                ptr1++;
                count++;
            }
            else if (row1 > row2)
            {
                res_A[count] = m2->A[ptr2];
                res_IA[count] = row2;
                ptr2++;
                count++;
            }
            else
            {
                int sum = m1->A[ptr1] + m2->A[ptr2];
                if (sum != 0)
                {
                    res_A[count] = sum;
                    res_IA[count] = row1;
                    count++;
                }
                ptr1++;
                ptr2++;
            }
        }

        while (ptr1 < end1)
        {
            res_A[count] = m1->A[ptr1];
            res_IA[count] = m1->IA[ptr1];
            ptr1++;
            count++;
        }

        while (ptr2 < end2)
        {
            res_A[count] = m2->A[ptr2];
            res_IA[count] = m2->IA[ptr2];
            ptr2++;
            count++;
        }

        res_JA[col + 1] = count;
    }

    int rc = create_sparse_matrix(m1->rows, m1->cols, count, result);
    if (rc)
    {
        free(res_A);
        free(res_IA);
        free(res_JA);
        return rc;
    }

    for (int i = 0; i < count; i++)
    {
        result->A[i] = res_A[i];
        result->IA[i] = res_IA[i];
    }
    for (int i = 0; i <= m1->cols; i++)
        result->JA[i] = res_JA[i];

    free(res_A);
    free(res_IA);
    free(res_JA);
    return 0;
}

// Ввод CSC матрицы в координатном формате.
int input_csc_matrix_coordinate(int rows, int cols, SparseMatrixCSC_t *matrix)
{
    int non_zeros;
    printf("Введите количество ненулевых элементов: ");
    if (scanf("%d", &non_zeros) != 1 || non_zeros < 0 || non_zeros > rows * cols)
        return ERR_INPUT;

    StandardMatrix_t temp_std = {0};
    int rc = create_standard_matrix(rows, cols, &temp_std);
    if (rc) return rc;

    printf("Введите %d ненулевых элементов в формате 'строка столбец значение':\n", non_zeros);
    for (int i = 0; i < non_zeros; ++i)
    {
        int r, c, val;
        if (scanf("%d %d %d", &r, &c, &val) != 3 || r < 0 || r >= rows || c < 0 || c >= cols || val == 0)
        {
            free_standard_matrix(&temp_std);
            return ERR_INPUT;
        }
        if (temp_std.data[r][c] != 0)
            printf("Предупреждение: дублирующийся ввод для элемента (%d, %d). Предыдущее значение будет перезаписано.\n", r, c);
        temp_std.data[r][c] = val;
    }

    rc = convert_to_csc(&temp_std, matrix);
    free_standard_matrix(&temp_std);
    return rc;
}

int input_standard_matrix_coordinate(int rows, int cols, StandardMatrix_t *matrix)
{
    int non_zeros;
    printf("Введите количество ненулевых элементов: ");
    if (scanf("%d", &non_zeros) != 1 || non_zeros < 0 || non_zeros > rows * cols)
        return ERR_INPUT;

    free_standard_matrix(matrix);
    
    int rc = create_standard_matrix(rows, cols, matrix);
    if (rc) return rc;

    printf("Введите %d ненулевых элементов в формате 'строка столбец значение':\n", non_zeros);
    for (int i = 0; i < non_zeros; ++i)
    {
        int r, c, val;
        if (scanf("%d %d %d", &r, &c, &val) != 3 || r < 0 || r >= rows || c < 0 || c >= cols || val == 0)
        {
            free_standard_matrix(matrix);
            return ERR_INPUT;
        }
        if (matrix->data[r][c] != 0)
            printf("Предупреждение: дублирующийся ввод для элемента (%d, %d). Предыдущее значение будет перезаписано.\n", r, c);
        matrix->data[r][c] = val;
    }

    return 0;
}

// Генерация случайной разреженной матрицы.
int generate_sparse_matrix(int rows, int cols, double percent, SparseMatrixCSC_t *csc_matrix, StandardMatrix_t *std_matrix)
{
    if (percent < 0 || percent > 100)
        return ERR_INVALID_PERCENT;

    int rc = create_standard_matrix(rows, cols, std_matrix);
    if (rc) return rc;

    int non_zeros = (int)(rows * cols * percent / 100.0);

    for (int i = 0; i < non_zeros; ++i)
    {
        int r, c;
        do
        {
            r = rand() % rows;
            c = rand() % cols;
        } while (std_matrix->data[r][c] != 0);
        std_matrix->data[r][c] = (rand() % 100) + 1;
    }

    return convert_to_csc(std_matrix, csc_matrix);
}
