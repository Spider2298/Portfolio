#ifndef BST_H
#define BST_H

typedef struct _Tree {
    struct _node* head;
} *Tree;

typedef struct _node {
    char* word;
    struct _url_node* head;
    int height;
    struct _node* left;
    struct _node* right;
} *node;


//links for a linked list of urls that contain certain words
typedef struct _url_node {
    char* url;
    struct _url_node* next;
} *url_node;

//allocate and return a tree
Tree create_tree(void);

//perform an insertion maintaining its balance
void insertAVL(Tree, char* word, char* url);

//print out the inverted index
void display_in_order(Tree t);

//free all memory associated with the tree
void drop_tree(Tree t);

//return a linked list of urls that contain 'word'
url_node return_list(Tree, char* word);

#endif
