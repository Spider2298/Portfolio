#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "graph.h"
#include "read_data.h"



// using the lists returned from read_data.h construct the graph
graph read_input(void);

//add all the edges starting from "vert" into G
void get_links(graph G, char* vert);

//calculate the weights of each of the edges in the graph by the Pagerank algorithm
double* generate_weights(graph G, double damping_factor, double min_diff, int max_iterations);

//return a list of indexes which ranks the vertices by their importance
int* generate_sorted_indexes(graph G, double* weights);

int main(int argc, char ** argv) {
    if (argc != 4) {
        fprintf(stderr,"Usage: %s [damping factor] [min_diff] [max_iterations]\n",argv[0]);
        abort();
    }
    
    // Read in arguments for generate_weight
    double damping_factor = atof(argv[1]);
    double min_diff = atof(argv[2]);
    int max_iterations = atoi(argv[3]);

    // Read URLs from collection.txt
    graph G = read_input();
    
    // Compute the weighted pagerank for each URL
    double* weights = generate_weights(G, damping_factor,min_diff,max_iterations);
    
    // Sort the weights generated from the function above
    int* sorted_indexes = generate_sorted_indexes(G, weights);

    // Create a text file called pageRankList.txt
    FILE* output = fopen("pagerankList.txt","w");
    assert(output);
    
    // Write the url, the number of outlinks and the weighted pagerank into the opened file
    for(int i = 0; i < G->nV; i++) {
        int index = sorted_indexes[i];
        fprintf(output,"%s, %d, %.7lf\n",G->map[index],G->links[index].links_out,weights[i]);
    }
    
    // Free memory associated with malloced data structures and close any existing file pointers
    free(sorted_indexes);
    free(weights);
    drop_graph(G);
    fclose(output);
    return 0;
}

// using the lists returned from read_data.h construct the graph
graph read_input(void) {

    // Read data from collection.txt
    Rep list = read_collection();

    // Create an adjacency matrix graph with size list->size
    graph G = create_graph(list->size);

    // Add vertices into the graph G
    for (Data curr = list->data_list; curr != NULL; curr = curr->next) {
        add_vertex(G,curr->info);
    }

    free_rep(list);

    // the string name of each url is now stored in the graph data structure
    // use these to create the edges in the graph
    for (int i = 0 ; i < G->nV; i++) {
        get_links(G,G->map[i]);							
    }													

    // Calculate the product of w_in and w_out and weight of edges
    count_links(G);
    caclulate_weights(G);
    return G;
}

//add all the edges starting from "vert" into G
void get_links(graph G, char* vert) {

    // Get all the outgoing links from the file vert
    Rep list = read_links(vert);

    // Add edges into the graph g
    for (Data curr = list->data_list; curr != NULL; curr = curr->next) {
        add_edge(G,vert,curr->info);
    }

    free_rep(list);
}

// calculate the weights of each of the edges in the graph by the Pagerank algorithm
double* generate_weights(graph G, double damping_factor, double min_diff, int max_iterations) {
	int size = G->nV;

    // Declare two arrays of type double
    double* new_rank = malloc(sizeof(double)*size);
    double* old_rank = malloc(sizeof(double)*size);
    
    // The initial value of the pagerank should equal to 1 divided by the number of URLs
    for(int i = 0; i < size; i++) old_rank[i] = 1.0/size;


    int count = 0;
    
    // While the limit of iterations has not been reached, continue to iterate and compute the pagerank values
    while (count < max_iterations) {
        for (int vert = 0; vert < size; vert++) {
            new_rank[vert] = (1-damping_factor)/size;
            for (int from = 0; from < size; from++) {
                if (G->arr[from][vert] != 0) {
                    new_rank[vert] += damping_factor*G->arr[from][vert]*old_rank[from];
                }
            }  
        }
        
        // While the difference is still greater than the minimum suggested value, continue to compute the pagerank values
        double diff = 0;
        for(int i = 0; i < size; i++) diff += fabs(old_rank[i]-new_rank[i]);
        if (diff < min_diff) {
            break;
        }
        for(int i = 0; i < size; i++) old_rank[i] = new_rank[i];
        count++;
    }
    
    free(old_rank);
	return new_rank;
}

//return a list of indexes which ranks the vertices by their importance
int* generate_sorted_indexes(graph G, double* weights) {
	int* indexes = malloc(sizeof(int)*G->nV);
	assert(indexes);
    for(int i = 0; i < G->nV; i++) indexes[i] = i;

    //simple bubble sort - sort indexes based on their weights
    int sorted = 0;
    int upto = G->nV;
    while (!sorted) {
        sorted = 1;
        upto--;
        for(int i = 0; i < upto; i++) {
            //sort by weights or by alphabetic order if weights are very close (don't check equality of double values using ==)
            if (weights[i] < weights[i+1] || 
                (fabs(weights[i] - weights[i+1]) < 0.0000000001 && strcmp(G->map[indexes[i]],G->map[indexes[i+1]]) > 0)) {
                double temp = weights[i];
                weights[i] = weights[i+1];
                weights[i+1] = temp;
                int i_temp = indexes[i];
                indexes[i] = indexes[i+1];
                indexes[i+1] = i_temp;
                sorted = 0;
           }
        }
    }
    return indexes;
}
