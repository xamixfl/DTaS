#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

graph_t *graph_create(int vertices)
{
    if (vertices > MAX_VERTICES || vertices <= 0)
        return NULL;
    graph_t *g = malloc(sizeof(graph_t));
    if (!g)
        return NULL;
    g->num_vertices = vertices;
    for (int i = 0; i < vertices; i++)
        for (int j = 0; j < vertices; j++)
            g->matrix[i][j] = 0;
    return g;
}

void graph_free(graph_t *g)
{
    if (g)
        free(g);
}

// Добавляем ребро в обе стороны — граф неориентированный
int graph_add_edge(graph_t *g, int src, int dest)
{
    if (!g)
        return ERR_MEM;
    if (src < 0 || src >= g->num_vertices || dest < 0 || dest >= g->num_vertices)
        return ERR_VERTEX;
    g->matrix[src][dest] = 1;
    g->matrix[dest][src] = 1;
    return OK;
}

graph_t *graph_load_from_file(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    int v;
    if (fscanf(f, "%d", &v) != 1)
    {
        fclose(f);
        return NULL;
    }
    graph_t *g = graph_create(v);
    if (!g)
    {
        fclose(f);
        return NULL;
    }
    int src, dest;
    while (fscanf(f, "%d %d", &src, &dest) == 2)
    {
        graph_add_edge(g, src, dest);  // автоматически в обе стороны
    }
    fclose(f);
    return g;
}

void graph_print_matrix(graph_t *g)
{
    if (!g)
    {
        printf("Граф пуст или не инициализирован.\n");
        return;
    }
    printf("Матрица смежности (%d вершин):\n", g->num_vertices);
    printf("   ");
    for (int i = 0; i < g->num_vertices; i++)
        printf("%2d ", i);
    printf("\n");
    for (int i = 0; i < g->num_vertices; i++)
    {
        printf("%2d:", i);
        for (int j = 0; j < g->num_vertices; j++)  // исправлено: было i вместо j
            printf(" %2d", g->matrix[i][j]);
        printf("\n");
    }
}

void graph_dfs(graph_t *g, int v, int *visited)
{
    visited[v] = 1;
    for (int i = 0; i < g->num_vertices; i++)
    {
        if (g->matrix[v][i] == 1 && !visited[i])
            graph_dfs(g, i, visited);
    }
}

int graph_clear_vertex(graph_t *g, int v)
{
    if (!g || v < 0 || v >= g->num_vertices)
        return ERR_VERTEX;
    for (int i = 0; i < g->num_vertices; i++)
    {
        g->matrix[v][i] = 0;
        g->matrix[i][v] = 0;
    }
    return OK;
}

long long graph_memory_usage(graph_t *g)
{
    if (!g)
        return 0;
    return sizeof(graph_t);
}

// Проверка связности
int graph_is_connected(graph_t *g)
{
    if (!g || g->num_vertices <= 0)
        return 0;

    int *visited = calloc(g->num_vertices, sizeof(int));
    if (!visited)
        return 0;

    graph_dfs(g, 0, visited);  // старт с вершины 0

    int connected = 1;
    for (int i = 0; i < g->num_vertices; i++)
    {
        if (!visited[i])
        {
            connected = 0;
            break;
        }
    }

    free(visited);
    return connected;
}

int graph_add_vertex(graph_t *g)
{
    if (!g)
        return -1;
    int v = g->num_vertices;
    if (v >= MAX_VERTICES)
        return -1;
    for (int j = 0; j < v; j++)
    {
        g->matrix[v][j] = 0;
        g->matrix[j][v] = 0;
    }
    g->matrix[v][v] = 0;
    g->num_vertices++;
    return v;
}

int graph_delete_vertex(graph_t *g, int v)
{
    if (!g || v < 0 || v >= g->num_vertices)
        return ERR_VERTEX;
    if (g->num_vertices <= 0)
        return ERR_EMPTY;

    int N = g->num_vertices;

    // Сдвиг строк (вершины с индексами > v перемещаются вверх)
    for (int i = v; i < N - 1; i++)
    {
        for (int j = 0; j < N; j++)
        {
            g->matrix[i][j] = g->matrix[i + 1][j];
        }
    }

    // Сдвиг столбцов (правильно: копируем из следующего столбца той же строки)
    for (int j = v; j < N - 1; j++)
    {
        for (int i = 0; i < N; i++)  // теперь i идёт по всем строкам
        {
            g->matrix[i][j] = g->matrix[i][j + 1];
        }
    }

    g->num_vertices--;
    return OK;
}
