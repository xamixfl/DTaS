#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "graph.h"
#include "graphviz.h"

#define DEFAULT_FILENAME "graph_data.txt"
#define NUM_COMPARE_OPS 100

double get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec * 1000.0 + (double) tv.tv_usec / 1000.0;
}

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_error(int rc)
{
    switch (rc)
    {
        case OK: break;
        case ERR_MEM: printf("Ошибка: Нехватка памяти или граф полон.\n");
            break;
        case ERR_FILE: printf("Ошибка: Не удалось открыть файл.\n");
            break;
        case ERR_VERTEX: printf("Ошибка: Неверный индекс вершины.\n");
            break;
        case ERR_EMPTY: printf("Ошибка: Граф пуст.\n");
            break;
        default: printf("Неизвестная ошибка: %d\n", rc);
    }
}

void ensure_default_file(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        f = fopen(filename, "w");
        if (f)
        {
            fprintf(f, "6\n");
            fprintf(f, "0 1\n0 2\n1 2\n3 4\n3 5\n");
            printf("Создан файл по умолчанию '%s' для теста.\n", filename);
            fclose(f);
        }
    } else
        fclose(f);
}

void show_menu(const char *current_filename)
{
    printf("\n--- Меню ---\n");
    printf("Текущий файл: %s\n", current_filename);
    printf("1. Загрузить граф из файла\n");
    printf("2. Вывести матрицу смежности\n");
    printf("3. Добавить вершину\n");
    printf("4. Удалить вершину\n");
    printf("5. Проверить связность графа\n");
    printf("6. Экспортировать граф в PNG\n");
    printf("7. Сравнение производительности\n");
    printf("0. Выход\n");
    printf("Введите номер действия: ");
}

int handle_load_graph(graph_t **g, char *filename_buffer)
{
    char input_name[256];
    printf("Введите имя файла (или Enter для '%s'): ", filename_buffer);

    clear_input_buffer();
    if (fgets(input_name, sizeof(input_name), stdin) != NULL)
    {
        size_t len = strlen(input_name);
        if (len > 0 && input_name[len - 1] == '\n')
            input_name[len - 1] = '\0';

        if (strlen(input_name) > 0)
            strcpy(filename_buffer, input_name);
    }

    graph_free(*g);
    *g = graph_load_from_file(filename_buffer);

    if (*g)
    {
        printf("Граф успешно загружен. Вершин: %d.\n", (*g)->num_vertices);
        return OK;
    }
    return ERR_FILE;
}

int handle_print_matrix(graph_t *g)
{
    if (!g)
        return ERR_EMPTY;
    graph_print_matrix(g);
    return OK;
}

int handle_add_vertex(graph_t *g)
{
    if (!g)
        return ERR_EMPTY;

    int new_v = graph_add_vertex(g);

    if (new_v < 0)
    {
        printf("Ошибка: Достигнут максимальный предел вершин (%d).\n", MAX_VERTICES);
        return ERR_MEM;
    }

    printf("Вершина %d успешно добавлена. Всего вершин: %d.\n", new_v, g->num_vertices);

    char choice = 'y';
    printf("Хотите добавить ребра для вершины %d? (y/n): ", new_v);

    if (scanf(" %c", &choice) != 1)
        choice = 'n';
    clear_input_buffer();

    while (choice == 'y' || choice == 'Y')
    {
        char line[256];
        int target_v = -1;

        printf("Введите индекс другой вершины (0 - %d): ", g->num_vertices - 2);

        // Надёжное чтение строки
        if (fgets(line, sizeof(line), stdin) == NULL || line[0] == '\n')
        {
            printf("Ввод отменён или ошибка. Добавление ребёр прервано.\n");
            break;
        }

        // Парсим число из строки
        if (sscanf(line, "%d", &target_v) != 1)
        {
            printf("Ошибка: Введено не число. Добавление ребёр прервано.\n");
            break;
        }

        if (target_v < 0 || target_v >= g->num_vertices)
        {
            printf("Ошибка: Неверный индекс цели.\n");
        }
        else if (target_v == new_v)
        {
            printf("Ошибка: Петли запрещены.\n");
        }
        else
        {
            graph_add_edge(g, new_v, target_v);
            printf("Ребро %d — %d успешно добавлено (неориентированное).\n", new_v, target_v);
        }

        // Спрашиваем, продолжать ли
        printf("Добавить еще ребро для вершины %d? (y/n): ", new_v);
        if (scanf(" %c", &choice) != 1)
            choice = 'n';
        clear_input_buffer();
    }

    return OK;
}

int handle_delete_vertex(graph_t *g)
{
    if (!g)
        return ERR_EMPTY;

    if (g->num_vertices == 0)
    {
        printf("Ошибка: Граф уже пуст.\n");
        return ERR_EMPTY;
    }

    int v_to_delete;
    printf("Введите индекс удаляемой вершины (0 - %d): ", g->num_vertices - 1);
    if (scanf("%d", &v_to_delete) != 1)
    {
        clear_input_buffer();
        return ERR_VERTEX;
    }

    if (g->num_vertices == 1 && v_to_delete == 0)
    {
        printf("Ошибка: Нельзя удалить единственную вершину. Используйте пункт 0 для выхода или 1 для перезагрузки.\n");
        return ERR_VERTEX;
    }

    int rc = graph_delete_vertex(g, v_to_delete);

    if (rc == OK)
    {
        printf("Вершина %d успешно удалена. Все последующие вершины сдвинуты.\n", v_to_delete);
        printf("Текущее количество вершин: %d.\n", g->num_vertices);
    }

    return rc;
}

int handle_solve_task(graph_t *g)
{
    if (!g || g->num_vertices == 0)
        return ERR_EMPTY;

    printf("\n--- Результат проверки связности ---\n");
    if (graph_is_connected(g))
        printf("Граф является связным.\n");
    else
        printf("Граф НЕ является связным.\n");

    return OK;
}

int handle_export_graph(graph_t *g)
{
    if (!g)
        return ERR_EMPTY;

    // -1 означает, что специальная раскраска не нужна (для варианта 2)
    return graph_export_to_dot(g, "graph_output");
}

graph_t *graph_generate_random(int V)
{
    if (V <= 0 || V > MAX_VERTICES)
        return NULL;
    graph_t *g = graph_create(V);
    if (!g)
        return NULL;

    int E = (V * (V - 1)) / 10;  // чуть меньше ребер для разнообразия
    if (E < V - 1)
        E = V - 1;

    for (int i = 0; i < E; i++)
    {
        int src = rand() % V;
        int dest = rand() % V;
        if (src != dest)
            graph_add_edge(g, src, dest);  // автоматически в обе стороны
    }
    return g;
}

int handle_performance_test()
{
    int sizes[] = {10, 50, 100};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n--- Сравнение производительности (%d итераций на операцию) ---\n", NUM_COMPARE_OPS);
    printf("| V | Память, байт | t_добавл, мкс | t_удален, мкс | t_DFS, мкс | t_Связность, мкс |\n");
    printf("|---|--------------|---------------|---------------|------------|------------------|\n");

    for (int i = 0; i < num_sizes; i++)
    {
        int V = sizes[i];

        graph_t *g = graph_generate_random(V);
        if (!g)
            continue;

        double total_time_add = 0;
        double total_time_delete = 0;
        double total_time_dfs = 0;
        double total_time_conn = 0;

        int *visited = malloc(sizeof(int) * V);
        if (!visited)
        {
            graph_free(g);
            return ERR_MEM;
        }

        for (int k = 0; k < NUM_COMPARE_OPS; k++)
        {
            double start, end;
            int src = rand() % V;
            int dest = rand() % V;

            start = get_time_ms();
            graph_add_edge(g, src, dest);
            end = get_time_ms();
            total_time_add += (end - start);

            int target_v = rand() % V;
            start = get_time_ms();
            graph_clear_vertex(g, target_v);
            end = get_time_ms();
            total_time_delete += (end - start);

            memset(visited, 0, sizeof(int) * V);
            start = get_time_ms();
            graph_dfs(g, 0, visited);
            end = get_time_ms();
            total_time_dfs += (end - start);

            start = get_time_ms();
            graph_is_connected(g);
            end = get_time_ms();
            total_time_conn += (end - start);
        }

        double avg_t_add = total_time_add * 1000 / NUM_COMPARE_OPS;
        double avg_t_del = total_time_delete * 1000 / NUM_COMPARE_OPS;
        double avg_t_dfs = total_time_dfs * 1000 / NUM_COMPARE_OPS;
        double avg_t_conn = total_time_conn * 1000 / NUM_COMPARE_OPS;
        long long memory = graph_memory_usage(g);

        printf("| %3d | %12lld | %13.3f | %13.3f | %10.3f | %16.3f |\n",
               V, memory, avg_t_add, avg_t_del, avg_t_dfs, avg_t_conn);

        free(visited);
        graph_free(g);
    }
    printf("\nЗамеры завершены.\n");
    return OK;
}

int main(void)
{
    graph_t *graph = NULL;
    int choice;
    char current_filename[256] = DEFAULT_FILENAME;
    int rc = OK;

    ensure_default_file(current_filename);
    srand(time(NULL));

    graph = graph_load_from_file(current_filename);
    if (graph)
        printf("Файл '%s' загружен автоматически.\n", current_filename);

    do
    {
        show_menu(current_filename);
        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка ввода. Введите число.\n");
            clear_input_buffer();
            continue;
        }

        if (choice != 1)
            clear_input_buffer();

        rc = OK;
        switch (choice)
        {
            case 1:
                rc = handle_load_graph(&graph, current_filename);
                break;
            case 2:
                rc = handle_print_matrix(graph);
                break;
            case 3:
                rc = handle_add_vertex(graph);
                break;
            case 4:
                rc = handle_delete_vertex(graph);
                break;
            case 5:
                rc = handle_solve_task(graph);
                break;
            case 6:
                rc = handle_export_graph(graph);
                break;
            case 7:
                rc = handle_performance_test();
                break;
            case 0:
                printf("Завершение работы.\n");
                break;
            default:
                printf("Неверный пункт меню.\n");
        }

        if (rc != OK)
            print_error(rc);
    } while (choice != 0);

    graph_free(graph);
    return OK;
}
