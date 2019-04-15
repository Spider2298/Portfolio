#ifndef CACHE_TREE_H
#define CACHE_TREE_H

typedef struct _Tree {
    struct _node* head;
} *Tree;

typedef struct _node {
    char* word;
    double value;
    int pos;
    int height;
    struct _node* left;
    struct _node* right;
} *node;

//allocate and return a cache
Tree create_cache_tree(void);

//perform an insertion into the cache
void insertAVL_cache(Tree, char* word, double value, int pos);

//void display_in_order(Tree t);

//free all memory associated with the tree
void drop_cache_tree(Tree t);

//return node with the key 'word'
//returns NULL if no such node exists
node return_node(Tree, char* word);

#endif
