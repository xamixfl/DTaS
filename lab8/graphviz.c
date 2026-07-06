#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphviz.h"

// Локальная реализация DFS для поиска компонент связности
static void dfs_component(graph_t *g, int v, int *visited, int component_id, int *component)
{
    visited[v] = 1;
    component[v] = component_id;
    for (int i = 0; i < g->num_vertices; i++)
    {
        if (g->matrix[v][i] && !visited[i])
            dfs_component(g, i, visited, component_id, component);
    }
}

int graph_export_to_dot(graph_t *g, const char *filename)
{
    if (!g || g->num_vertices == 0)
        return ERR_MEM;

    char dot_filename[256];
    char png_filename[256];
    char command[1024];

    snprintf(dot_filename, sizeof(dot_filename), "%s.dot", filename);
    snprintf(png_filename, sizeof(png_filename), "%s.png", filename);

    FILE *f = fopen(dot_filename, "w");
    if (!f)
    {
        printf("Ошибка: Не удалось создать файл %s\n", dot_filename);
        return ERR_FILE;
    }

    // Неориентированный граф
    fprintf(f, "graph G {\n");
    fprintf(f, "    rankdir=LR;\n");
    fprintf(f, "    node [shape=circle, style=filled, fontname=\"Arial\"];\n");
    fprintf(f, "    edge [dir=none];\n");  // ребра без стрелок

    // Определяем компоненты связности и окрашиваем их
    int *visited = calloc(g->num_vertices, sizeof(int));
    int *component = malloc(g->num_vertices * sizeof(int));
    if (!visited || !component)
    {
        free(visited);
        free(component);
        fclose(f);
        return ERR_MEM;
    }
    memset(component, 0, g->num_vertices * sizeof(int));

    const char *colors[] = {
        "lightblue", "lightgreen", "salmon", "khaki", "plum", "wheat"
    };
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int component_id = 0;

    for (int i = 0; i < g->num_vertices; i++)
    {
        if (!visited[i])
        {
            component_id++;
            dfs_component(g, i, visited, component_id, component);
        }
    }

    // Окрашиваем вершины по компонентам
    for (int i = 0; i < g->num_vertices; i++)
    {
        int color_idx = (component[i] - 1) % num_colors;
        fprintf(f, "    %d [fillcolor=\"%s\", label=\"%d\"];\n",
                i, colors[color_idx], i);
    }

    // Добавляем ребра (только верхний треугольник, чтобы не дублировать)
    for (int i = 0; i < g->num_vertices; i++)
    {
        for (int j = i + 1; j < g->num_vertices; j++)  // i+1 чтобы не рисовать дважды
        {
            if (g->matrix[i][j])
                fprintf(f, "    %d -- %d;\n", i, j);
        }
    }

    fprintf(f, "}\n");
    fclose(f);

    free(visited);
    free(component);

    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dot_filename, png_filename);

    printf("Генерация изображения: %s\n", command);
    int rc = system(command);

    if (rc == 0)
    {
        if (component_id == 1)
            printf("Граф связный. Экспортирован в '%s' (все вершины одного цвета).\n", png_filename);
        else
            printf("Граф не связный (%d компонент(ы)). Экспортирован в '%s' с раскраской компонент.\n",
                   component_id, png_filename);
    }
    else
        printf("Внимание: Ошибка запуска утилиты 'dot'. Убедитесь, что Graphviz установлен.\n");

    return OK;
}
