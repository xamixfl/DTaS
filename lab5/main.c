#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue_arr.h"
#include "queue_list.h"
#include "simulation.h"
#include "request.h"

#define ERROR_INPUT 1

int max_size_list = 0;

simulation_params_t current_params = {
    .t1_min = 0.0,      
    .t1_max = 5.0,
    .t2_min = 0.0,      
    .t2_max = 4.0,
    .t3_min = 0.0,      
    .t3_max = 4.0
};

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_error(int rc)
{
    switch (rc)
    {
        case 0:
            break;
        case ERROR_ARR_OVERFLOW:
            printf("Ошибка: Очередь (массив) переполнена.\n");
            break;
        case ERROR_EMPTY_ARR:
            printf("Ошибка: Очередь (массив) пуста.\n");
            break;
        case ERROR_LIST_OVERFLOW:
            printf("Ошибка: Очередь (список) переполнена.\n");
            break;
        case ERROR_EMPTY_LIST:
            printf("Ошибка: Очередь (список) пуста.\n");
            break;
        case ERROR_TIME_LIMIT_EXCEEDED:
            printf("Ошибка: Превышено максимальное время моделирования.\n");
            break;
        default:
            printf("Неизвестная ошибка с кодом: %d\n", rc);
            break;
    }
}

void show_menu(void)
{
    printf("\n==== Меню =====\n");
    printf("1. Запуск симуляции (очередь на массиве)\n");
    printf("2. Запуск симуляции (очередь на списке)\n");
    printf("3. Сравнить производительность и память\n");
    printf("4. Изменить параметры моделирования\n");
    printf("5. Показать текущие параметры\n");
    printf("0. Выход\n");
    printf("Введите номер действия: ");
}

// Функция изменения параметров
void change_parameters(void)
{
    printf("\n--- Изменение параметров моделирования ---\n");
    
    printf("Текущие параметры:\n");
    printf("T1 (приход заявок 1 типа): %.1f - %.1f\n", current_params.t1_min, current_params.t1_max);
    printf("T2 (обслуживание заявок 1 типа): %.1f - %.1f\n", current_params.t2_min, current_params.t2_max);
    printf("T3 (обслуживание заявок 2 типа): %.1f - %.1f\n", current_params.t3_min, current_params.t3_max);
    
    printf("\nВведите новые параметры:\n");
    
    // Ввод T1
    printf("T1_min (мин. время прихода заявки 1 типа): ");
    if (scanf("%lf", &current_params.t1_min) != 1 || current_params.t1_min < 0) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    printf("T1_max (макс. время прихода заявки 1 типа, >= %.1f): ", current_params.t1_min);
    if (scanf("%lf", &current_params.t1_max) != 1 || current_params.t1_max < current_params.t1_min) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    // Ввод T2
    printf("T2_min (мин. время обслуживания заявки 1 типа): ");
    if (scanf("%lf", &current_params.t2_min) != 1 || current_params.t2_min < 0) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    printf("T2_max (макс. время обслуживания заявки 1 типа, >= %.1f): ", current_params.t2_min);
    if (scanf("%lf", &current_params.t2_max) != 1 || current_params.t2_max < current_params.t2_min) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    // Ввод T3
    printf("T3_min (мин. время обслуживания заявки 2 типа): ");
    if (scanf("%lf", &current_params.t3_min) != 1 || current_params.t3_min < 0) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    printf("T3_max (макс. время обслуживания заявки 2 типа, >= %.1f): ", current_params.t3_min);
    if (scanf("%lf", &current_params.t3_max) != 1 || current_params.t3_max < current_params.t3_min) {
        printf("Ошибка ввода! Параметр не изменен.\n");
        clear_input_buffer();
        return;
    }
    
    clear_input_buffer();
    printf("\nПараметры успешно изменены!\n");
}

// Функция показа текущих параметров
void show_current_parameters(void)
{
    printf("\n--- Текущие параметры моделирования ---\n");
    printf("T1 (приход заявок 1 типа): %.1f - %.1f е.в.\n", current_params.t1_min, current_params.t1_max);
    printf("T2 (обслуживание заявок 1 типа): %.1f - %.1f е.в.\n", current_params.t2_min, current_params.t2_max);
    printf("T3 (обслуживание заявок 2 типа): %.1f - %.1f е.в.\n", current_params.t3_min, current_params.t3_max);
    
    // Расчет средних значений
    double avg_t1 = (current_params.t1_min + current_params.t1_max) / 2.0;
    double avg_t2 = (current_params.t2_min + current_params.t2_max) / 2.0;
    double avg_t3 = (current_params.t3_min + current_params.t3_max) / 2.0;
    
    printf("\nСредние значения:\n");
    printf("Среднее T1: %.2f е.в.\n", avg_t1);
    printf("Среднее T2: %.2f е.в.\n", avg_t2);
    printf("Среднее T3: %.2f е.в.\n", avg_t3);

    if (avg_t1 > avg_t2) {
        printf("Прогноз: система не перегружена (T1 > T2)\n");
    } else {
        printf("Прогноз: возможна перегрузка системы (T1 <= T2)\n");
    }
}

int simulation_run_array_with_params(const simulation_params_t params)
{
    printf("\nПараметры симуляции:\n");
    printf("T1: %.1f-%.1f, T2: %.1f-%.1f, T3: %.1f-%.1f\n",
           params.t1_min, params.t1_max, 
           params.t2_min, params.t2_max,
           params.t3_min, params.t3_max);

    return simulation_run_array(params);
}

int simulation_run_list_with_params(const simulation_params_t params)
{
    printf("\nПараметры симуляции:\n");
    printf("T1: %.1f-%.1f, T2: %.1f-%.1f, T3: %.1f-%.1f\n",
           params.t1_min, params.t1_max, 
           params.t2_min, params.t2_max,
           params.t3_min, params.t3_max);
    
    return simulation_run_list(params);
}

// Сравнение производительности и памяти.
void compare_performance(void)
{
    printf("\n--- Сравнение эффективности (для %d заявок 1 типа) ---\n", REQUESTS_COUNT);
    printf("Параметры: T1(%.1f-%.1f), T2(%.1f-%.1f), T3(%.1f-%.1f)\n",
           current_params.t1_min, current_params.t1_max,
           current_params.t2_min, current_params.t2_max, 
           current_params.t3_min, current_params.t3_max);

    clock_t start, end;
    double cpu_time_used;
    
    // Тестирование массива
    printf("\n1. Запуск на массиве...\n");
    start = clock();
    simulation_run_array_with_params(current_params);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    double time_array = cpu_time_used;
    
    // Тестирование списка
    printf("\n2. Запуск на списке...\n");
    start = clock();
    simulation_run_list_with_params(current_params);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    double time_list = cpu_time_used;

    // Расчет памяти
    size_t mem_array = sizeof(queue_arr_t);
    size_t mem_list_base = sizeof(queue_list_t);
    size_t mem_list_node = sizeof(node_t);
    size_t mem_list_total = mem_list_base + (max_size_list * mem_list_node);

    printf("\n| Реализация       | Время выполнения, сек | Объем памяти (Байт)                             |\n");
    printf("|------------------|-----------------------|-------------------------------------------------|\n");
    printf("| Очередь (массив) | %-21.6f | %-47zu |\n", time_array, mem_array);
    printf("| Очередь (список) | %-21.6f | %-47zu |\n", time_list, mem_list_total);
    printf(" --------------------------------------------------------------------------------------------\n\n");
    
    printf("  Память массива: фиксированная %zu байт (%d элементов)\n", mem_array, ARR_MAX_SIZE);
    printf("  Память списка: базовая структура %zu байт + узлы %zu байт каждый\n", mem_list_base, mem_list_node);
}

int main(void)
{
    srand(time(NULL));

    int choice;
    do
    {
        show_menu();
        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка: Неверный ввод данных.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        int rc = 0;
        switch (choice)
        {
            case 1:
                rc = simulation_run_array_with_params(current_params);
                break;
            case 2:
                rc = simulation_run_list_with_params(current_params);
                break;
            case 3:
                compare_performance();
                break;
            case 4:
                change_parameters();
                break;
            case 5:
                show_current_parameters();
                break;
            case 0:
                printf("Завершение работы.\n");
                break;
            default:
                printf("Ошибка: Неверный номер действия.\n");
        }
        if (rc) print_error(rc);
    } while (choice != 0);

    return 0;
}
