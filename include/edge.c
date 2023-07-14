#include "edge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Edge* newEdge(struct Vertex* _start, struct Vertex* _end, double _weight)
{
    if (strcmp(_start->name, _end->name) == 0) return NULL;

    struct Edge *e = (struct Edge *)malloc(sizeof(struct Edge));
    e->start = _start; e->end = _end; e->weight = _weight;

    return e;
}

void printEdge(struct Edge edge)
{
    printf("From: ");
    printVertex(*edge.start);
    printf("To  : ");
    printVertex(*edge.end);
    printf("W   : %f", edge.weight);
}

int EdgeListLen(struct EdgeList *list)
{
    int out = 0;
    while (list != NULL)
    {
        out++;
        list = list->nextPtr;
    }
    return out;
}

struct EdgeList *addEdge(struct EdgeList **list, struct Edge *edge)
{
    // create the edge to add
    struct EdgeList *toAdd = (struct EdgeList *)malloc(sizeof(struct EdgeList));
    toAdd->data = edge;
    toAdd->nextPtr = NULL;

    if (*list == NULL)
    {
        *list = toAdd;
    }
    else
    {
        struct EdgeList *lastEdge = *list;

        while (lastEdge->nextPtr != NULL)
        {
            struct Vertex *st_v = lastEdge->data->start;
            struct Vertex *en_v = lastEdge->data->end;
            if (strcmp(st_v->name, edge->start->name) == 0 && strcmp(en_v->name, edge->end->name) == 0)
            {
                //printf("Edge %s - %s already exist.", edge->start->name, edge->end->name);
                free(toAdd);
                return lastEdge;
            }
            else if (strcmp(st_v->name, edge->end->name) == 0 && strcmp(en_v->name, edge->start->name) == 0)
            {
                //printf("Edge %s - %s already exist.\n", edge->start->name, edge->end->name);
                free(toAdd);
                return lastEdge;
            }
            else
                lastEdge = lastEdge->nextPtr;
        }
        struct Vertex *st_v = lastEdge->data->start;
        struct Vertex *en_v = lastEdge->data->end;
        if (strcmp(st_v->name, edge->start->name) == 0 && strcmp(en_v->name, edge->end->name) == 0)
        {
            //printf("Edge %s - %s already exist.\n", edge->start->name, edge->end->name);
            free(toAdd);
            return lastEdge;
        }
        else if (strcmp(st_v->name, edge->end->name) == 0 && strcmp(en_v->name, edge->start->name) == 0)
        {
            //printf("Edge %s - %s already exist.\n", edge->start->name, edge->end->name);
            free(toAdd);
            return lastEdge;
        }
        else
            lastEdge->nextPtr = toAdd;
    }

    //printf("Edge %s - %s %f added.\n", edge->start->name, edge->end->name, edge->weight);
    return toAdd;
}

struct EdgeList *pushEdge(struct EdgeList **list, struct Edge *edge)
{
    // create the edge to add
    struct EdgeList *toAdd = (struct EdgeList *)malloc(sizeof(struct EdgeList));
    toAdd->data = edge;
    toAdd->nextPtr = NULL;

    struct EdgeList *lastEdge = *list;
    while (lastEdge != NULL)
    {
        struct Vertex *st_v = lastEdge->data->start;
        struct Vertex *en_v = lastEdge->data->end;
        if (strcmp(st_v->name, edge->start->name) == 0 && strcmp(en_v->name, edge->end->name) == 0)
        {
            //printf("Edge %s - %s already exist.\n", edge->start->name, edge->end->name);
            free(toAdd);
            return lastEdge;
        }
        else if (strcmp(st_v->name, edge->end->name) == 0 && strcmp(en_v->name, edge->start->name) == 0)
        {
            //printf("Edge %s - %s already exist.\n", edge->start->name, edge->end->name);
            free(toAdd);
            return lastEdge;
        }
        else
            lastEdge = lastEdge->nextPtr;
    }

    toAdd->nextPtr = *list;
    *list = toAdd;

    //printf("Edge %s - %s %f added.\n", edge->start->name, edge->end->name, edge->weight);
    return toAdd;
}
