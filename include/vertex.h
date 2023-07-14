#ifndef VERTEX_H
#define VERTEX_H

#include "vector.h"

struct Vertex
{
    char *name;
    struct Vector location;
};

struct Vertex *newVertex(char* name);

double getX(struct Vertex vertex);

double getY(struct Vertex vertex);

void printVertex(struct Vertex vertex);

struct VertexList
{
    struct Vertex *data;
    struct VertexList *nextPtr;
};

int VertexListLen(struct VertexList *list);

struct VertexList *addVertex(struct VertexList **list, struct Vertex *vertex);

struct VertexList *pushVertex(struct VertexList **list, struct Vertex *vertex);

struct VertexList *popVertex(struct VertexList **list);

#endif // VERTEX_H
