#include "sparse.h"

// Вывод сообщения об ошибке по коду возврата.
void print_error(int rc)
{
    switch (rc)
    {
        case 0:
            break;
        case ERR_INVALID_DIM:
            printf("Ошибка: неверные размеры матрицы.\n");
            break;
        case ERR_MEM_ALLOC:
            printf("Ошибка: не удалось выделить память.\n");
            break;
        case ERR_DIM_MISMATCH:
            printf("Ошибка: размеры матриц для сложения должны совпадать.\n");
            break;
        case ERR_NULL_PTR:
            printf("Критическая ошибка: передан нулевой указатель.\n");
            break;
        case ERR_INPUT:
            printf("Ошибка: неверный ввод данных.\n");
            break;
        case ERR_INVALID_PERCENT:
            printf("Ошибка: процент заполнения должен быть от 0 до 100.\n");
            break;
        default:
            printf("Неизвестная ошибка с кодом: %d\n", rc);
            break;
    }
}

// Отображение меню программы.
void show_menu(void)
{
    printf("\n--- Меню ---\n");
    printf("1. Сложение матриц в стандартной форме\n");
    printf("2. Сложение матриц в разреженной форме (CSC)\n");
    printf("3. Сравнить производительность сложения\n");
    printf("0. Выход\n");
    printf("Введите номер действия: ");
}

// Очистка буфера ввода.
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Обработка сложения матриц в стандартной форме.
void process_standard_addition(void)
{
    int rows, cols;
    printf("--- Сложение стандартных матриц ---\n");
    printf("Введите количество строк: ");
    if (scanf("%d", &rows) != 1 || rows <= 0)
    {
        print_error(ERR_INVALID_DIM);
        clear_input_buffer();
        return;
    }
    printf("Введите количество столбцов: ");
    if (scanf("%d", &cols) != 1 || cols <= 0)
    {
        print_error(ERR_INVALID_DIM);
        clear_input_buffer();
        return;
    }

    printf("\nВыберите способ ввода матриц:\n");
    printf("1. Ввод всех элементов (включая нули)\n");
    printf("2. Ввод только ненулевых элементов (координатный)\n");
    printf("Ваш выбор: ");
    int choice;
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2))
    {
        print_error(ERR_INPUT);
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    StandardMatrix_t m1 = {0};
    StandardMatrix_t m2 = {0};
    StandardMatrix_t result = {0};
    int rc = 0;

    rc = create_standard_matrix(rows, cols, &m1);
    if (rc) goto cleanup;
    rc = create_standard_matrix(rows, cols, &m2);
    if (rc) goto cleanup;

    if (choice == 1)
    {
        printf("\nМатрица 1:\n");
        rc = input_standard_matrix(&m1);
        if (rc) goto cleanup;
        printf("\nМатрица 2:\n");
        rc = input_standard_matrix(&m2);
        if (rc) goto cleanup; 
    }
    else
    {
        printf("\nМатрица 1:\n");
        rc = input_standard_matrix_coordinate(rows, cols, &m1);
        if (rc == 0)
        {
            printf("\nМатрица 2:\n");
            rc = input_standard_matrix_coordinate(rows, cols, &m2);
        }
    }
    if (rc) goto cleanup;    

    rc = add_standard_matrices(&m1, &m2, &result);
    if (rc) goto cleanup;

    printf("\n--- Результат ---\n");
    printf("Матрица 1:\n");
    print_standard_matrix(&m1);
    printf("\nМатрица 2:\n");
    print_standard_matrix(&m2);
    printf("\nСумма:\n");
    print_standard_matrix(&result);

cleanup:
    free_standard_matrix(&m1);
    free_standard_matrix(&m2);
    free_standard_matrix(&result);
    if (rc) print_error(rc);
}

// Обработка сложения матриц в разреженной форме.
void process_sparse_addition(void)
{
    int rows, cols, choice;
    printf("\n--- Сложение разреженных матриц ---\n");
    printf("Введите количество строк: ");
    if (scanf("%d", &rows) != 1 || rows <= 0)
    {
        print_error(ERR_INVALID_DIM);
        clear_input_buffer();
        return;
    }
    printf("Введите количество столбцов: ");
    if (scanf("%d", &cols) != 1 || cols <= 0)
    {
        print_error(ERR_INVALID_DIM);
        clear_input_buffer();
        return;
    }

    printf("\nВыберите способ ввода матриц:\n");
    printf("1. Ввод всех элементов (включая нули)\n");
    printf("2. Ввод только ненулевых элементов (координатный)\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2))
    {
        print_error(ERR_INPUT);
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    SparseMatrixCSC_t csc_m1 = {0}, csc_m2 = {0}, csc_result = {0};
    int rc = 0;

    if (choice == 1)
    {
        StandardMatrix_t s_m1 = {0}, s_m2 = {0};
        create_standard_matrix(rows, cols, &s_m1);
        create_standard_matrix(rows, cols, &s_m2);

        printf("\nМатрица 1:\n");
        input_standard_matrix(&s_m1);
        printf("\nМатрица 2:\n");
        input_standard_matrix(&s_m2);

        rc = convert_to_csc(&s_m1, &csc_m1);
        if (rc == 0) rc = convert_to_csc(&s_m2, &csc_m2);

        free_standard_matrix(&s_m1);
        free_standard_matrix(&s_m2);
    }
    else
    {
        printf("\nМатрица 1:\n");
        rc = input_csc_matrix_coordinate(rows, cols, &csc_m1);
        if (rc == 0)
        {
            printf("\nМатрица 2:\n");
            rc = input_csc_matrix_coordinate(rows, cols, &csc_m2);
        }
    }
    if (rc) goto cleanup;

    rc = add_csc_matrices(&csc_m1, &csc_m2, &csc_result);
    if (rc) goto cleanup;

    printf("\n--- Результат ---\n");
    printf("Матрица 1 в CSC формате:\n");
    print_csc_matrix(&csc_m1);
    printf("\nМатрица 2 в CSC формате:\n");
    print_csc_matrix(&csc_m2);
    printf("\nСумма в CSC формате:\n");
    print_csc_matrix(&csc_result);

cleanup:
    free_sparse_matrix(&csc_m1);
    free_sparse_matrix(&csc_m2);
    free_sparse_matrix(&csc_result);
    if (rc) print_error(rc);
}

// Сравнение производительности сложения.
void compare_performance(void)
{
    srand(time(NULL));
    int sizes[] = {500, 1000, 1500, 2000};
    double percents[] = {0.5, 1, 5, 10, 15, 20, 50};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    int num_percents = sizeof(percents) / sizeof(percents[0]);

    printf("\n--- Сравнение производительности сложения матриц ---\n\n");
    printf("|   Размер   |   %%   | Память станд, КБ | Память CSC, КБ | Экономия памяти, %% | Время станд, мс | Время CSC, мс |   Ускорение, %%   |\n");
    printf("|------------|-------|------------------|----------------|--------------------|-----------------|---------------|------------------|\n");

    for (int i = 0; i < num_sizes; i++)
    {
        for (int j = 0; j < num_percents; j++)
        {
            int rows = sizes[i], cols = sizes[i];
            double percent = percents[j];

            StandardMatrix_t std_m1 = {0}, std_m2 = {0}, std_res = {0};
            SparseMatrixCSC_t csc_m1 = {0}, csc_m2 = {0}, csc_res = {0};

            generate_sparse_matrix(rows, cols, percent, &csc_m1, &std_m1);
            generate_sparse_matrix(rows, cols, percent, &csc_m2, &std_m2);

            clock_t start = clock();
            for (int k = 0; k < NUM_RUNS; k++)
            {
                add_standard_matrices(&std_m1, &std_m2, &std_res);
                if (k < NUM_RUNS - 1) free_standard_matrix(&std_res);
            }
            clock_t end = clock();
            double time_std = ((double)(end - start) / CLOCKS_PER_SEC / NUM_RUNS) * 1000.0;

            start = clock();
            for (int k = 0; k < NUM_RUNS; k++)
            {
                add_csc_matrices(&csc_m1, &csc_m2, &csc_res);
                if (k < NUM_RUNS - 1) free_sparse_matrix(&csc_res);
            }
            end = clock();
            double time_csc  = ((double)(end - start) / CLOCKS_PER_SEC / NUM_RUNS) * 1000.0;

            double mem_std = (double)(rows * cols * sizeof(int) + rows * sizeof(int*)) / 1024.0;
            double mem_csc  = (double)(csc_m1.non_zeros * (sizeof(int) + sizeof(int)) + (rows + 1) * sizeof(int)) / 1024.0;

            double mem_saving_percent = (mem_std > 0.001) ? ((mem_std - mem_csc ) / mem_std) * 100.0 : 0.0;
            double time_saving_percent = (time_std > 0.000001) ? ((time_std - time_csc ) / time_std) * 100.0 : 0.0;

            char size_str[12];
            sprintf(size_str, "%dx%d", rows, cols);
            printf("| %-10s | %-5.2f | %16.2f | %14.2f | %18.2f | %15.4f | %13.4f | %16.2f |\n",
                   size_str, percent, mem_std, mem_csc , mem_saving_percent, time_std, time_csc , time_saving_percent);
            printf("----------------------------------------------------------------------------------------------------------------------------------\n");

            free_standard_matrix(&std_m1);
            free_standard_matrix(&std_m2);
            free_standard_matrix(&std_res);
            free_sparse_matrix(&csc_m1);
            free_sparse_matrix(&csc_m2);
            free_sparse_matrix(&csc_res);
        }
    }
}

// Основная функция программы.
int main(void)
{
    int choice;
    do
    {
        show_menu();
        if (scanf("%d", &choice) != 1)
        {
            print_error(ERR_INPUT);
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
            case 1:
                process_standard_addition();
                break;
            case 2:
                process_sparse_addition();
                break;
            case 3:
                compare_performance();
                break;
            case 0:
                printf("Завершение работы.\n");
                break;
            default:
                print_error(ERR_INPUT);
        }
    } while (choice != 0);

    return 0;
}
