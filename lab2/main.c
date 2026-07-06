#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "vacation.h"


void print_error(int rc)
{
    switch (rc)
    {
        case 0:
            break;
        case ERROR_INPUT:
            printf("Ошибка ввода/вывода.\n");
            break;
        case ERROR_TOO_LONG:
            printf("Слишком длинная строка.\n");
            break;
        case ERROR_FULL_TABLE:
            printf("Таблица полная, невозможно добавить больше записей.\n");
            break;
        case ERROR_EMPTY_TABLE:
            printf("Таблица пуста.\n");
            break;
        default:
            printf("Неизвестная ошибка: %d\n", rc);
            break;
    }
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Сортирует таблицу стран напрямую методом пузырька.
void bubble_sort_direct(vacation_t *table, size_t size)
{
    int swapped;
    for (size_t i = 0; i < size - 1; i++)
    {
        swapped = 0;
        for (size_t j = 0; j < size - i - 1; j++)
        {
            if (strcmp(table[j].country, table[j + 1].country) > 0)
            {
                vacation_t temp = table[j];
                table[j] = table[j + 1];
                table[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

/*
Вспомогательная функция для быстрой сортировки (прямой).
Разделяет массив на две части относительно опорного элемента.
*/
int partition_direct(vacation_t *table, int low, int high)
{
    char pivot[STR_LEN];
    strcpy(pivot, table[high].country);
    int i = low - 1;
    for (int j = low; j <= high - 1; j++)
    {
        if (strcmp(table[j].country, pivot) < 0)
        {
            i++;
            vacation_t temp = table[i];
            table[i] = table[j];
            table[j] = temp;
        }
    }
    vacation_t temp = table[i + 1];
    table[i + 1] = table[high];
    table[high] = temp;
    return i + 1;
}

// Сортирует таблицу стран напрямую методом быстрой сортировки.
void quick_sort_direct(vacation_t *table, int low, int high)
{
    if (low < high)
    {
        int pi = partition_direct(table, low, high);
        quick_sort_direct(table, low, pi - 1);
        quick_sort_direct(table, pi + 1, high);
    }
}

// Сортирует таблицу ключей методом пузырька.
void bubble_sort_keys(KeyEntry_t *keys, size_t size)
{
    int swapped;
    for (size_t i = 0; i < size - 1; i++)
    {
        swapped = 0;
        for (size_t j = 0; j < size - i - 1; j++)
        {
            if (strcmp(keys[j].country, keys[j + 1].country) > 0)
            {
                KeyEntry_t temp = keys[j];
                keys[j] = keys[j + 1];
                keys[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

/*
Вспомогательная функция для быстрой сортировки (по ключам).
Разделяет массив ключей на две части относительно опорного элемента.
*/
int partition_keys(KeyEntry_t *keys, int low, int high)
{
    char pivot[STR_LEN];
    strncpy(pivot, keys[high].country, STR_LEN - 1);
    int i = low - 1;
    for (int j = low; j <= high - 1; j++)
    {
        if (strcmp(keys[j].country, pivot) < 0)
        {
            i++;
            KeyEntry_t temp = keys[i];
            keys[i] = keys[j];
            keys[j] = temp;
        }
    }
    KeyEntry_t temp = keys[i + 1];
    keys[i + 1] = keys[high];
    keys[high] = temp;
    return i + 1;
}

// Сортирует таблицу ключей методом быстрой сортировки.
void quick_sort_keys(KeyEntry_t *keys, int low, int high)
{
    if (low < high)
    {
        int pi = partition_keys(keys, low, high);
        quick_sort_keys(keys, low, pi - 1);
        quick_sort_keys(keys, pi + 1, high);
    }
}

// Сравнивает производительность (время и память) различных методов сортировки.
int compare_performance(vacation_t *table, size_t size)
{
    if (size == 0)
        return ERROR_EMPTY_TABLE;

    size_t test_sizes[] = {20, 40};
    size_t num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);
    const int NUM_RUNS = 100; // Количество запусков для усреднения

    for (size_t t = 0; t < num_tests; t++)
    {
        size_t cur_size = test_sizes[t];
        if (cur_size > size)
            continue;

        vacation_t table_copy[TABLE_SIZE];
        KeyEntry_t key_table[TABLE_SIZE];
        KeyEntry_t keys_copy[TABLE_SIZE];
        
        // Инициализация ключевой таблицы один раз
        for (size_t i = 0; i < cur_size; i++)
        {
            strcpy(key_table[i].country, table[i].country);
            key_table[i].original_index = i;
        }

        // Замеры для пузырьковой прямой сортировки
        double total_time_bubble_direct = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            memcpy(table_copy, table, sizeof(vacation_t) * cur_size);
            clock_t start = clock();
            bubble_sort_direct(table_copy, cur_size);
            clock_t end = clock();
            total_time_bubble_direct += ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        double time_bubble_direct = total_time_bubble_direct / NUM_RUNS;

        // Замеры для быстрой прямой сортировки
        double total_time_quick_direct = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            memcpy(table_copy, table, sizeof(vacation_t) * cur_size);
            clock_t start = clock();
            quick_sort_direct(table_copy, 0, cur_size - 1);
            clock_t end = clock();
            total_time_quick_direct += ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        double time_quick_direct = total_time_quick_direct / NUM_RUNS;

        // Замеры для пузырьковой сортировки ключей
        double total_time_bubble_keys = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            memcpy(keys_copy, key_table, sizeof(KeyEntry_t) * cur_size);
            clock_t start = clock();
            bubble_sort_keys(keys_copy, cur_size);
            clock_t end = clock();
            total_time_bubble_keys += ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        double time_bubble_keys = total_time_bubble_keys / NUM_RUNS;

        // Замеры для быстрой сортировки ключей
        double total_time_quick_keys = 0;
        for (int run = 0; run < NUM_RUNS; run++)
        {
            memcpy(keys_copy, key_table, sizeof(KeyEntry_t) * cur_size);
            clock_t start = clock();
            quick_sort_keys(keys_copy, 0, cur_size - 1);
            clock_t end = clock();
            total_time_quick_keys += ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        double time_quick_keys = total_time_quick_keys / NUM_RUNS;

        // Расчет использования памяти
        long mem_direct = cur_size * sizeof(vacation_t);
        long mem_keys_extra = cur_size * sizeof(KeyEntry_t);
        long mem_keys = mem_direct + mem_keys_extra;

        // Вывод результатов
        printf("\n--- Сравнительный анализ для %zu записей (%d запусков) ---\n\n", cur_size, NUM_RUNS);
        printf("--- Анализ по памяти ---\n");
        printf("Прямая: %ld байт\n", mem_direct);
        printf("Ключи: %ld байт (%ld таблица + %ld ключи)\n", mem_keys, mem_direct, mem_keys_extra);
        printf("Ключи требуют на %.2f%% больше памяти.\n\n", ((double)mem_keys_extra / mem_direct) * 100.0);

        printf("--- Анализ по времени (среднее за %d запусков) ---\n", NUM_RUNS);
        printf("| %-20s | %-25s | %-25s |\n", "Метод", "Пузырьковая", "Быстрая (QuickSort)");
        printf("------------------------------------------------------------------------------\n");
        printf("| %-20s | %-25.8f | %-25.8f |\n", "Прямая", time_bubble_direct, time_quick_direct);
        printf("| %-20s | %-25.8f | %-25.8f |\n", "Ключи", time_bubble_keys, time_quick_keys);
        printf("------------------------------------------------------------------------------\n");

        printf("\n--- Относительная эффективность (ключи быстрее прямой, в %%) ---\n");
        if (time_bubble_direct > 0)
        {
            double bubble_gain = (time_bubble_direct - time_bubble_keys) / time_bubble_direct * 100.0;
            printf("Пузырьковая: ключи на %.2f%% быстрее.\n", bubble_gain);
        }
        else
            printf("Пузырьковая: N/A (время = 0).\n");
            
        if (time_quick_direct > 0)
        {
            double quick_gain = (time_quick_direct - time_quick_keys) / time_quick_direct * 100.0;
            printf("Быстрая: ключи на %.2f%% быстрее.\n", quick_gain);
        }
        else
            printf("Быстрая: N/A (время = 0).\n");
    }

    return 0;
}


int main(void)
{
    srand(time(NULL));

    int rc;
    vacation_t main_table[TABLE_SIZE];
    size_t country_count = 0;
    KeyEntry_t keys[TABLE_SIZE];
    //bool is_key_table_filled = false;

    int choice;
    do
    {
        printf("\n--- Доступные действия ---\n");
        printf("1. Добавить страну\n");
        printf("2. Удалить страну\n");
        printf("3. Просмотреть таблицу стран\n");
        printf("4. Загрузить тестовые данные\n");
        printf("5. Выполнить поиск по варианту\n");
        printf("6. Сравнить эффективность сортировок\n");
        printf("7. Сортировать таблицу пузырьком (прямая)\n");
        printf("8. Сортировать таблицу quick (прямая)\n");
        printf("9. Просмотр данных по ключам (пузырек)\n");
        printf("10. Просмотр данных по ключам (quick)\n");
        printf("11. Просмотреть таблицу ключей\n");
        printf("0. Выход\n");
        printf("Введите номер действия: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Неверный ввод. Попробуйте снова.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
            case 1:
                rc = add_vacation(main_table, &country_count);
                if (rc != 0) print_error(rc);
                break;
            case 2:
                rc = delete_vacation(main_table, &country_count);
                if (rc != 0) print_error(rc);
                break;
            case 3:
                print_table(main_table, country_count);
                break;
            case 4:
                load_test_data(main_table, &country_count);
                break;
            case 5:
                rc = search_country_by_options(main_table, country_count);
                if (rc != 0) print_error(rc);
                break;
            case 6:
                rc = compare_performance(main_table, country_count);
                if (rc != 0) print_error(rc);
                break;
            case 7:
                if (country_count == 0)
                {
                    print_error(ERROR_EMPTY_TABLE);
                    break;
                }
                bubble_sort_direct(main_table, country_count);
                printf("Таблица отсортирована пузырьком (прямая).\n");
                print_table(main_table, country_count);
                break;
            case 8:
                if (country_count == 0)
                {
                    print_error(ERROR_EMPTY_TABLE);
                    break;
                }
                quick_sort_direct(main_table, 0, country_count - 1);
                printf("Таблица отсортирована quick (прямая).\n");
                print_table(main_table, country_count);
                break;
            case 9:
                if (country_count == 0)
                {
                    print_error(ERROR_EMPTY_TABLE);
                    break;
                }
                for (size_t i = 0; i < country_count; i++)
                {
                    strcpy(keys[i].country, main_table[i].country);
                    keys[i].original_index = i;
                    //is_key_table_filled = true;
                }
                bubble_sort_keys(keys, country_count);
                print_by_keys(main_table, keys, country_count);
                break;
            case 10:
                if (country_count == 0)
                {
                    print_error(ERROR_EMPTY_TABLE);
                    break;
                }
                for (size_t i = 0; i < country_count; i++)
                {
                    strncpy(keys[i].country, main_table[i].country, STR_LEN - 1);
                    keys[i].original_index = i;
                    //is_key_table_filled = true;
                }
                quick_sort_keys(keys, 0, country_count - 1);
                print_by_keys(main_table, keys, country_count);
                break;
            case 11:
                if (country_count == 0)
                {
                    print_error(ERROR_EMPTY_TABLE);
                    break;
                }
                // ВСЕГДА перестраиваем ключевую таблицу
                for (size_t i = 0; i < country_count; i++)
                {
                    strncpy(keys[i].country, main_table[i].country, STR_LEN - 1);
                    keys[i].original_index = i;
                }
                //is_key_table_filled = true;
                bubble_sort_keys(keys, country_count); // сортируем для порядка
                print_key_table(keys, country_count);
                break;
            case 0:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Неверный ввод. Попробуйте снова.\n");
        }
    }
    while (choice != 0);

    return 0;
}
