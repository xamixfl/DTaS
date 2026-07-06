#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "queue_arr.h"
#include "queue_list.h"
#include "simulation.h"
#include "request.h"

#define REQUESTS_COUNT 1000
#define OUTPUT_INTERVAL 100

// Замер времени в микросекундах.
static long long get_time_mcs(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

// Генерация случайного времени в диапазоне.
static double rand_time(double min, double max)
{
    return min + (double)rand() / RAND_MAX * (max - min);
}

// Вывод промежуточной статистики.
static void print_statistics(int processed_type1, int q_size, double avg_q_len, double avg_wait_time, int entered_type1, int type2_requests)
{
    printf("\n--- Промежуточный итог (Обработано %d заявок 1 типа) ---\n", processed_type1);
    printf("Текущая длина очереди: %d\n", q_size);
    printf("Средняя длина очереди: %.2f\n", avg_q_len);
    printf("Среднее время пребывания в очереди: %.2f е.в.\n", avg_wait_time);
    printf("Вошедших заявок 1 типа: %d\n", entered_type1);
    printf("Обращений заявки 2 типа: %d\n", type2_requests);
    printf("--------------------------------------------------\n");
}

// Вывод итоговых результатов.
static void print_final_report(double total_time, int entered_type1, int exited_type1, int type2_requests, double idle_time, double avg_wait_time, simulation_params_t current_params)
{
    printf("\n========== ИТОГОВЫЙ ОТЧЕТ ==========\n");
    printf("Общее время моделирования: %.2f е.в.\n", total_time);
    printf("Время простоя аппарата (OA): %.2f е.в.\n", idle_time);
    printf("Количество вошедших заявок 1 типа: %d\n", entered_type1);
    printf("Количество вышедших заявок 1 типа: %d\n", exited_type1);
    printf("Количество обращений заявки 2 типа: %d\n", type2_requests);
    printf("Ожидаемое время моделирования: %.2f е.в.\n", 
           ((current_params.t1_min + current_params.t1_max) * exited_type1) / 2.0);
    printf("Среднее время ожидания заявок 1 типа: %.2f\n", avg_wait_time);

    // Проверка точности
    double expected_time = ((current_params.t1_min + current_params.t1_max) * exited_type1) / 2.0;
    double error_time = fabs(total_time - expected_time) / expected_time * 100;
    printf("Погрешность моделирования: %.2f%%\n", error_time);
    printf("======================================\n");
}

int simulation_run_array(const simulation_params_t current_params)
{
    long long start_sim = get_time_mcs();

    queue_arr_t queue;
    queue_arr_init(&queue);
    
    srand(time(NULL));
    
    int entered_type1 = 0;
    int exited_type1 = 0;
    int type2_requests = 0;
    
    double current_time = 0.0;
    double total_idle_time = 0.0;
    double next_arrival_type1 = rand_time(current_params.t1_min, current_params.t1_max);

    // Заявка 2-го типа начинает в ОА
    double type2_service_end = rand_time(current_params.t2_min, current_params.t2_max);
    int type2_in_service = 1;
    
    double total_wait_time_type1 = 0.0;
    double total_queue_length = 0.0;

    int total_dequeued_count = 0;
    
    printf("=== СИМУЛЯЦИЯ ОЧЕРЕДИ НА МАССИВЕ ===\n");
    
    while(exited_type1 < REQUESTS_COUNT)
    {
        // Опреледеление следующего события
        double next_event = next_arrival_type1;
        if (type2_in_service && type2_service_end < next_event) 
        {
            next_event = type2_service_end;
        }

        // Обновление статистики
        double time_delta = next_event - current_time;
        total_queue_length += queue.size * time_delta;
        if (!type2_in_service && queue.size == 0) 
        {
            total_idle_time += time_delta;
        }
        current_time = next_event;

        int event_occurred = 0;

        // 1. Приход заявки 1-го типа
        if (current_time == next_arrival_type1) 
        {
            if (queue_arr_add(&queue, 1) == 0) 
            {
                entered_type1++;
            }
            next_arrival_type1 = current_time + rand_time(current_params.t1_min, current_params.t1_max);
            event_occurred = 1;
        }

        // 2. Завершение обслуживания заявки 2-го типа
        if (type2_in_service && current_time == type2_service_end) 
        {
            type2_requests++;
            if (queue_arr_insert_4th(&queue, 2) == 0){}
            type2_in_service = 0;
            event_occurred = 1;
        }

        // 3. Начало обслуживания
        if (!type2_in_service && queue.size > 0) 
        {
            int request_type;
            if (queue_arr_del(&queue, &request_type) == 0) 
            {
                total_wait_time_type1 += current_time;
                total_dequeued_count++;

                if (request_type == 1) 
                {
                    type2_service_end = current_time + rand_time(current_params.t3_min, current_params.t3_max);
                    type2_in_service = 1;
                    exited_type1++;
                    
                    if (exited_type1 % OUTPUT_INTERVAL == 0) 
                    {
                        double avg_q_len = total_queue_length / current_time;
                        double avg_wait = total_wait_time_type1 / exited_type1;
                        print_statistics(exited_type1, queue.size, avg_q_len, avg_wait, entered_type1, type2_requests);
                    }
                } 
                else 
                {
                    type2_service_end = current_time + rand_time(0, 4);
                    type2_in_service = 1;
                }
                event_occurred = 1;
            }
        }

        // Если событий нет, двигаемся к следующему
        if (!event_occurred)
            current_time = next_event;

        if (current_time > 100000) 
        {
            return ERROR_TIME_LIMIT_EXCEEDED;
            break;
        }
    }

    double avg_wait_time = (exited_type1 > 0) ? total_wait_time_type1 / exited_type1 : 0;
    print_final_report(current_time, entered_type1, exited_type1, type2_requests, total_idle_time, avg_wait_time, current_params);

    long long end_sim = get_time_mcs();
    printf("Время выполнения: %lld мкс\n", end_sim - start_sim);
    
    return 0;
}

int simulation_run_list(const simulation_params_t current_params)
{
    int show_addresses;
    printf("Включить вывод адресов? (1 - да, 0 - нет): ");
    if (scanf("%d", &show_addresses) != 1) 
    {
        printf("Ошибка: Неверный ввод данных. Отключение вывода адресов.\n");
        show_addresses = 0;
    }

    max_size_list = 0;
    long long start_sim = get_time_mcs();

    queue_list_t queue;
    queue_list_init(&queue);
    
    srand(time(NULL));
    
    int entered_type1 = 0;
    int exited_type1 = 0;
    int type2_requests = 0;
    
    double current_time = 0.0;
    double total_idle_time = 0.0;
    double next_arrival_type1 = rand_time(current_params.t1_min, current_params.t1_max);

    // Заявка 2-го типа начинает в ОА
    double type2_service_end = rand_time(current_params.t3_min, current_params.t3_max);
    int type2_in_service = 1;
    
    double total_wait_time_type1 = 0.0;
    double total_queue_length = 0.0;

    int total_dequeued_count = 0;
    
    printf("=== СИМУЛЯЦИЯ ОЧЕРЕДИ НА СПИСКЕ ===\n");
    if (show_addresses)
        printf("Режим с выводом адресов\n");
    
    while(exited_type1 < REQUESTS_COUNT)
    {
        // Определение следующего события
        double next_event = next_arrival_type1;
        if (type2_in_service && type2_service_end < next_event)
            next_event = type2_service_end;

        // Обновляем статистику
        double time_delta = next_event - current_time;
        total_queue_length += queue.size * time_delta;
        if (!type2_in_service && queue.size == 0)
            total_idle_time += time_delta;
        current_time = next_event;

        // Обрабатываем события на текущем времени
        int event_occurred = 0;

        // 1. Приход заявки 1-го типа
        if (current_time == next_arrival_type1) 
        {
            if (queue_list_add(&queue, 1) == 0) 
            {
                entered_type1++;
                if (show_addresses && queue.pin) 
                {
                    printf("Время %.2f: Добавлена заявка 1 типа. Адрес узла: %p\n", 
                           current_time, (void*)queue.pin);
                }
            }
            next_arrival_type1 = current_time + rand_time(current_params.t1_min, current_params.t1_max);
            event_occurred = 1;
        }

        // 2. Завершение обслуживания заявки 2-го типа
        if (type2_in_service && current_time == type2_service_end) 
        {
            type2_requests++;
            
            node_t *new_node = NULL; 
            int rc = queue_list_insert_4th(&queue, 2, &new_node); 
            if (rc == 0)
            {
                if (show_addresses && queue.pin)
                    printf("Время %.2f: Заявка 2 типа возвращена в очередь. Адрес узла: %p\n", current_time, (void*)new_node);
            }
            type2_in_service = 0;
            event_occurred = 1;
        }

        // 3. Начало обслуживания
        if (!type2_in_service && queue.size > 0) 
        {
            int request_type;
            node_t* node_to_delete = queue.pout;
            if (queue_list_del(&queue, &request_type) == 0) 
            {
                if (show_addresses) 
                {
                    printf("Время %.2f: Удален узел по адресу: %p (тип заявки: %d)\n", 
                           current_time, (void*)node_to_delete, request_type);
                }
                total_wait_time_type1 += current_time;
                total_dequeued_count++;

                if (request_type == 1) 
                {
                    type2_service_end = current_time + rand_time(current_params.t3_min, current_params.t3_max);
                    type2_in_service = 1;
                    exited_type1++;
                    if (show_addresses && queue.pin) 
                        printf("Время %.2f: Добавлена заявка 1 типа. Адрес узла: %p\n", current_time, (void*)queue.pin);
                    
                    // Статистика каждые 100 заявок
                    if (exited_type1 % OUTPUT_INTERVAL == 0) 
                    {
                        double avg_q_len = total_queue_length / current_time;
                        double avg_wait = total_wait_time_type1 / exited_type1;
                        print_statistics(exited_type1, queue.size, avg_q_len, avg_wait, entered_type1, type2_requests);
                    }
                } 
                else 
                {
                    type2_service_end = current_time + rand_time(current_params.t3_min, current_params.t3_max);
                    type2_in_service = 1;
                }
                event_occurred = 1;
            }
        }

        if (!event_occurred)
            current_time = next_event;

        if (current_time > 6500) 
        {
            return ERROR_TIME_LIMIT_EXCEEDED;
        }
    }

    double avg_wait_time = (exited_type1 >= 1000) ? total_wait_time_type1 / 1000.0 : 0;
    print_final_report(current_time, entered_type1, exited_type1, type2_requests, total_idle_time, avg_wait_time, current_params);

    long long end_sim = get_time_mcs();
    printf("Время выполнения: %lld мкс\n", end_sim - start_sim);
    
    return 0;
}
