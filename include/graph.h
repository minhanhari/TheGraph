#ifndef GRAPH_H
#define GRAPH_H

#include "vertex.h"
#include "edge.h"
#include <stdbool.h>

struct Graph
{
  struct Vertex **vertexes;
  struct Edge **edges;
  int vertexes_num;
  int edges_num;
};

#define listLen(list) _Generic((list),             \
                               struct VertexList * \
                               : VertexListLen,    \
                                 struct EdgeList * \
                               : EdgeListLen)(list)

struct Graph *newGraph(struct VertexList *vertexes, struct EdgeList *edges);

int vertexIndex(struct Graph *graph, struct Vertex *vertex);

void InitVerticesPosition(struct Graph *graph, bool random);

void ArrangeVertexPosition(struct Graph *graph, double width, double height);

void ForceDirectedLayout(struct Graph *graph, int max_iteration);

void LocalMinimum(struct Graph *gr, double eps);

double **FloydWarshall(struct Graph *gr, bool weighted);

struct EdgeList *Dijsktra(struct Graph *graph, struct Vertex *source, struct Vertex *destination, double *distance);

double* DegreeCentrality(struct Graph *graph);

double* ClosenessCentrality(struct Graph *graph);

double* BetweennessCentrality(struct Graph *graph);

#endif // GRAPH_H
