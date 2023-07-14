#ifndef EDGE_H
#define EDGE_H

#include "vertex.h"

struct Edge
{
    struct Vertex *start;
    struct Vertex *end;
    double weight;
};

struct Edge *newEdge(struct Vertex* start, struct Vertex* end, double weight);

void printEdge(struct Edge edge);

struct EdgeList
{
    struct Edge *data;
    struct EdgeList *nextPtr;
};

int EdgeListLen(struct EdgeList *list);

struct EdgeList *addEdge(struct EdgeList **list, struct Edge *edge);

struct EdgeList *pushEdge(struct EdgeList **list, struct Edge *edge); // Add to the beginning of the list

#endif // EDGE_H
