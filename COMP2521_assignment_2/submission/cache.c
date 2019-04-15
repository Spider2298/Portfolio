#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cache.h"
#include "strdup.h"
//to fix compatibility issues across devices, custom_strdup from strdup.c is used
//instead of strdup from the string.h library


//tree function similar to others in this assignment
//keys are strings of ascii '0' and '1', indicating which elements have already been included into the order
//each node contains information on the minimum cost way to arrange 'from this point', and the element that should be
//included next to minimize the cost



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
Tree create_cache_tree(void) {
    Tree new_Tree = malloc(sizeof(struct _Tree));
    assert(new_Tree);
    new_Tree->head = NULL;
    return new_Tree;
}


/*

//print functions

void display_in_order(Tree t);										//header function for internal recursive version
static void recursive_display_in_order(node curr, FILE* output);//recursively print out the content of the tree in order in the format specified for invertedIndex


void display_in_order(Tree t) {
    //FILE* output = fopen("invertedIndex.txt","w");
    //assert(output);
    recursive_display_in_order(t->head,stdout);
    //fclose(output);
}

static void recursive_display_in_order(node curr, FILE* output) {
    if (curr != NULL) {
        recursive_display_in_order(curr->left, output);
        fprintf(output,"%s: %lf\n",curr->word,curr->value);
        recursive_display_in_order(curr->right, output);
    }
}
*/



//output functions

//header function for internal recursive version, returns a pointer to the node with the key value 'word'
//returns NULL if there is no such node
node return_node(Tree t, char* word);
//return the pointer to the node with the key value 'word'					
static node recursive_return_node(node curr, char* word);	

//header function for internal recursive version
node return_node(Tree t, char* word) {
    return recursive_return_node(t->head,word);
}
//return the pointer to the node with the key value 'word'
static node recursive_return_node(node curr, char* word) {
    if (curr == NULL) return NULL;
    if (strcmp(word,curr->word) == 0) {
        return curr;
    }
    if (strcmp(word,curr->word) > 0) return recursive_return_node(curr->right, word);
    else return recursive_return_node(curr->left, word);
}




//insertion functions

//header function for the recursive function
void insertAVL_cache(Tree t, char* word, double value, int pos);	
//find the position to insert the new node					
static node recursive_insertAVL(node curr, char* word, double value, int pos);
//create the new node
static node create_node(char* word, double value, int pos);						



//header function for the recursive function
void insertAVL_cache(Tree t, char* word, double value, int pos) {
    t->head = recursive_insertAVL(t->head,word,value, pos);
}

//find the position to insert the new node
static node recursive_insertAVL(node curr, char* word, double value, int pos) {
    // Inserting at leaf
    if (curr == NULL) return create_node(word, value, pos);
    // Update url list
    if (strcmp(word,curr->word) == 0) {
        return curr;
    }
    // Inserting into the right subtree
    if (strcmp(word,curr->word) > 0) curr->right = recursive_insertAVL(curr->right, word, value, pos);
    // Inserting into the left subtree
    else if (strcmp(word,curr->word) < 0) curr->left = recursive_insertAVL(curr->left, word, value, pos);
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
static node create_node(char* word, double value, int pos) {
    node new_node = malloc(sizeof(struct _node));
    assert(new_node);
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->word = custom_strdup(word);
    new_node->value = value;
    new_node->pos = pos;
    new_node->height = 1;
    return new_node;
}




//drop functions

//header function for recursive function
void drop_cache_tree(Tree t);
//calls itself on left, right, and then frees itself					
static void recursive_drop_tree(node curr);	

//header function for recursive function
void drop_cache_tree(Tree t) {
    recursive_drop_tree(t->head);
    free(t);
}

//calls itself on left, right, and then frees itself					
static void recursive_drop_tree(node curr) {
    if (curr == NULL) return;
    recursive_drop_tree(curr->right);
    recursive_drop_tree(curr->left);
    free(curr->word);
    free(curr);
}
