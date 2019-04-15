#ifndef RBTREE_H
#define RBTREE_H

typedef enum {RED, BLACK} Colour;

struct url_list
{
    char *url;
    struct url_list* next;
};

typedef struct url_list* URL;

struct node
{
    double tfidf;
    struct url_list* url;
    Colour colour;
    struct node* left;
    struct node* right;
};

typedef struct node* RBTree;    

struct RBTree_rep
{
    RBTree root;
    int size;
};

typedef struct RBTree_rep* Tree_Rep;

// Initialise a new RBTree
Tree_Rep new_RBTree(void);

// Insert a new node into the RBTree using tfidf as a key
void RBTree_insert_tfidf(Tree_Rep tree_rep, double tfidf, char *url);

// Insert a node into the RBTree using the url as a key
void RBTree_insert_url(Tree_Rep tree_rep, double tfidf, char *url);

// Search to see if a particular tfidf value exists within the tree
int RBTree_search_tfidf(RBTree tree, double tfidf);

// Search to see if a particular url exists within the tree
RBTree RBTree_search_url(RBTree tree, char *url);

// Free all memory associated with the RBTree
void free_RBTree(Tree_Rep tree_rep);

// Print the nodes in an infix order using tfidf as a key
void infix_tfidf_print(RBTree tree);

// Print the nodes in an infix order using the URL as a key
void infix_url_print(RBTree tree);

// Print the nodes in descending order based on tfidf as a key
int descending_print_tfidf(RBTree tree, int limit);

// Print the nodes in descending order based on URL as a key
void descending_print_url(RBTree tree);

// Create a RBTree that uses tfidf as keys. The nodes will be derived
// from the nodes in the existing url_tree
void transfer_nodes(RBTree url_tree, Tree_Rep tfidf_tree);

#endif
