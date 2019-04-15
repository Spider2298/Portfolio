//Author: Michael Ellis
//implemention of the Hungarian Method (Bipartite Graph Version) as described on https://en.wikipedia.org/wiki/Hungarian_algorithm
//This code uses the same symbolic notation as the wikipedia article



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dist.h"
#include "queue.h"

#define LARGE_VALUE 100000

typedef struct graph {
    int** arr;
    int size;
    int* R_S;
    int* R_T;
    int* Z;
    double* Y;
    data d;
} * Graph;



//graph functions
Graph genGraph(int size, data d);
int isTightDirectedEdge(Graph g, int from, int to);
void dropGraph(Graph g);
void genZ(Graph g);
int attemptPathReverse(Graph g);
void displayGraph(Graph g);
void adjustPotential(Graph g);
int complete(Graph g);
void displayCostResult(Graph g);

//utility functions
int isEqual(double a, double b);


int main(int argc, char** argv) {
	if (argc == 1) {
    	fprintf(stderr,"Usage: %s [rank1] [rank2] ... [rankN]\n",argv[0]);
    	abort();
    }
	
	data d = gen_tree(argc, argv);
	int size = d->no_elements;
	
	if (size == 0) {
	    drop(d);
	    return 0;
	}
	
    Graph g = genGraph(size, d);
    
    //displayGraph(g);
    
    genZ(g);
    
    while (!complete(g)) {
        if (!attemptPathReverse(g)) {
            //printf("adjusting potential\n");
            adjustPotential(g);
        } else {
            //printf("adjusting edges\n");
        }
        //displayGraph(g);
        genZ(g);
    }
    //printf("done!\n");
    //displayGraph(g);
    displayCostResult(g);
    dropGraph(g);
}

//graph functions
Graph genGraph(int size, data d) {
    int** newArr = malloc(sizeof(int*)*size);
    assert(newArr);
    for (int i = 0; i < size; i++) {
        newArr[i] = malloc(sizeof(int)*size);
        assert(newArr[i]);
        for (int j = 0; j < size; j++) newArr[i][j] = 1;    //initially everything oriented from S to T
    }
    int* R_S = malloc(sizeof(int)*size);
    assert(R_S);
    for (int i = 0; i < size; i++) R_S[i] = 1;              //intially no vertices are covered by M
    int* R_T = malloc(sizeof(int)*size);
    assert(R_T);
    for (int i = 0; i < size; i++) R_T[i] = 1;              //intially no vertices are covered by M 
    int* Z = malloc(sizeof(int)*size*2);
    assert(Z);
    for (int i = 0; i < size*2; i++) Z[i] = 0;              //assume no vertices reachable
    
    double* Y = malloc(sizeof(double)*size*2);
    assert(Y);
    for (int i = 0; i < size*2; i++) Y[i] = 0;              //intially the potential function is 0 everywhere
    
    
    Graph newGraph = malloc(sizeof(struct graph));
    newGraph->arr = newArr;
    newGraph->size = size;
    newGraph->R_S = R_S;
    newGraph->R_T = R_T;
    newGraph->Z = Z;
    newGraph->Y = Y;
    newGraph->d = d;
    
    return newGraph;
}

int isTightDirectedEdge(Graph g, int from, int to) {
    int size = g->size;
    if (from < to) {
        assert(from >= 0);
        assert(from < size);
        assert(to >= size);
        assert(to < 2*size);
        //printf("%d, %lf, %lf\n", g->arr[from][to-size], g->d->arr[from][to-size], g->Y[from] + g->Y[to]);
        return (g->arr[from][to-size] == 1 && isEqual(g->d->arr[from][to-size], g->Y[from] + g->Y[to]));
    } else {
        assert(to >= 0);
        assert(to < size);
        assert(from >= size);
        assert(from < 2*size);
        //printf("%d, %lf, %lf\n", g->arr[to][from-size], g->d->arr[to][from-size], g->Y[from] + g->Y[to]);
        return (g->arr[to][from-size] == -1 && isEqual(g->d->arr[to][from-size], g->Y[from] + g->Y[to]));
    }
}

//calculates the set of reachable vertices generates a predecessor array as follows
//g->Z[index] = 0 if not reachable
//g->Z[index] = predecessor + 1 if reachable
void genZ(Graph g) {
    int size = g->size;
    Queue q = genQueue();
    for (int i = 0; i < size*2; i++) g->Z[i] = 0;           //all unreachable
    for (int i = 0; i < size; i++) {
        if (g->R_S[i] == 1) {
            g->Z[i] = i+1;
            enterQueue(q, i);
        }
    }
    
    while(!emptyQueue(q)) {
        int vertex = leaveQueue(q);
        int start = size, end = 2*size;
        if (vertex >= size) {
            start = 0;
            end = size;
        }
        for (int i = start; i < end; i++) {
            //printf("vertex: %d, i: %d\n",vertex, i);
            //printf("%d\n",isTightDirectedEdge(g, vertex, i));
            if (g->Z[i] == 0 && isTightDirectedEdge(g, vertex, i)) {
                g->Z[i] = vertex+1;
                enterQueue(q,i);
            }
        }
    }
    dropQueue(q);
    
}

void reverseEdge(Graph g, int from, int to) {
    int size = g->size;
    if (from < to) {
        assert(from >= 0);
        assert(from < size);
        assert(to >= size);
        assert(to < 2*size);
        g->arr[from][to-size] *=-1;
    } else {
        assert(to >= 0);
        assert(to < size);
        assert(from >= size);
        assert(from < 2*size);
        g->arr[to][from-size] *=-1;
    }
}

void adjustR(Graph g) {
    int size = g->size;
    for (int i = 0; i < size; i++) {
        g->R_S[i] = 1;
        g->R_T[i] = 1;
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (g->arr[i][j] == -1) {
                g->R_S[i] = 0;
                g->R_T[j] = 0;
            }
        }
    }
}

int attemptPathReverse(Graph g) {
    int size = g->size;
    for (int i = size; i < 2*size; i++) {
        if (g->Z[i] && g->R_T[i-size]) {
            //printf("reversing path to %d\n",i);
            int index = i;
            while(index != g->Z[index] - 1) {
                reverseEdge(g, index, g->Z[index] - 1);
                index = g->Z[index] - 1;
            }
            adjustR(g);
            return 1;
        }   
    }
    return 0;
}

void adjustPotential(Graph g) {
    int size = g->size;
    double min = LARGE_VALUE;
    for (int i = 0; i < size; i++) {
        if (g->Z[i]) {
            for (int j = size; j < 2*size; j++) {
                if (!g->Z[j]) {
                    double result = g->d->arr[i][j-size] - g->Y[i] - g->Y[j];
                    if (result < min) min = result;
                }
            }
        }
    }
    for (int i = 0; i < size; i++) {
        if (g->Z[i]) g->Y[i] += min;
        if (g->Z[i+size]) g->Y[i+size] -=min;
    }
}

int complete(Graph g) {
    for (int i = 0; i < g->size; i++) {
        if (g->R_S[i] == 1 || g->R_T[i] == 1) return 0;
    }
    return 1;
}

void displayCostResult(Graph g) {
    int size = g->size;
    double cost = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (g->arr[i][j] == -1) cost += g->d->arr[i][j];
        }
    }
    printf("Cost: %lf\n", cost);
    for(int j = 0; j < size; j++) {
        for (int i = 0; i < size; i++) {
            if (g->arr[i][j] == -1) {
                printf("%s\n",g->d->names[i]);
            }
        }
    }
}

void displayGraph(Graph g) {
    int size = g->size;
    printf("Graph:\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0 ; j < size; j++) {
            printf("%d ", g->arr[i][j]);
        }
        putchar('\n');
    }
    putchar('\n');
    
    printf("R_S:\n");
    for (int i = 0; i < size; i++) printf("%d ",g->R_S[i]); 
    putchar('\n');
    printf("R_T:\n");
    for (int i = 0; i < size; i++) printf("%d ",g->R_T[i]); 
    putchar('\n');
    printf("Z:\n");
    for (int i = 0; i < size*2; i++) printf("%d ",g->Z[i]); 
    putchar('\n');
    printf("Y:\n");
    for (int i = 0; i < size*2; i++) printf("%lf ",g->Y[i]); 
    putchar('\n');
}


void dropGraph(Graph g) {
    for (int i = 0; i < g->size; i++) {
        free(g->arr[i]);
    }
    
    free(g->arr);
    free(g->R_S);
    free(g->R_T);
    free(g->Z);
    free(g->Y);
    drop(g->d);
    free(g);
}

//utility functions

int isEqual(double a, double b) {
    double result = a-b;
    if (result < 0) result = -result;
    if (result < 0.000001) return 1;
    else return 0;
}

