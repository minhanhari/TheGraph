#define __STDC_WANT_LIB_EXT1__ 1
#include "graph.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

const double LENGTH = 100.0;

struct Graph *newGraph(struct VertexList *vertexes, struct EdgeList *edges)
{
    int v_num = listLen(vertexes);
    int e_num = listLen(edges);
    struct Vertex **v_list = calloc(v_num, sizeof(struct Vertex *));
    struct Edge **e_list = calloc(e_num, sizeof(struct Edge *));

    struct VertexList *v = vertexes;
    int i = 0;
    while (v != NULL)
    {
        v_list[i] = v->data;
        v = v->nextPtr;
        i++;
    }

    struct EdgeList *e = edges;
    i = 0;
    while (e != NULL)
    {
        e_list[i] = e->data;
        e = e->nextPtr;
        i++;
    }

    struct Graph *gr = malloc(sizeof(struct Graph));
    gr->vertexes = v_list;
    gr->edges = e_list;
    gr->vertexes_num = v_num;
    gr->edges_num = e_num;

    return gr;
}

//! [1] Some additional functions
int vertexIndex(struct Graph *graph, struct Vertex *vertex)
{
    for (int i = 0; i < graph->vertexes_num; i++)
    {
        if (strcmp(vertex->name, graph->vertexes[i]->name) == 0)
        {
            return i;
        }
    }
    printf("Index not found!\n");
    return graph->vertexes_num;
}

struct Edge *getEdge(struct Graph *graph, struct Vertex *v_a, struct Vertex *v_b)
{
    for (int i = 0; i < graph->edges_num; i++)
    {
        struct Edge *this_edge = graph->edges[i];
        if (this_edge->end == v_a && this_edge->start == v_b)
            return this_edge;
        else if (this_edge->end == v_b && this_edge->start == v_a)
            return this_edge;
    }
    return NULL;
}

struct VertexList *allVertexesNeighbour(struct Graph *gr, struct Vertex *v)
{
    struct VertexList *list = NULL;
    for (int i = 0; i < gr->edges_num; i++)
    {
        struct Edge *this_edge = gr->edges[i];
        if (this_edge->end == v)
            addVertex(&list, this_edge->start);
        else if (this_edge->start == v)
            addVertex(&list, this_edge->end);
    }
    return list;
}

struct EdgeList *allEdgesConnected(struct Graph *gr, struct Vertex *v)
{
    struct EdgeList *list = NULL;
    for (int i = 0; i < gr->edges_num; i++)
    {
        struct Edge *this_edge = gr->edges[i];
        if (this_edge->end == v || this_edge->start == v)
            addEdge(&list, this_edge);
    }
    return list;
}
//! [1]
//!
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void InitVerticesPosition(struct Graph *graph, bool random)
{
    int v_num = graph->vertexes_num;
    printf("Initializing vertexes' positions...\n");
    if (random)
    {
        for (int i = 0; i < v_num; i++)
        {
            graph->vertexes[i]->location.x = fRand(0, LENGTH);
            graph->vertexes[i]->location.y = fRand(0, LENGTH);
        }
    }
    else
    {
        double angle = 2.0 * M_PI / v_num;
        for (int i = 0; i < v_num; i++)
        {
            graph->vertexes[i]->location.x = cos(angle * i) * LENGTH/4 + LENGTH/4;
            graph->vertexes[i]->location.y = sin(angle * i) * LENGTH/4 + LENGTH/4;
        }
    }
    printf("Positions initialized!\n");
}

/* Arrange vertexes' positions so they will fit in drawing area */
void ArrangeVertexPosition(struct Graph *graph, double width, double height)
{
    double xMin = getX(*graph->vertexes[0]);
    double yMin = getY(*graph->vertexes[0]);
    double xMax = getX(*graph->vertexes[0]);
    double yMax = getY(*graph->vertexes[0]);

    for (int i = 0; i < graph->vertexes_num; i++)
    {
        struct Vertex v = *graph->vertexes[i];
        if (getX(v) < xMin)
            xMin = getX(v);
        if (getX(v) > xMax)
            xMax = getX(v);
        if (getY(v) < yMin)
            yMin = getY(v);
        if (getY(v) > yMax)
            yMax = getY(v);
    }

    double ratioX = width / (xMax - xMin);
    double ratioY = height / (yMax - yMin);
    double ratio = 0.9 * ((ratioX < ratioY) ? ratioX : ratioY);

    struct Vector center = devide(new_vector(xMax + xMin, yMax + yMin), 2.0);
    struct Vector offset = devide(new_vector(width, height), 2.0);

    for (int i = 0; i < graph->vertexes_num; i++)
    {
        struct Vector curr_loc = graph->vertexes[i]->location;
        graph->vertexes[i]->location = add(multiply(sub(curr_loc, center), ratio), offset);
    }
}

//! [2] Additional functions for Force-Directed Layout
/* Attraction force */
double Fa(double distance, double k)
{
    return distance * distance / k;
}

/* Repulsion force */
double Fr(double distance, double k)
{
    return k * k / distance;
}

double cool(double t)
{
    double cold_t = 0.01;
    if (t > cold_t)
    {
        return t * 0.999;
    }
    else
    {
        return cold_t;
    }
}
//! [2]
//!
void ForceDirectedLayout(struct Graph *graph, int max_iteration)
{
    int v_num = graph->vertexes_num;
    if (v_num == 0)
        return;

    double k = sqrt(LENGTH * LENGTH / v_num);
    double t = 20;

    int stop_count = 0;

    // Stop when total movement falls under a certain range
    // for (int i = 0; i < max_iteration; i++)
    while (stop_count < max_iteration)
    {
        struct Vector displacement[v_num];
        // Calculate the repulsive forces on vertexes/electrons
        for (int i = 0; i < v_num; i++)
        {
            displacement[i] = new_vector(0, 0);

            for (int j = 0; j < v_num; j++)
            {
                if (i != j)
                {
                    struct Vector diff = sub(graph->vertexes[i]->location, graph->vertexes[j]->location);
                    // displacement = displacement + (diff / |diff|) * Fr
                    displacement[i] = add(displacement[i], multiply(devide(diff, absolute(diff)), Fr(absolute(diff), k)));
                }
            }
        }

        // Calculate the attractive forces on edges/springs
        for (int i = 0; i < graph->edges_num; i++)
        {
            int start_i = vertexIndex(graph, graph->edges[i]->start);
            int end_i = vertexIndex(graph, graph->edges[i]->end);

            struct Vector diff = sub(graph->vertexes[start_i]->location, graph->vertexes[end_i]->location);
            // displacement = displacement +- (diff / |diff|) * Fa
            displacement[start_i] = sub(displacement[start_i], multiply(devide(diff, absolute(diff)), Fa(absolute(diff), k)));
            displacement[end_i] = add(displacement[end_i], multiply(devide(diff, absolute(diff)), Fa(absolute(diff), k)));
        }

        double total_displacement = 0;

        // Limit the max displacement to a temperature t and keep them inside the frame
        // The temperature t allows for large movements at the beginning of the loop
        // and smaller, more refined movements near the end. AKA Simulated Annealing
        for (int i = 0; i < v_num; i++)
        {
            struct Vector disp = displacement[i];
            struct Vector lim_disp = multiply(devide(disp, absolute(disp)), __min(absolute(disp), t));

            graph->vertexes[i]->location = add(graph->vertexes[i]->location, lim_disp);
            total_displacement += absolute(lim_disp);
        }

        // Stop when total movement falls under a certain range
        if (total_displacement < 0.05 * (v_num))
            stop_count++;

        t = cool(t);
    }
    printf("Graph placed with force-directed layout!\n");
}

//! [3] Additional functions for Force-Directed Layout
int calcDelta(struct Graph gr, double **k, double **l, double *Delta)
{
    double maxDelta = 0;
    int m_i = 0;
    for (int i = 0; i < gr.vertexes_num; i++)
    {
        // dE is vector energy
        struct Vector dE = new_vector(0, 0);
        for (int j = 0; j < gr.vertexes_num; j++)
        {
            if (i == j)
                continue;

            struct Vector d = sub(gr.vertexes[i]->location, gr.vertexes[j]->location);
            double n = 1.0 - l[i][j] / absolute(d);
            dE = add(dE, multiply(multiply(d, n), k[i][j]));
        }
        // Find vertex with highest energy
        Delta[i] = absolute(dE);
        if (Delta[i] > maxDelta)
        {
            maxDelta = Delta[i];
            m_i = i;
        }
    }
    return m_i;
}
//! [3]
//!
void LocalMinimum(struct Graph *gr, double eps)
{
    // Two dimensional array of shortest path between two vertexes
    // Calculate using Floyd-Warshall algorithm
    int v_num = gr->vertexes_num;
    int e_num = gr->edges_num;
    if (v_num <= 0 || e_num <= 0) return;

    double **d = FloydWarshall(gr, false);
    double Lo = LENGTH;
    double K = sqrt(LENGTH * LENGTH / v_num);

    double d_max = d[0][0];
    for (int i = 0; i < v_num; i++)
    {
        for (int j = i + 1; j < v_num; j++)
        {
            d_max = __max(d_max, d[i][j]);
        }
    }

    // Initializing l_ij, k_ij
    double **l = (double **)malloc(sizeof(double *) * v_num);
    double **k = (double **)malloc(sizeof(double *) * v_num);
    for (int i = 0; i < v_num; i++)
    {
        l[i] = (double *)malloc(sizeof(double) * v_num);
        k[i] = (double *)malloc(sizeof(double) * v_num);
        for (int j = 0; j < v_num; j++)
        {
            l[i][j] = Lo / d_max * d[i][j];
            k[i][j] = K / pow(d[i][j], 2);
        }
    }
    free(d);

    // Moving the vertex with highest energy decrease
    double *Delta = (double *)malloc(sizeof(double) * v_num);
    int max_i = calcDelta(*gr, k, l, Delta);
    while (Delta[max_i] > eps)
    {
        while (Delta[max_i] > eps)
        {
            struct Vector dE = new_vector(0, 0);
            double Exx = 0;
            double Exy = 0;
            double Eyy = 0;
            for (int i = 0; i < v_num; i++)
            {
                if (i == max_i)
                    continue;

                struct Vector dmax_i = sub(gr->vertexes[max_i]->location, gr->vertexes[i]->location);
                double n = 1.0 - l[max_i][i] / absolute(dmax_i);
                dE = add(dE, multiply(multiply(dmax_i, n), k[max_i][i]));

                Exy += k[max_i][i] * l[max_i][i] * dmax_i.x * dmax_i.y / pow(absolute(dmax_i), 3);
                Exx += k[max_i][i] * (1.0 - l[max_i][i] * dmax_i.y * dmax_i.y / pow(absolute(dmax_i), 3));
                Eyy += k[max_i][i] * (1.0 - l[max_i][i] * dmax_i.x * dmax_i.x / pow(absolute(dmax_i), 3));
            }

            double D = Exx * Eyy - Exy * Exy;
            struct Vector d;
            d.x = -(Eyy * dE.x - Exy * dE.y) / D;
            d.y = -(-Exy * dE.x + Exx * dE.y) / D;

            gr->vertexes[max_i]->location = add(gr->vertexes[max_i]->location, d);

            Delta[max_i] = absolute(dE);
        }

        max_i = calcDelta(*gr, k, l, Delta);
    }
    free(l);
    free(k);
    free(Delta);
    printf("Graph placed with local minimum layout!\n");
}

double **FloydWarshall(struct Graph *gr, bool weighted)
{
    int v_num = gr->vertexes_num;
    int e_num = gr->edges_num;
    if (v_num <= 0 || e_num <= 0) return NULL;

    double **dist = (double **)malloc(sizeof(double *) * v_num);
    for (int i = 0; i < v_num; i++)
    {
        dist[i] = (double *)malloc(sizeof(double) * v_num);
        for (int j = 0; j < v_num; j++)
        {
            dist[i][j] = INFINITY;
        }
    }
    for (int i = 0; i < e_num; i++)
    {
        struct Edge *edge = gr->edges[i];
        int u = vertexIndex(gr, edge->start);
        int v = vertexIndex(gr, edge->end);
        if (weighted)
            dist[u][v] = dist[v][u] = edge->weight * 10;
        else
            dist[u][v] = dist[v][u] = 1;
    }
    for (int i = 0; i < v_num; i++)
    {
        dist[i][i] = 0;
    }
    for (int k = 0; k < v_num; k++)
    {
        for (int i = 0; i < v_num; i++)
        {
            for (int j = 0; j < v_num; j++)
            {
                dist[i][j] = __min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }
    return dist;
}

struct EdgeList *Dijsktra(struct Graph *graph, struct Vertex *source, struct Vertex *target, double *distance)
{
    int v_num = graph->vertexes_num;
    if (v_num <= 1 || source == target) return NULL;
    int i_source = vertexIndex(graph, source);
    int i_target = vertexIndex(graph, target);
    if (i_source >= v_num || i_target >= v_num) return NULL;

    struct Vertex **vertexes = graph->vertexes;

    double d[v_num]; // Distance to other vertexes
    int previous[v_num];  // Index of previous vertex on the path to it
    bool visited[v_num];
    for (int i = 0; i < v_num; i++)
    {
        d[i] = INFINITY;
        previous[i] = -1;
        visited[i] = false;
    }
    d[i_source] = 0;

    bool found_destination = false;
    for (int i = 0; i < v_num && !found_destination; i++)
    {
        /* Find vertex with the lowest distance */
        double min_d = INFINITY;
        int min_i = 0;
        for (int j = 0; j < v_num; j++)
        {
            if (visited[j] == false && d[j] < min_d)
            {
                min_d = d[j];
                min_i = j;
            }
        }
        if (vertexes[min_i] == target)
            found_destination = true;
        else
        {
            visited[min_i] = true;

            struct VertexList *neighbor = allVertexesNeighbour(graph, vertexes[min_i]);
            while (neighbor != NULL)
            {
                int n_i = vertexIndex(graph, neighbor->data);
                if (n_i < v_num && visited[n_i] == false)
                {
                    double alt = d[min_i] + fabs(getEdge(graph, vertexes[min_i], neighbor->data)->weight);
                    if (alt < d[n_i])
                    {
                        d[n_i] = alt;
                        previous[n_i] = min_i;
                    }
                }
                neighbor = neighbor->nextPtr;
            }
        }
    }

    struct EdgeList *path = NULL;
    int i = i_target;
    while (previous[i] != -1)
    {
        struct Edge *edge = getEdge(graph, vertexes[i], vertexes[previous[i]]);
        pushEdge(&path, edge);
        i = previous[i];
    }

    *distance = d[i_target];
    return path;
}

double* DegreeCentrality(struct Graph *gr)
{
    int v_num = gr->vertexes_num;
    int e_num = gr->edges_num;

    double* result = calloc(v_num, sizeof(double));
    memset(result, 0, v_num * sizeof(double));

    for (int i = 0; i < e_num; i++) {
        struct Edge *edge = gr->edges[i];
        result[vertexIndex(gr, edge->start)] += edge->weight;
        result[vertexIndex(gr, edge->end)] += edge->weight;
    }
    return result;
}

double *ClosenessCentrality(struct Graph *gr)
{
    int v_num = gr->vertexes_num;
    double* result = calloc(v_num, sizeof(double));
    memset(result, 0, v_num * sizeof(double));

    double **distance = FloydWarshall(gr, true);

    for (int i = 0; i < v_num; ++i) {
        for (int j = 0; j < v_num; ++j) {
            result[i] += distance[i][j] / 10;
        }
        result[i] = (gr->vertexes_num - 1) / result[i];
    }
    free(distance);

    return result;
}

double *BetweennessCentrality(struct Graph *gr)
{
    int v_num = gr->vertexes_num;
    double *C = calloc(v_num, sizeof(double));
    memset(C, 0, v_num * sizeof(double));

    for (int cur_vertex = 0; cur_vertex < v_num; ++cur_vertex) {
        struct VertexList *S = NULL;
        struct VertexList *P[v_num];
        double sigma[v_num], delta[v_num];

        double dist[v_num];

        for (int j = 0; j < v_num; ++j) {
            sigma[j] = 0;
            delta[j] = 0;
            dist[j] = -1;
            P[j] = NULL;
        }
        sigma[cur_vertex] = 1; dist[cur_vertex] = 0;

        struct VertexList *Q = NULL;
        addVertex(&Q, gr->vertexes[cur_vertex]);

        while (Q != NULL) {
            struct Vertex *v = popVertex(&Q)->data;
            pushVertex(&S, v);

            struct EdgeList *connected = allEdgesConnected(gr, v);
            while (connected != NULL) {
                struct Vertex *w = (v == connected->data->start ? connected->data->end : connected->data->start);
                int i_v = vertexIndex(gr, v);
                int i_w = vertexIndex(gr, w);
                if (i_v >= v_num || i_w >= v_num) continue;

                if (dist[i_w] < 0) {
                    addVertex(&Q, w);
                    dist[i_w] = dist[i_v] + connected->data->weight;
                }

                if (dist[i_w] == dist[i_v] + connected->data->weight) {
                    sigma[i_w] += sigma[i_v];
                    addVertex(&P[i_w], v);
                }
                connected = connected->nextPtr;
            }
        }

        while (S != NULL) {
            struct Vertex *w = popVertex(&S)->data;
            int i_w = vertexIndex(gr, w);
            if (i_w >= v_num) continue;

            struct VertexList *list = P[i_w];
            while (list != NULL) {
                int i_v = vertexIndex(gr, list->data);
                if (i_v >= v_num) continue;
                delta[i_v] += sigma[i_v] / sigma[i_w] * (1 + delta[i_w]);

                if (w != gr->vertexes[cur_vertex])
                    C[i_w] += delta[i_w];

                list = list->nextPtr;
            }
        }
    }
    return C;
}
