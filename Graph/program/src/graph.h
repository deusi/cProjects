#ifndef GRAPH_H
#define GRAPH_H
#define MAXNODE 32
#define MAXCHAR 550
#define MAXARGS 10
#define MAXCHARS 50
#define MAXEDGE 992

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// A structure to represent an adjacency list node 
struct AdjListNode {
    int dest;
    struct AdjListNode* next; 
}; 
  
// A structure to represent an adjacency list 
struct AdjList{
    //int visit;
    struct AdjListNode *head; 
}; 
  
// A structure to represent a graph. A graph 
// is an array of adjacency lists. 
// Size of array will be V (number of vertices  
// in graph) 
struct DepGraph{ 
    int V;
    int* visited;
    struct AdjList* array; 
};

// function that creates a new node for adjacency list
struct AdjListNode* newAdjListNode(int dest);

// function that creates a graph of V vertices
struct DepGraph* createGraph(int V);

// Adds an edge to an directed graph
void addEdge(struct DepGraph* graph, int src, int dest);

// DFS algorithm
void DFS(struct DepGraph* graph, int vertex, int num_vertex, FILE* out, char cmds[MAXNODE][MAXCHAR]);

void printGraph(struct DepGraph* graph);

#endif