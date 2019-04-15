#ifndef GRAPH_H
#define GRAPH_H

struct info {
    int links_in;
    int links_out;
};

typedef struct _graph {
    int nV;
    struct info* links;
    char** map;
    double** arr;
} *graph;

//allocate memory for a graph, setting all values to 0/NULL
graph create_graph(int size);

//print out the structure of the graph
void display_graph(graph G);

//add a vertex to the map field
void add_vertex(graph G, char* name);

//add a directed edge to the graph
void add_edge(graph G, char* from, char* to);

//count edges leading out and in for every vertex
void count_links(graph G);

//calculate edge weights
void caclulate_weights(graph G);

//free all memory associated with the graph
void drop_graph(graph G);

#endif
