#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "BST.h"
#include "strdup.h"
//to fix compatibility issues across devices, custom_strdup from strdup.c is used
//instead of strdup from the string.h library


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
Tree create_tree(void) {
    Tree new_Tree = malloc(sizeof(struct _Tree));
    assert(new_Tree);
    new_Tree->head = NULL;
    return new_Tree;
}

//print functions

//header function for internal recursive version
void display_in_order(Tree t);									
//recursively print out the content of the tree in order in the format specified for invertedIndex	
static void recursive_display_in_order(node curr, FILE* output);

//header function for internal recursive version
void display_in_order(Tree t) {
    FILE* output = fopen("invertedIndex.txt","w");
    assert(output);
    recursive_display_in_order(t->head,output);
    fclose(output);
}

//recursively print out the content of the tree in order in the format specified for invertedIndex
static void recursive_display_in_order(node curr, FILE* output) {
    if (curr != NULL) {
        recursive_display_in_order(curr->left, output);
        fprintf(output,"%s ",curr->word);
        url_node ptr = curr->head;
        while (ptr != NULL) {
            fprintf(output,"%s ",ptr->url);
            ptr = ptr->next;
        }
        fprintf(output,"\n");
        recursive_display_in_order(curr->right, output);
    }
}

//output functions

//header function for internal recursive version
url_node return_list(Tree t, char* word);						
//return the linked list of urls that contain this word
static url_node recursive_return_list(node curr, char* word);	

//header function for internal recursive version
url_node return_list(Tree t, char* word) {
    return recursive_return_list(t->head,word);
}

//return the linked list of urls that contain this word
static url_node recursive_return_list(node curr, char* word) {
    if (curr == NULL) return NULL;
    if (strcmp(word,curr->word) == 0) {
        return curr->head;
    }
    if (strcmp(word,curr->word) > 0) return recursive_return_list(curr->right, word);
    else return recursive_return_list(curr->left, word);
}




//insertion functions

//header function for the recursive function
void insertAVL(Tree t, char* word, char* url);						
//find the position to insert the new node
static node recursive_insertAVL(node curr, char* word,char* url);	
//create the new node
static node create_node(char* word, char* url);						
//update the list of urls that contain this word
static void insert_url(node parent, char* url);						
//create a url node to contain url string
static url_node create_url_node(char* url);							

//header function for the recursive function
void insertAVL(Tree t, char* word, char* url) {
    t->head = recursive_insertAVL(t->head,word,url);
}

//find the position to insert the new node
static node recursive_insertAVL(node curr, char* word, char* url) {
    // Inserting at leaf
    if (curr == NULL) return create_node(word,url);
    // Update url list
    if (strcmp(word,curr->word) == 0) {
        insert_url(curr,url);
        return curr;
    }
    // Inserting into the right subtree
    if (strcmp(word,curr->word) > 0) curr->right = recursive_insertAVL(curr->right, word, url);
    // Inserting into the left subtree
    else if (strcmp(word,curr->word) < 0) curr->left = recursive_insertAVL(curr->left, word, url);
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
static node create_node(char* word, char* url) {
    node new_node = malloc(sizeof(struct _node));
    assert(new_node);
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->word = custom_strdup(word);
    new_node->head = NULL;
    new_node->height = 1;
    insert_url(new_node,url);
    return new_node;
}

//update the list of urls that contain this word
static void insert_url(node parent, char* url) {
    url_node curr = parent->head;
    if (curr == NULL) {
        parent->head = create_url_node(url);
    } else if(strcmp(curr->url,url) == 0) {
        return;
    } else {
        //move to point of insertion
        while (curr->next != NULL && strcmp(url,curr->next->url) > 0) {		
            curr = curr->next;
        }
        //if the url being inserted is already in the list, do nothing
        if (curr->next && strcmp(url,curr->next->url) == 0) return;			
        url_node new_node = create_url_node(url);
        new_node->next = curr->next;
        curr->next = new_node;
    }
}

//create a url node to contain url string
static url_node create_url_node(char* url) {
    url_node new_node = malloc(sizeof(struct _url_node));
    assert(new_node);
    new_node->next = NULL;
    new_node->url = custom_strdup(url);
    return new_node;
}




//drop functions

//header function for recursive function
void drop_tree(Tree t);					
//calls itself on left, right, and then frees itself
static void recursive_drop_tree(node curr);	
//drop the linked list of urls
static void drop_url_list(url_node curr);		

//header function for recursive function
void drop_tree(Tree t) {
    recursive_drop_tree(t->head);
    free(t);
}

//calls itself on left, right, and then frees itself
static void recursive_drop_tree(node curr) {
    if (curr == NULL) return;
    recursive_drop_tree(curr->right);
    recursive_drop_tree(curr->left);
    free(curr->word);
    drop_url_list(curr->head);
    free(curr);
}

//drop the linked list of urls
static void drop_url_list(url_node curr) {
    url_node next;
    while (curr != NULL) {
        next = curr->next;
        free(curr->url);
        free(curr);
        curr = next;
    }
}

