#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "strdup.h"
#include "graph.h"

//Michael Ellis z5215441 10/10/18
//This is an implementation of an adjacency matrix representation for graphs
//the graph has a 'map' field which relates the indexes of vertices in the graph to 
//a string representing the name of the url



//helper functions//


//convert a name to an integer ID
static int name_to_ID(graph G, char* name) {
    for (int i = 0; i < G->nV; i++) {
        if (strcmp(G->map[i],name) == 0) return i;
    }
    abort();
    return -1;				//return an invalid vertex index
}

//check vertices are valid
static int validV(graph G, int vertex) {
    return (G != NULL && vertex >= 0 && vertex < G->nV);
}



//creation functions//


//allocate memory for a graph, setting all values to 0/NULL
graph create_graph(int size) {
    graph G = malloc(sizeof(struct _graph));
    assert(G);
    G->nV = size;

	// Allocate memory for the adjacency matrix
    G->arr = malloc(sizeof(double*)*size);
    assert(G->arr);
    for(int i = 0; i < size; i++) {
        G->arr[i] = malloc(sizeof(double)*size);
        assert(G->arr[i]);
        for(int j = 0; j < size; j++) G->arr[i][j] = 0;
    }

	// Allocate memory for an array of strings that is used to map indexes to URL names
    G->map = malloc(sizeof(char*) *size);
    assert(G->map);
    for(int i = 0; i < size; i++) G->map[i] = NULL;

	//  Create an array of structs that stores the value of in weights and out weights
    G->links = malloc(sizeof(struct info) * size);
    assert(G->links);
    for(int i = 0; i < size; i++) {
        G->links[i].links_in = 0;
        G->links[i].links_out = 0;
    }

    return G;
}

//add a vertex to the map field
void add_vertex(graph G, char* name) {
    int index = 0;
    while (G->map[index] != NULL) {
        index++;
        if (index == G->nV) {
            fprintf(stderr,"Too many vertices added\n");
            abort();
        }
    }
    G->map[index] = custom_strdup(name);
}

//add a directed edge to the graph
void add_edge(graph G, char* from, char* to) {
    int from_ID = name_to_ID(G, from);
    int to_ID = name_to_ID(G, to);
    if (from_ID == to_ID) return;						//ignore loops
    if (!(validV(G, from_ID) && validV(G,to_ID))) {
        fprintf(stderr, "Invalid vertex\n");
        abort();
    }
    G->arr[from_ID][to_ID] = 1;
}



//display functions//


//print out the structure of the graph
void display_graph(graph G) {

    for(int i = 0; i < G->nV; i++) {
        printf("[%s]->",G->map[i]);
        for(int j = 0; j < G->nV; j++) {
            if (G->arr[i][j] != 0) printf("[%s]->",G->map[j]);
        }
        printf("[X]\n");
        //printf("links_out: %d\n",G->links[i].links_out);
        //printf("links_in: %d\n",G->links[i].links_in);
    }

}



//graph processing functions//


//count edges leading out and in for every vertex
void count_links(graph G) {
    for (int vert = 0; vert < G->nV; vert++) {
        for (int dest = 0; dest < G->nV; dest++) {
            if (G->arr[vert][dest] != 0) {
            	G->links[vert].links_out++;
            	G->links[dest].links_in++;
            }
        }
    }
}

//calculate edge weights
void caclulate_weights(graph G) {
	// Iterate through all the edges in the graph
    for (int i = 0; i < G->nV; i++) {
        for (int j = 0; j < G->nV; j++) {
        	// If there's no edge, continue
            if (G->arr[i][j] == 0) continue;
            
            // Compute the product of weight in and weight out
            double I_j = G->links[j].links_in;
            double sum_I_p = 0;
            for (int k = 0; k < G->nV; k++) {
                if (G->arr[i][k] != 0) sum_I_p += G->links[k].links_in;
            }
            
            // Calculate the sum of inlinks
            double O_j = G->links[j].links_out > 0 ? G->links[j].links_out : 0.5;
            double sum_O_p = 0;
            for (int k = 0; k < G->nV; k++) {
                if (G->arr[i][k] != 0) {
                    double value = G->links[k].links_out;
                    sum_O_p += value > 0 ? value : 0.5;
                }
            }
            //printf("%d %d\n",i,j);
            double W_in = I_j/sum_I_p;
            double W_out = O_j/sum_O_p;

            //printf("W_in: %.7lf\n",W_in);
            //printf("W_out: %.7lf\n",W_out);
            double weight = W_in * W_out;
            
            // Assign the weight to the edge
            G->arr[i][j] = weight;
        }
    }
}

//free all memory associated with the graph
void drop_graph(graph G) {
    for (int i = 0; i < G->nV; i++) {
        free(G->map[i]);
        free(G->arr[i]);
    }
    free(G->links);
    free(G->arr);
    free(G->map);
    free(G);
}
