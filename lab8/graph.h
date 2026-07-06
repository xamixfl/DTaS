#ifndef GRAPH_H
#define GRAPH_H

#define MAX_VERTICES 100

/**
 * @brief Коды возврата ошибок.
 */
#define OK 0
#define ERR_MEM 1
#define ERR_FILE 2
#define ERR_VERTEX 3
#define ERR_EMPTY 4

/**
 * @brief Структура неориентированного графа (матрица симметрична).
 */
typedef struct
{
    int matrix[MAX_VERTICES][MAX_VERTICES]; /**< Матрица смежности */
    int num_vertices;                       /**< Текущее количество вершин */
} graph_t;

/**
 * @brief Создает пустой граф с заданным количеством вершин.
 */
graph_t* graph_create(int vertices);

/**
 * @brief Освобождает память под граф.
 */
void graph_free(graph_t *g);

/**
 * @brief Добавляет неориентированное ребро (в обе стороны).
 */
int graph_add_edge(graph_t *g, int src, int dest);

/**
 * @brief Загружает граф из файла (ребра добавляются как неориентированные).
 */
graph_t* graph_load_from_file(const char *filename);

/**
 * @brief Выводит матрицу смежности.
 */
void graph_print_matrix(graph_t *g);

/**
 * @brief DFS-обход.
 */
void graph_dfs(graph_t *g, int v, int *visited);

/**
 * @brief Очищает все ребра у вершины.
 */
int graph_clear_vertex(graph_t *g, int v);

/**
 * @brief Подсчёт памяти.
 */
long long graph_memory_usage(graph_t *g);

/**
 * @brief Проверяет, является ли граф связным.
 */
int graph_is_connected(graph_t *g);

/**
 * @brief Добавляет новую вершину.
 */
int graph_add_vertex(graph_t *g);

/**
 * @brief Удаляет вершину со сдвигом.
 */
int graph_delete_vertex(graph_t *g, int v);

#endif // GRAPH_H
