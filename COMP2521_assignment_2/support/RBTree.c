#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "RBTree.h"
#include "strdup.h"

// Generic functions that can be applied to both variations of RBTrees
URL new_url_node(char *url);
RBTree RBTree_new_node(double tfidf, char *url);
RBTree red_children(RBTree parent);
RBTree RBTree_left_rotation(RBTree tree);
RBTree RBTree_right_rotation(RBTree tree);
RBTree RBTree_LR_rotation(RBTree tree);
RBTree RBTree_RL_rotation(RBTree tree);
void free_RBTree_nodes(RBTree tree);
void free_urls(URL list);

// Functions specific to a single variation of the RBTree
RBTree insert_node_tfidf(RBTree tree, double tfidf, char *url);
char *retrieve_url(RBTree tree);
RBTree insert_node_url(RBTree tree, double tfidf, char *url);

// Allocate memory for a RBTree_rep structure
Tree_Rep new_RBTree(void)
{
    Tree_Rep new = malloc(sizeof (struct RBTree_rep));
    assert (new != NULL);
    new->root = NULL;
    new->size = 0;
    return new;
}

// New URL
URL new_url_node(char *url)
{
    URL new = malloc(sizeof(struct url_list));
    assert(new != NULL);
    new->url = custom_strdup(url);
    new->next = NULL;
    return new;
}

// Create a new node that will be stored in the RBTree
// There will be two variations, one that uses the tfidf as a key, and the other
// the URL as a key
RBTree RBTree_new_node(double tfidf, char *url)
{
    RBTree new = malloc(sizeof(struct node));
    assert(new != NULL);
    new->tfidf = tfidf;
    new->url = new_url_node(url);
    // All new nodes will be red upon creation
    new->colour = RED;
    new->left = NULL;
    new->right = NULL;
    return new;
}

// After inserting a node as a grandchild, if the two children nodes for a given black node are red,
// convert them to black nodes and change the parent node from a black node to a red node. This will ensure
// that there are no two consecutive reds.
RBTree red_children(RBTree parent)
{
    parent->left->colour = BLACK;
    parent->right->colour = BLACK;
    parent->colour = RED;
    return parent;
}

// Perform a left rotation to reduce the height of the right subtree
RBTree RBTree_left_rotation(RBTree tree)
{
    RBTree new_root = tree->right;
    tree->right = new_root->left;
    new_root->left = tree;
    // The previous parent will be red and its child will be black
    new_root->colour = BLACK;
    tree->colour = RED;
    return new_root;
}

// Perform a right rotation to reduce the height of the left subree
RBTree RBTree_right_rotation(RBTree tree)
{
    RBTree new_root = tree->left;
    tree->left = new_root->right;
    new_root->right = tree;
    // The previous parent will be red and its child will be black
    new_root->colour = BLACK;
    tree->colour = RED;
    return new_root;
}

// Perform two consecutive rotations, a left then a right rotation to get the grandchild node to the parent position
RBTree RBTree_LR_rotation(RBTree tree)
{
    // Perform a left rotation
    RBTree new_root = tree->left->right;
    tree->left->right = new_root->left;
    new_root->left = tree->left;
    // Perform a right rotation
    tree->left = new_root->right; 
    new_root->right = tree;
    // Ensure that there are no two consecutive reds 
    new_root->colour = BLACK;
    tree->colour = RED;
    return new_root;
}

// Perform two consecutive rotations, a right and then a left rotation to get the grandchild node to the parent position
RBTree RBTree_RL_rotation(RBTree tree)
{
    // Perform a right rotation
    RBTree new_root = tree->right->left;
    tree->right->left = new_root->right;
    new_root->right = tree->right;
    // Perform a left rotation
    tree->right = new_root->left;
    new_root->left = tree;
    // Ensure that there are no two consecutive reds 
    new_root->colour = BLACK;
    tree->colour = RED;
    return new_root;
}

// Used for a tree that uses URLs as a key value
// URLs are stored in a linked list and there will only be a single node in this variant of a linked list
// Thus this function is simply for the purpose of making the code more readable
char *retrieve_url(RBTree tree)
{
    assert(tree != NULL);
    assert(tree->url != NULL);
    assert(tree->url->url != NULL);
    return tree->url->url;
}

// Insert a node into the RBTree using tfidf values as a key
RBTree insert_node_tfidf(RBTree tree, double tfidf, char *url)
{
    if (tree == NULL)
    {
        return RBTree_new_node(tfidf, url);
    }
    // Once an insertion has occurred, continuing moving up the tree until a black node is identified
    // Insertion has occurred to the left of this tree
    else if (tfidf < tree->tfidf)
    {
        tree->left = insert_node_tfidf(tree->left, tfidf, url);
        if (tree->colour == RED)
        {
            return tree;
        }

        // Check for imbalances 
        // If there are two red nodes in a row
        if ((tree->left != NULL) && (tree->left->colour == RED))
        {
            if ((tree->left->left != NULL) && (tree->left->left->colour == RED))
            {
                // If the right branch is also red, convert the two child nodes to black and make the parent node a red node
                if ((tree->right != NULL) && (tree->right->colour == RED))
                {
                    return red_children(tree);
                }
                // The right child is not a red node
                // There are two cases to consider, if the value was inserted to either the left of the left child, or to its right
                // First case, tfidf < tree->left->tfidf
                // Perform a right rotation such that the child node is now the parent of the original parent and the grandchild node
                return RBTree_right_rotation(tree);
            }
            else if ((tree->left->right != NULL) && (tree->left->right->colour == RED))
            {
                // If the right branch is also red, convert the two child nodes to black and make the parent node a red node
                if ((tree->right != NULL) && (tree->right->colour == RED))
                {
                    return red_children(tree);
                }
 
                // Next case, tfidf > tree->left->tfidf
                // Perform two rotations, a left rotation and then a right rotation to get the grandchild node to the parent node
                return RBTree_LR_rotation(tree);
            }
            // If there is only a single red node, do nothing
        }
    }
    else if (tfidf > tree->tfidf) // Insertion has occured to the right of the tree
    {
        tree->right = insert_node_tfidf(tree->right, tfidf, url);
        if (tree->colour == RED)
        {
            return tree;
        }

        // Check for imbalances
        // If there are two red nodes in a row
        if ((tree->right != NULL) && (tree->right->colour == RED))
        {
            if ((tree->right->right != NULL) && (tree->right->right->colour == RED))
            {
                // If the left branch is also red, convert the two children nodes to black and the parent node to red
                if ((tree->left != NULL) && (tree->left->colour == RED))
                {
                    return red_children(tree);
                }
                // The left child is not a red node
                // There are two cases to consider, if the value was inserted to the right of the right child or to its left
                // First case, tfidf > tree->right->tfidf
                // Perform a left rotation such that the child node is now the parent of the original parent and the grandchild node
                return RBTree_left_rotation(tree);
            }
            else if ((tree->right->left != NULL) && (tree->right->left->colour == RED))
            {
                // If the left branch is also red, convert the two children nodes to black and the parent node to red
                if ((tree->left != NULL) && (tree->left->colour == RED))
                {
                    return red_children(tree);
                }
 
                // Next case, tfidf < tree->right->tfidf
                // Perform two rotations, a right rotation followed by a left rotation to get the grandchild node to the parent position
                return RBTree_RL_rotation(tree);
            }
        }
    }
    else // Node with a tfidf value has been found that is identical to the supplied argument
    {
        URL new_url = new_url_node(url);
        new_url->next = tree->url;
        tree->url = new_url;
    }
    return tree;
}

// Insert a node into the RBTree using URLs as keys
RBTree insert_node_url(RBTree tree, double tfidf, char *url)
{
    if (tree == NULL)
    {
        return RBTree_new_node(tfidf, url);
    }
    // Once an insertion has occurred, continuing moving up the tree until a black node is identified
    // Insertion has occurred to the left of this tree
    else if (strcmp(url, retrieve_url(tree)) < 0)
    {
        tree->left = insert_node_url(tree->left, tfidf, url);
        if (tree->colour == RED)
        {
            return tree;
        }

        // Check for imbalances 
        // If there are two red nodes in a row
        if ((tree->left != NULL) && (tree->left->colour == RED))
        {
            if ((tree->left->left != NULL) && (tree->left->left->colour == RED))
            {
                // If the right branch is also red, convert the two child nodes to black and make the parent node a red node
                if ((tree->right != NULL) && (tree->right->colour == RED))
                {
                    return red_children(tree);
                }
                else
                {
                    // There are two cases to consider, if the value was inserted to either the left of the left child, or to its right
                    // First case, strcmp(url, retrieve_url(tree->left)) < 0
                    // Perform a right rotation such that the child node is now the parent of the original parent and the grandchild node
                    return RBTree_right_rotation(tree);
                }
            }
            else if ((tree->left->right != NULL) && (tree->left->right->colour == RED))
            {
                // If the right branch is also red, convert the two child nodes to black and make the parent node a red node
                if ((tree->right != NULL) && (tree->right->colour == RED))
                {
                    return red_children(tree);
                }
                // Next case, strcmp(url, retrieve_url(tree->left)) > 0
                // Perform two rotations, a left rotation and then a right rotation to get the grandchild node to the parent node
                return RBTree_LR_rotation(tree);
            }
            // If there is only a single red node, do nothing
        }
    }
    else if (strcmp(url, retrieve_url(tree)) > 0) // Insertion has occured to the right of the tree
    {
        tree->right = insert_node_url(tree->right, tfidf, url);
        if (tree->colour == RED)
        {
            return tree;
        }

        // Check for imbalances
        // If there are two red nodes in a row
        if ((tree->right != NULL) && (tree->right->colour == RED))
        {
            if ((tree->right->right != NULL) && (tree->right->right->colour == RED))
            {
                // If the left branch is also red, convert the two children nodes to black and the parent node to red
                if ((tree->left != NULL) && (tree->left->colour == RED))
                {
                    return red_children(tree);
                }
                // The left child is not a red node
                // There are two cases to consider, if the value was inserted to the right of the right child or to its left
                // First case, strcmp(url, retrieve_url(tree->right)) > 0 
                // Perform a left rotation such that the child node is now the parent of the original parent and the grandchild node
                return RBTree_left_rotation(tree);
            }
            else if ((tree->right->left != NULL) && (tree->right->left->colour == RED))
            {
                // If the left branch is also red, convert the two children nodes to black and the parent node to red
                if ((tree->left != NULL) && (tree->left->colour == RED))
                {
                    return red_children(tree);
                }
                // Next case, strcmp(url, retrieve_url(tree->right)) < 0
                // Perform two rotations, a right rotation followed by a left rotation to get the grandchild node to the parent position
                return RBTree_RL_rotation(tree);
            }
        }
    }
    return tree;
}

// Initiate the insertion process for a tree that uses tfidf as a key. Need to ensure that the head of the tree remains black
void RBTree_insert_tfidf(Tree_Rep tree_rep, double tfidf, char *url)
{
    assert(tree_rep != NULL);
    // Unlike the insert with URLs, it is unnecessary to run a check to see if a node already exists in the tree
    // There is a slim possibility that there will be two URLs with the same tfidf and they will each be considered
    // as seperate terms when printed.
    tree_rep->root = insert_node_tfidf(tree_rep->root, tfidf, url);
    ++tree_rep->size;
    // Change the root to black
    tree_rep->root->colour = BLACK;
}

// Initiate the insertion process for a tree that uses URLs as a key. Need to ensure that the root remains black
void RBTree_insert_url(Tree_Rep tree_rep, double tfidf, char *url)
{
    assert(tree_rep != NULL);
    RBTree search = RBTree_search_url(tree_rep->root, url);
    if (search != NULL) // Found a node with the URL to be inserted
    {
        search->tfidf += tfidf;
        return;
    }
    // Have to insert the node into the tree
    tree_rep->root = insert_node_url(tree_rep->root, tfidf, url);
    ++tree_rep->size;
    // Change the root to black
    tree_rep->root->colour = BLACK;
}

// Search to see if a particular tfidf value exists within the tree
int RBTree_search_tfidf(RBTree tree, double tfidf)
{
    if (tree == NULL)
    {
        return 0;
    }
    else if (tree->tfidf == tfidf)
    {
        return 1;
    }
    else if (tfidf < tree->tfidf)
    {
        return RBTree_search_tfidf(tree->left, tfidf);
    }
    else
    {
        return RBTree_search_tfidf(tree->right, tfidf);
    }
}

// Search to see if a particular url exists within the tree
RBTree RBTree_search_url(RBTree tree, char *url)
{
    if (tree == NULL)
    {
        return NULL;
    }
    else if (strcmp(url, retrieve_url(tree)) == 0)
    {
        return tree;
    }
    else if (strcmp(url, retrieve_url(tree)) < 0)
    {
        return RBTree_search_url(tree->left, url);
    }
    else
    {
        return RBTree_search_url(tree->right, url);
    }
}


// Free memory associated with the linked list of URLs
void free_urls(URL list)
{
    URL curr = list;
    while (curr != NULL)
    {
        URL to_delete = curr;
        curr = curr->next;
        free(to_delete->url);
        free(to_delete);
    }
}

// Free memory associated with the nodes in the RBTree
void free_RBTree_nodes(RBTree tree)
{
    if (tree == NULL)
    {
        return;
    }
    free_RBTree_nodes(tree->left);
    free_RBTree_nodes(tree->right);
    free_urls(tree->url);
    free(tree);
}

// Free all memory associated with a RBTree
void free_RBTree(Tree_Rep tree_rep)
{
    if (tree_rep->size > 0)
    {
        free_RBTree_nodes(tree_rep->root);
    }
    free(tree_rep);
}

// Print the nodes in an infix order using tfidf as a key
void infix_tfidf_print(RBTree tree)
{
    if (tree == NULL)
    {
        return;
    }
    infix_tfidf_print(tree->left);
    printf("%lf %d\n", tree->tfidf, tree->colour);
    infix_tfidf_print(tree->right);
}

void infix_url_print(RBTree tree)
{
    if (tree == NULL)
    {
        return;
    }
    infix_url_print(tree->left);
    printf("%s %d\n", retrieve_url(tree), tree->colour);
    infix_url_print(tree->right);
}

// Print the nodes in a descending order based on the tfidf value
int descending_print_tfidf(RBTree tree, int limit)
{
    assert(limit >= 0);
    if (tree == NULL)
    {
        return 0;
    }

    // Count will maintain the number of nodes that has been seen in the recursion
    int count = 0;
    count += descending_print_tfidf(tree->right, limit);

    // Exceeded the number of nodes that can be printed
    if (count > limit)
    {
        return count;
    }
    else
    {
        // There is the possibility that two nodes could potentially have the same tfidf value
        URL curr = tree->url;
        while (curr != NULL)
        {
            printf("%s %.6f\n", curr->url, tree->tfidf);
            ++count;
            curr = curr->next;
        }
    }
    count += descending_print_tfidf(tree->left, limit);
    return count;
}

// Print the nodes in descending order based on URL as a key
void descending_print_url(RBTree tree)
{
    if (tree == NULL)
    {
        return;
    }

    descending_print_url(tree->right);
    printf("%s\n", retrieve_url(tree));
    descending_print_url(tree->left);
}

// Create a RBTree that uses tfidf as keys. The nodes will be derived form the nodes
// in the existing url_tree
void transfer_nodes(RBTree url_tree, Tree_Rep tfidf_tree)
{
    if (url_tree == NULL)
    {
        return;
    }

    transfer_nodes(url_tree->left, tfidf_tree);
    transfer_nodes(url_tree->right, tfidf_tree);
    RBTree_insert_tfidf(tfidf_tree, url_tree->tfidf, retrieve_url(url_tree));
}

/*
int main(void)
{
    Tree_Rep new = new_RBTree();
    new = RBTree_insert_url(new, 11, "url11");
    infix_url_print(new->root); printf("\n\n\n");

    new = RBTree_insert_url(new, 12, "url12");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 13, "url13");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 14, "url14");
    infix_url_print(new->root); printf("\n\n\n");

    new = RBTree_insert_url(new, 15, "url15");
    infix_url_print(new->root); printf("\n\n\n");

    new = RBTree_insert_url(new, 1, "url01");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 10, "url10");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 5, "url05");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 110, "url999");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 6, "url06");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 20, "url20");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 30, "url30");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 16, "url16");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 19, "url19");
    infix_url_print(new->root); printf("\n\n\n");
    new = RBTree_insert_url(new, 50, "url50");
    infix_url_print(new->root); printf("\n\n\n");

    new = RBTree_insert_url(new, 50, "url50");

    infix_url_print(new->root); printf("\n\n\n");
    printf("%d\n\n\n\n\n\n\n\n\n", new->size);


    descending_print_url(new->root);
    printf("\n\n\n");
    free_RBTree(new);

    return 0;
}
*/
/*
int main(void)
{
    Tree_Rep new = new_RBTree();
    RBTree_insert_tfidf(new, 11, "hi");
    infix_tfidf_print(new->root); printf("\n\n\n");

    RBTree_insert_tfidf(new, 12, "there");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 13, "my");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 14, "name");
    infix_tfidf_print(new->root); printf("\n\n\n");

    RBTree_insert_tfidf(new, 15, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");

    RBTree_insert_tfidf(new, 1, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 10, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 5, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 110, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 6, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 20, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 30, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 16, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 19, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");
    RBTree_insert_tfidf(new, 50, "is");
    infix_tfidf_print(new->root); printf("\n\n\n");

    RBTree_insert_tfidf(new, 50, "replica");

    infix_tfidf_print(new->root); printf("\n\n\n");
    printf("%d\n\n\n\n\n\n\n\n\n", new->size);


    descending_print_tfidf(new->root, 30);
    printf("\n\n\n");
    free_RBTree(new);

    return 0;
}
*/
