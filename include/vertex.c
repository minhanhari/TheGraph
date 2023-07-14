#include "vertex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Vertex *newVertex(char* _name)
{
    struct Vertex *new = (struct Vertex*)malloc(sizeof(struct Vertex));
    new->name = _name;
    new->location = new_vector(0, 0);

    return new;
}

double getX(struct Vertex vertex)
{
    return vertex.location.x;
}

double getY(struct Vertex vertex)
{
    return vertex.location.y;
}

void printVertex(struct Vertex vertex)
{
    printf("%s\n", vertex.name);
}

int VertexListLen(struct VertexList *list)
{
    int out = 0;
    while (list != NULL)
    {
        out++;
        list = list->nextPtr;
    }
    return out;
}

struct VertexList *addVertex(struct VertexList **list, struct Vertex *vertex)
{
    // create the vertex to add
    struct VertexList *toAdd = (struct VertexList *)malloc(sizeof(struct VertexList));
    toAdd->data = vertex;
    toAdd->nextPtr = NULL;

    if (*list == NULL)
    {
        *list = toAdd;
    }
    else
    {
        struct VertexList *lastVertex = *list;

        while (lastVertex->nextPtr != NULL)
        {
            if (strcmp(lastVertex->data->name, vertex->name) == 0)
            {
                //printf("Vertex %s already exist.\n", vertex->name);
                free(toAdd);
                return lastVertex;
            }
            else
                lastVertex = lastVertex->nextPtr;
        }
        if (strcmp(lastVertex->data->name, vertex->name) == 0)
        {
            //printf("Vertex %s already exist.\n", vertex->name);
            free(toAdd);
            return lastVertex;
        }
        else
            lastVertex->nextPtr = toAdd;
    }

    //printf("Vertex %s added.\n", vertex->name);
    return toAdd;
}

struct VertexList *pushVertex(struct VertexList **list, struct Vertex *vertex)
{
    // create the vertex to add
    struct VertexList *toAdd = (struct VertexList *)malloc(sizeof(struct VertexList));
    toAdd->data = vertex;
    toAdd->nextPtr = NULL;

        struct VertexList *lastVertex = *list;

        while (lastVertex != NULL)
        {
            if (strcmp(lastVertex->data->name, vertex->name) == 0)
            {
                //printf("Vertex %s already exist.\n", vertex->name);
                free(toAdd);
                return lastVertex;
            }
            else
                lastVertex = lastVertex->nextPtr;
        }
        toAdd->nextPtr = *list;
        *list = toAdd;

    //printf("Vertex %s added.\n", vertex->name);
    return toAdd;
}

struct VertexList *popVertex(struct VertexList **list)
{
    struct VertexList *toPop = *list;
    *list = (*list)->nextPtr;
    toPop->nextPtr = NULL;
    //printf("Vertex %s removed.\n", toPop->data->name);

    return toPop;
}
