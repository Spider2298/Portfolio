#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dist.h"
#include "cache.h"

#define LARGE_VALUE 1000000
#define TRUE_ '1'
#define FALSE_ '0'


// recursively calculate the minimum cost for arranging the n elements into n positions
double rec_rank(Tree t, data d, int pos, char* used, double cost) {

    int size = d->no_elements;
    // if we have already chosen all the neccesary elements, return
    if (pos == size) return 0;
    // check to see whether the resuts we are looking for has already been calculated
    // if so, return the previous result
    node n = return_node(t, used);
    if (n != NULL) return n->value;
    // Initialize values needed to track minimum cost from this point
    double curr_min = LARGE_VALUE;
    int min_pos = 0;
    // calculate minimum cost by considering putting each element in position 'pos'
    for (int i = 0; i < size; i++) {
        if(used[i] == FALSE_) {
            used[i] = TRUE_;
            double value = rec_rank(t, d, pos+1, used, cost + d->arr[i][pos]) + d->arr[i][pos];
            if (value < curr_min) {
            	curr_min = value;
            	min_pos = i;
            }
            used[i] = FALSE_;
        }
    }
    // store the result just calculated for later use
    insertAVL_cache(t,used,curr_min,min_pos);
    return curr_min;
}





int main(int argc, char** argv) {
	if (argc == 1) {
    	fprintf(stderr,"Usage: %s [rank1] [rank2] ... [rankN]\n",argv[0]);
    	abort();
    }
	// generate the data struct containing the number of elements, the names of the elements
	// and the cost of putting each element in each position
	data d = gen_tree(argc, argv);
	
	// create and initialize an array to track usage of elements, with extra space for a null terminator
	char* used = malloc(sizeof(char)*(d->no_elements+1));           assert(used);
	for (int i = 0; i < d->no_elements; used[i++] = FALSE_);
	used[d->no_elements] = 0;
	
	// initialize the cache tree
	Tree t = create_cache_tree();
    
    // call the recursive function to calculate the minimum cost
	rec_rank(t, d, 0, used, 0);
	
	// retrieve information about the minimum way to arrange the elements from the cache tree
	node curr = t->head;
	while(curr->left != NULL) curr = curr->left;
	double min = curr->value;
	printf("%lf\n",min);
	while (curr != NULL) {
		printf("%s\n",d->names[curr->pos]);
		used[curr->pos] = TRUE_;
		curr = return_node(t,used);
	}
	
	// free all relevant memory
	free(used);
	drop_cache_tree(t);
	drop(d);
}
