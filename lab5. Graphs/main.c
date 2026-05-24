#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_EDGES 100
#define MAX_VERTICES 100
#define START_VERTEX 1

typedef struct {
    int from;
    int to;
    int weight;
} Edge;

typedef struct {
    Edge edges[MAX_EDGES];
    int edge_count;
    int vertex_count;
} Graph;

static bool read_graph(Graph *graph) {
    printf("Введите количество вершин и рёбер: ");
    if (scanf("%d %d", &graph->vertex_count, &graph->edge_count) != 2) {
        printf("Ошибка: не удалось прочитать количество вершин и рёбер.\n");
        return false;
    }

    if (graph->vertex_count <= 0 || graph->vertex_count > MAX_VERTICES) {
        printf("Ошибка: количество вершин должно быть от 1 до %d.\n", MAX_VERTICES);
        return false;
    }

    if (graph->edge_count < 0 || graph->edge_count > MAX_EDGES) {
        printf("Ошибка: количество рёбер должно быть от 0 до %d.\n", MAX_EDGES);
        return false;
    }

    printf("Введите рёбра в формате: откуда куда вес\n");
    printf("Вершины нумеруются от 1 до %d.\n", graph->vertex_count);

    for (int i = 0; i < graph->edge_count; i++) {
        Edge *edge = &graph->edges[i];

        if (scanf("%d %d %d", &edge->from, &edge->to, &edge->weight) != 3) {
            printf("Ошибка: не удалось прочитать ребро №%d.\n", i + 1);
            return false;
        }

        if (edge->from < 1 || edge->from > graph->vertex_count || edge->to < 1 || edge->to > graph->vertex_count) {
            printf("Ошибка: в ребре №%d вершины должны быть в диапазоне от 1 до %d.\n", i + 1, graph->vertex_count);
            return false;
        }
    }

    return true;
}

static int find_min_crossing_edge(const Graph *graph, const bool visited[]) {
    int min_weight = INT_MAX;
    int min_edge_index = -1;

    for (int i = 0; i < graph->edge_count; i++) {
        int from = graph->edges[i].from;
        int to = graph->edges[i].to;
        int weight = graph->edges[i].weight;
        bool crosses_cut = (visited[from] && !visited[to]) || (!visited[from] && visited[to]);

        if (crosses_cut && weight < min_weight) {
            min_weight = weight;
            min_edge_index = i;
        }
    }

    return min_edge_index;
}

static int build_mst_prim(const Graph *graph, Edge mst_edges[], long long *total_weight) {
    bool visited[MAX_VERTICES + 1] = {false};
    int mst_edge_count = 0;

    *total_weight = 0;
    visited[START_VERTEX] = true;

    for (int step = 0; step < graph->vertex_count - 1; step++) {
        int edge_index = find_min_crossing_edge(graph, visited);

        if (edge_index == -1) {
            return -1;
        }

        Edge selected_edge = graph->edges[edge_index];
        visited[selected_edge.from] = true;
        visited[selected_edge.to] = true;
        mst_edges[mst_edge_count++] = selected_edge;
        *total_weight += selected_edge.weight;
    }

    return mst_edge_count;
}

static void print_mst(const Edge mst_edges[], int edge_count, long long total_weight) {
    printf("\nРёбра минимального остовного дерева:\n");

    for (int i = 0; i < edge_count; i++) {
        printf("%d -- %d (вес: %d)\n", mst_edges[i].from, mst_edges[i].to, mst_edges[i].weight);
    }

    printf("\nОбщий вес минимального остовного дерева = %lld\n", total_weight);
}

int main(void) {

    printf("Алгоритм минимального остовного дерева - ПРИМА для связного графа.\n");
    printf("Граф хранится как список ребер. Вершины нумеруются с 1.\n\n");

    Graph graph;
    Edge mst_edges[MAX_VERTICES - 1];
    long long total_weight;

    if (!read_graph(&graph)) {
        return 1;
    }

    int mst_edge_count = build_mst_prim(&graph, mst_edges, &total_weight);
    if (mst_edge_count == -1) {
        printf("Граф несвязный. Нельзя построить единое MST.\n");
        return 1;
    }

    print_mst(mst_edges, mst_edge_count, total_weight);
    return 0;
}
