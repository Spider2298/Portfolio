#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "dist.h"
#include "strdup.h"
//to fix compatibility issues across devices, custom_strdup from strdup.c is used
//instead of strdup from the string.h library

#define MAX_WORD_SIZE 50


// all type definitions are internal, so as to not conflict with other tree implementations
typedef struct _Tree {
    struct _node* head;
    int no_elements;
    int no_sets;
    int* set_sizes;
    struct _node** node_arr;
} *Tree;

typedef struct _node {
    char* word;
    struct _set_node* head;
    int height;
    struct _node* left;
    struct _node* right;
} *node;

//links for a linked list containing information on all the positions a
//particular element in is
typedef struct _set_node {
    int set;
    int position;
    struct _set_node* next;
} *set_node;



//all functions static to not conflict with the other, similar Tree files being used

//allocate and return a tree
static Tree create_tree(void);

//perform an insertion maintaining its balance
static void insertAVL(Tree, char* word, int set, int position);

//return an array of pointers to nodes in the tree in order
static node* return_array(Tree t);

//free all memory associated with the tree
static void drop_tree(Tree t);

//generate a data struct from the tree
static data return_data(Tree t);

// generate the data struct containing the number of elements, the names of the elements
// and the cost of putting each element in each position
data gen_tree(int argc, char** argv) {
    assert(argc > 1);
	char buffer[MAX_WORD_SIZE];
	// create and intialize an array indicating size of sets (rank lists)
	int* set_sizes = malloc(sizeof(int)*(argc - 1));
	assert(set_sizes);
	for (int i = 0; i < argc - 1; set_sizes[i++] = 0);
	//create a intialize the tree used to perform cost calculations
	Tree t = create_tree();
	t->set_sizes = set_sizes;
	t->no_sets = argc - 1;
	//open every file provided on by user, and read ranked items into tree, noting their position and the set they are in
	for (int set = 0; set < argc - 1; set++) {
		FILE* input = fopen(argv[set + 1],"r");
		assert(input);
		int position = 1;
		while (fscanf(input,"%s",buffer) == 1) {
			insertAVL(t,buffer,set,position++);
			t->set_sizes[set]++;
		}
		fclose(input);
	}
	//return an array of pointers to nodes in the tree in order
	t->node_arr = return_array(t);
	return return_data(t);
}

//calculate the cost of putting element c in position p
double W(Tree t, int c, int p) {
	// initialize variables, mainly for easy reading
    double no_elements = t->no_elements;
    int* set_sizes = t->set_sizes;
    node* node_arr = t->node_arr;
    double cost = 0;
    
    //calculate the weighted footrule value
    for (set_node curr = node_arr[c]->head; curr != NULL; curr = curr->next) {
        cost += fabs(curr->position/((double)set_sizes[curr->set]) - p/no_elements);
    }
    return cost;
}

//generates a returns a pointer to a 'data' struct with information on the total number of distinct elements to order
//their names, and the the cost of putting the cth elements in position p, indexed in the array as arr[c][p] where c and p are
//0-indexed
data return_data(Tree t) {
    data object = malloc(sizeof(struct _data));
    assert(object);
    //malloc memory for and populate the cost array
    double** arr = malloc(sizeof(double*)*(t->no_elements));
    assert(arr);
    for (int i = 0; i < t->no_elements; i++) {
        arr[i] = malloc(sizeof(double)*(t->no_elements));
        assert(arr[i]);
        for (int j = 0; j < t->no_elements; j++) {
        	//need to call with j+1 as the W function expectes the position to be 1 - indexed
            arr[i][j] = W(t,i,j+1);
        }
    }
    //malloc and initialize the names array
    char** names = malloc(sizeof(char*)*(t->no_elements));
    assert(names);
    for (int i = 0; i < t->no_elements; i++) {
        names[i] = custom_strdup(t->node_arr[i]->word);
        assert(names[i]);
    }
    //fill the data struct
    object->no_elements = t->no_elements;
    object->arr = arr;
    object->names = names;
    //no longer have any need of the tree
    drop_tree(t);                               
    return object;
}

//drop the data struct
void drop(data d) {
    int size = d->no_elements;
    for (int i = 0; i < size; i ++) {
        free(d->names[i]);
        free(d->arr[i]);
    }
    free(d->names);
    free(d->arr);
    free(d);
}



//tree function similar to others in this assignment
//keys are the url strings
//each node points on a linked list of all the different sets in which that url is found
//each node cotains information of the index of the set and the element's position within that set





//tree functions//

//helper functions
static int max(int a, int b) {
    return a>b?a:b;
}

// Return the height of the tree
static int height(node curr) {
    if (curr == NULL) return 0;
    return curr->height;
}

// Recalculate the height of a given node
static void update_height(node curr) {
    curr->height = max(height(curr->left),height(curr->right)) + 1;
}

// Implement a right rotation to reduce the height of the left subtree. 
// Additionally, update height of relevant nodes
static node rotate_right(node curr) {
    if (curr == NULL || curr->left == NULL) return curr;
    node left = curr->left;
    node right_of_left = left->right;
    left->right = curr;
    curr->left = right_of_left;
    update_height(curr);
    update_height(left);
    return left;
}

// Implement a left rotation to reduce the height of the right subtree. 
// Additionally, update height of relevant nodes
static node rotate_left(node curr) {
    if (curr == NULL || curr->right == NULL) return curr;
    node right = curr->right;
    node left_of_right = right->left;
    right->left = curr;
    curr->right = left_of_right;
    update_height(curr);
    update_height(right);
    return right;
}




//creation function
static Tree create_tree(void) {
    Tree new_Tree = malloc(sizeof(struct _Tree));
    assert(new_Tree);
    new_Tree->head = NULL;
    new_Tree->no_elements = 0;
    new_Tree->no_sets = 0;
    new_Tree->set_sizes = NULL;
    new_Tree->node_arr = NULL;
    return new_Tree;
}



//output functions

//header function for internal recursive version
static node* return_array(Tree t); 
//allocate and return an array of pointers to nodes in the tree                                    
static void recursive_return_array(node curr, node* arr, int* index);  

//header function for internal recursive version
static node* return_array(Tree t) {
    node* arr = malloc(sizeof(node)*t->no_elements);
    assert(arr);
    int index = 0;
    //pass in a pointer to index which is incremented every time we add an
    //element to the array
    recursive_return_array(t->head, arr, &index);
    return arr;
}

//allocate and return an array of pointers to nodes in the tree
static void recursive_return_array(node curr, node* arr, int* index) {
    if (curr != NULL) {
        recursive_return_array(curr->left, arr, index);
        arr[(*index)++] = curr;
        recursive_return_array(curr->right, arr, index);
    }
}




//insertion functions

//header function for the recursive function
static void insertAVL(Tree t, char* word, int set, int position);					 
//find the position to insert the new node
static node recursive_insertAVL(Tree t, node curr, char* word, int set, int position);	
//create the new node
static node create_node(char* word, int set, int position);
//update the list of urls that contain this word						    
static void insert_set(node parent, int set, int position);	
//create a url node to contain url string				    
static set_node create_set_node(int set, int position);						    

	
//header function for the recursive function
static void insertAVL(Tree t, char* word, int set, int position) {
    t->head = recursive_insertAVL(t,t->head,word,set,position);
}

//find the position to insert the new node
static node recursive_insertAVL(Tree t, node curr, char* word, int set, int position) {
	//Inserting at leaf
    if (curr == NULL) {
        t->no_elements++;
        return create_node(word,set,position);
    }
    //Update set list
    if (strcmp(word,curr->word) == 0) {
        insert_set(curr,set,position);
        return curr;
    }
    // Inserting into the right subtree
    if (strcmp(word,curr->word) > 0) curr->right = recursive_insertAVL(t, curr->right, word, set, position);
    // Inserting into the left subtree
    else if (strcmp(word,curr->word) < 0) curr->left = recursive_insertAVL(t, curr->left, word, set, position);
    update_height(curr);
    //this section takes care of the rebalancing
    if (height(curr->left) - height(curr->right) > 1) {
        if (strcmp(word, curr->left->word) > 0) {
            curr->left = rotate_left(curr->left);
        }
        curr = rotate_right(curr);
    }
    if (height(curr->right) - height(curr->left) > 1) {
        if (strcmp(word, curr->right->word) < 0) {
            curr->right = rotate_right(curr->right);
        }
        curr = rotate_left(curr);
    }
    return curr;
}

//create the new node
static node create_node(char* word, int set, int position) {
    node new_node = malloc(sizeof(struct _node));
    assert(new_node);
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->word = custom_strdup(word);
    new_node->head = NULL;
    new_node->height = 1;
    insert_set(new_node,set,position);
    return new_node;
}

//update the list of urls that contain this word						    
static void insert_set(node parent, int set, int position) {
    set_node new_node = create_set_node(set,position);
    new_node->next = parent->head;
    parent->head = new_node;
}

//create a url node to contain url string				    
static set_node create_set_node(int set, int position) {
    set_node new_node = malloc(sizeof(struct _set_node));
    assert(new_node);
    new_node->next = NULL;
    new_node-> set = set;
    new_node->position = position;
    return new_node;
}




//drop functions

//header function for recursive function
static void drop_tree(Tree t);
//calls itself on left, right, and then frees itself					
static void recursive_drop_tree(node curr);
//drop the linked list of urls	
static void drop_set_list(set_node curr);		

//header function for recursive function
static void drop_tree(Tree t) {
    recursive_drop_tree(t->head);
    free(t->set_sizes);
    free(t->node_arr);
    free(t);
}

//calls itself on left, right, and then frees the current node
static void recursive_drop_tree(node curr) {
    if (curr == NULL) return;
    recursive_drop_tree(curr->right);
    recursive_drop_tree(curr->left);
    free(curr->word);
    drop_set_list(curr->head);
    free(curr);
}

//drop the linked list of urls	
static void drop_set_list(set_node curr) {
    set_node next;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

