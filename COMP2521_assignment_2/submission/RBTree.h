#ifndef RBTREE_H
#define RBTREE_H

typedef enum {RED, BLACK} Colour;

struct url_list
{
    char *url;
    int terms;
    struct url_list* next;
};

typedef struct url_list* URL;

struct node
{
    double tfidf;
    struct url_list* url;
    Colour colour;
    struct node* internal_tree;
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
void RBTree_insert_tfidf(Tree_Rep tree_rep, double tfidf, URL url);

// Insert a node into the RBTree using the url as a key
void RBTree_insert_url(Tree_Rep tree_rep, double tfidf, char* url);

// Separate the nodes based on the number of values that are in common with the
// command line argument. This function will reuse the existing data structures
// and thus will be storing the number of common terms in the type double tfidf variable.
void RBTree_insert_group(Tree_Rep tree_rep, double tfidf, URL url);

// Search to see if a particular tfidf value exists within the tree
RBTree RBTree_search_tfidf(RBTree tree, double tfidf);

// Search to see if a particular url exists within the tree
RBTree RBTree_search_url(RBTree tree, char *url);

// Free all memory associated with the RBTree
void free_RBTree(Tree_Rep tree_rep);

// Free memory associated with a RBTree that is used to separate nodes into groups based on the
// number of terms in common with the command line argument
void free_RBTree_Group(Tree_Rep tree_rep);

// Print the nodes in an infix order using tfidf as a key
void infix_tfidf_print(RBTree tree);

// Print the nodes in an infix order using the URL as a key
void infix_url_print(RBTree tree);

// Print the nodes in descending order based on tfidf as a key
void descending_print_tfidf(RBTree tree, int limit, int *count);

// Print the nodes in descending order based on URL as a key
void descending_print_url(RBTree tree);

// Initiate the process to print the values in descending order based on the group number and within the
// group number, the tfidf value.
void descending_print_group(RBTree tree, int limit, int *count);

// Create a RBTree that separates nodes based on the number of elements in common with the
// command line arguments. Within each of these groups, sort by tfidf values in descending order.
void transfer_nodes(RBTree url_tree, Tree_Rep tfidf_tree);

#endif
