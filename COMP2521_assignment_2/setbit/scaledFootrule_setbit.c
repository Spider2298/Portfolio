#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dist.h"

#define LARGE_VALUE 1000000
#define TRUE_ '1'
#define FALSE_ '0'

//This program uses caching to reduce the search space of this problem
//Each recursive call recieves a 'used' array indicating which elements have already been incorporated
//into the 'trial' ranking
//Once the minimum cost for a particular usage combination has been calculated, the result is inserted into a BST tree
//At the beginning of each recusive call, a check is made to see whether the the minimum cost for this usage combination has
//already been calculated, if so, the previous result is returned

struct slot {
    double cost;
    int next;
};

struct slot* arr;

void setbit(int* used, int index) {
    *used = *used | (1 << index);
}

int checkbit(int used, int index) {
    return used & (1 << index);
}

void cache(int used, double cost, int next) {
    arr[used].cost = cost;
    arr[used].next = next;
}




// recursively calculate the minimum cost for arranging the n elements into n positions
double rec_rank(data d, int pos, int used, double cost) {

    int size = d->no_elements;
    // if we have already chosen all the neccesary elements, return
    if (pos == size) return 0;
    // check to see whether the resuts we are looking for has already been calculated
    // if so, return the previous result
    if (arr[used].next != -1) return arr[used].cost;
    
    // Initialize values needed to track minimum cost from this point
    double curr_min = LARGE_VALUE;
    int min_pos = 0;
    // calculate minimum cost by considering putting each element in position 'pos'
    for (int i = 0; i < size; i++) {
        if(! checkbit(used, i) ) {
            double value = rec_rank(d, pos+1, used | (1 << i), cost + d->arr[i][pos]) + d->arr[i][pos];
            if (value < curr_min) {
            	curr_min = value;
            	min_pos = i;
            }
        }
    }
    // store the result just calculated for later use
    cache(used,curr_min,min_pos);
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
	
	//if there were no elements to rank, exit
	if (d->no_elements == 0) {
	    drop(d);
	    return 0;
	}
	
	int size = (1 << d->no_elements);
	
	// create and initialize an array to track usage of elements, with extra space for a null terminator
	arr = malloc(sizeof(struct slot)*(size));           assert(arr);
	for (int i = 0; i < size; arr[i++].next = -1);
    
    // call the recursive function to calculate the minimum cost
	rec_rank(d, 0, 0, 0);
	
	
	printf("%lf\n", arr[0].cost);
	
	int curr = 0;
	while (curr != size - 1) {
		printf("%s\n",d->names[arr[curr].next]);
		setbit(&curr, arr[curr].next);
	}
	
	
	// free all relevant memory
	free(arr);
	drop(d);
}
