#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "RBTree.h"
#include "strdup.h"

// Generic functions that can be applied to both variations of RBTrees
URL new_url_node(char *url);
RBTree RBTree_new_node(double tfidf, URL url);
RBTree red_children(RBTree parent);
RBTree RBTree_left_rotation(RBTree tree);
RBTree RBTree_right_rotation(RBTree tree);
RBTree RBTree_LR_rotation(RBTree tree);
RBTree RBTree_RL_rotation(RBTree tree);
char *retrieve_url(RBTree tree);
void free_RBTree_nodes(RBTree tree);
void free_urls(URL list);

// Functions specific to a single variation of the RBTree
RBTree insert_node_tfidf(RBTree tree, double tfidf, URL url);
RBTree insert_node_url(RBTree tree, double tfidf, URL url);
RBTree insert_node_group(RBTree tree, double tfidf, URL url);
URL insert_url_list(URL head, URL new);
void free_group_nodes(RBTree tree);

// Allocate memory for a RBTree_rep structure
Tree_Rep new_RBTree(void)
{
    Tree_Rep new = malloc(sizeof (struct RBTree_rep));
    assert (new != NULL);
    new->root = NULL;
    new->size = 0;
    return new;
}

// New URL node that will be stored as a linked list within a RBTree node
URL new_url_node(char *url)
{
    URL new = malloc(sizeof(struct url_list));
    assert(new != NULL);
    new->terms = 1;
    new->url = custom_strdup(url);
    new->next = NULL;
    return new;
}

// Create a new node that will be stored in the RBTree
// There will be two variations, one that uses the tfidf as a key, and the other
// the URL as a key. The version that uses tfidf as a key will also have a separate variation,
// that being one that uses the tfidf value to store a group number.
RBTree RBTree_new_node(double tfidf, URL url)
{
    RBTree new = malloc(sizeof(struct node));
    assert(new != NULL);
    new->tfidf = tfidf;
    if (url != NULL)
    {
        new->url = new_url_node(url->url);
        new->url->terms = url->terms;
    }
    else
    {
        new->url = new_url_node("");
    }
    // All new nodes will be red upon creation
    new->colour = RED;
    new->internal_tree = NULL;
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

// Used for a tree that uses URLs as a key value.
// URLs are stored in a linked list in this particular implementation of a tree. In the scenario
// where the tree uses URLs as a key, there will only be a single node in the linked list as the
// URLs will be unique. Thus this function is simply for the purpose of making the code more readable.
char *retrieve_url(RBTree tree)
{
    assert(tree != NULL);
    assert(tree->url != NULL);
    assert(tree->url->url != NULL);
    return tree->url->url;
}

// Insert a url in the correct order in the linked list of URLs in a RBTree node.
// This function will be invoked when tfidf values are the same. The correct order
// that URLs should be ranked are to value those which have the most amount of terms 
// in common with the commandline argument, and if two URLs have the same value,
// to rank them by lexical order.
URL insert_url_list(URL head, URL new)
{
    assert(new != NULL);
    
    // If head is currently a pointer to NULL, simply return new
    if (head == NULL)
    {
        return new;
    }

    // Check if prepending to the URL list
    if (strcmp(new->url, head->url) < 0)
    {
        new->next = head;
        return new;
    }

    // Edge case - At this point, if head is a single element linked list, it is
    // necessary to append to the list
    URL curr = head;
    if (curr->next == NULL)
    {
        curr->next = new;
        return head;
    }

    // Iterate through the list and find when the next URL->str is greater than new->str
    // based of lexical order
    while (curr->next != NULL)
    {
        if (strcmp(new->url, curr->next->url) < 0)
        {
            new->next = curr->next;
            curr->next = new;
            break;
        }
        curr = curr->next;
    }

    // Reached the end of the linked list
    if (curr->next == NULL)
    {
        curr->next = new;
    }

    return head;
}

// Insert a node into the RBTree using tfidf values as a key
RBTree insert_node_tfidf(RBTree tree, double tfidf, URL url)
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
    // Insertion has occured to the right of the tree
    else if (tfidf > tree->tfidf)     
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
        URL new_url = new_url_node(url->url);
        new_url->terms = url->terms;
        tree->url = insert_url_list(tree->url, new_url);
    }
    return tree;
}

// Insert a node into the RBTree using URLs as keys
RBTree insert_node_url(RBTree tree, double tfidf, URL url)
{
    if (tree == NULL)
    {
        return RBTree_new_node(tfidf, url);
    }
    // Once an insertion has occurred, continuing moving up the tree until a black node is identified
    // Insertion has occurred to the left of this tree
    else if (strcmp(url->url, retrieve_url(tree)) < 0)
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
                    // First case, strcmp(url->url, retrieve_url(tree->left)) < 0
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
                // Next case, strcmp(url->url, retrieve_url(tree->left)) > 0
                // Perform two rotations, a left rotation and then a right rotation to get the grandchild node to the parent node
                return RBTree_LR_rotation(tree);
            }
            // If there is only a single red node, do nothing
        }
    }
    // Insertion has occured to the right of the tree
    else if (strcmp(url->url, retrieve_url(tree)) > 0)
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
                // First case, strcmp(url->url, retrieve_url(tree->right)) > 0 
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
                // Next case, strcmp(url->url, retrieve_url(tree->right)) < 0
                // Perform two rotations, a right rotation followed by a left rotation to get the grandchild node to the parent position
                return RBTree_RL_rotation(tree);
            }
        }
    }
    else
    {
        // URL exists in the tree. Increase the tfidf value of the node by the supplied argument and 
        // increment the number of times that the URL has appeared
        ++tree->url->terms;
        tree->tfidf += tfidf;
    }
    return tree;
}

// Insert a node into the RBTree using a group number as a key.
// To reuse the RBTree, the group number will be stored within the tfidf value despite being an int. 
RBTree insert_node_group(RBTree tree, double tfidf, URL url)
{
    // This is a slight variation to the previous insertion algorithms.
    // Find the point of insertion at the leaf and create a new node. The node will not be returned immediately
    // and will be used to store an internal tree which will use tfidf values as a key.
    if (tree == NULL)
    {
        RBTree new = RBTree_new_node(url->terms, NULL);
        new->internal_tree = insert_node_tfidf(new->internal_tree, tfidf, url);
        return new;
    }
    // Once an insertion has occurred, continuing moving up the tree until a black node is identified
    // Insertion has occurred to the left of this tree
    else if (url->terms < tree->tfidf)
    {
        tree->left = insert_node_group(tree->left, tfidf, url);
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
                // First case, url->terms < tree->left->tfidf
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
 
                // Next case, url->terms > tree->left->tfidf
                // Perform two rotations, a left rotation and then a right rotation to get the grandchild node to the parent node
                return RBTree_LR_rotation(tree);
            }
            // If there is only a single red node, do nothing
        }
    }
    else if (url->terms > tree->tfidf) // Insertion has occured to the right of the tree
    {
        tree->right = insert_node_group(tree->right, tfidf, url);
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
                // First case, url->terms > tree->right->tfidf
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
 
                // Next case, url->terms < tree->right->tfidf
                // Perform two rotations, a right rotation followed by a left rotation to get the grandchild node to the parent position
                return RBTree_RL_rotation(tree);
            }
        }
    }
    // A node has been found that contains the key value that was to be inserted
    else
    {
        tree->internal_tree = insert_node_tfidf(tree->internal_tree, tfidf, url);
    }
    return tree;
}

// Initiate the insertion process for a tree that uses tfidf as a key. Need to ensure that the head of the tree remains black
void RBTree_insert_tfidf(Tree_Rep tree_rep, double tfidf, URL url)
{
    assert(tree_rep != NULL);
    tree_rep->root = insert_node_tfidf(tree_rep->root, tfidf, url);
    ++tree_rep->size;
    // Change the root to black
    tree_rep->root->colour = BLACK;
}

// Initiate the insertion process for a tree that uses URLs as a key. Need to ensure that the root remains black
void RBTree_insert_url(Tree_Rep tree_rep, double tfidf, char* url)
{
    assert(tree_rep != NULL);
    // If incrementing the value of tfidf, there is no need to increase the size of tree_rep
    RBTree search = RBTree_search_url(tree_rep->root, url);
    if (search != NULL)
    {
        ++search->url->terms;
        search->tfidf += tfidf;
        return;
    }
 
    // Otherwise, have to insert the node into the tree
    URL new_url = new_url_node(url);
    tree_rep->root = insert_node_url(tree_rep->root, tfidf, new_url);
    ++tree_rep->size;
    // Change the root to black
    tree_rep->root->colour = BLACK;
    free_urls(new_url);
}

// Separate the nodes based on the number of values that are in common with the
// command line argument. This function will reuse the existing data structures
// and thus will be storing the number of common terms in the type double tfidf variable.
void RBTree_insert_group(Tree_Rep tree_rep, double tfidf, URL url)
{
    assert(tree_rep != NULL);
    // Insert the node into the tree
    tree_rep->root = insert_node_group(tree_rep->root, tfidf, url);
    ++tree_rep->size;
    // Change the root to black
    tree_rep->root->colour = BLACK;
}

// Search to see if a particular tfidf value exists within the tree
RBTree RBTree_search_tfidf(RBTree tree, double tfidf)
{
    if (tree == NULL)
    {
        return NULL;
    }
    else if (tree->tfidf == tfidf)
    {
        return tree;
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

// Free the nodes within the RBTree that is used to separate notes into groups
void free_group_nodes(RBTree tree)
{
    if (tree == NULL)
    {
        return;
    }
    free_group_nodes(tree->left);
    free_group_nodes(tree->right);
    free_RBTree_nodes(tree->internal_tree);
    free_urls(tree->url);
    free(tree);
}

// Free memory associated with a RBTree that is used to separate nodes into groups based on the
// number of terms in common with the command line argument
void free_RBTree_Group(Tree_Rep tree_rep)
{
    if (tree_rep->size > 0)
    {
        free_group_nodes(tree_rep->root);
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

// Print the nodes in an infix order using the URL as a key
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

// Print the nodes in descending order based on the tfidf value
void descending_print_tfidf(RBTree tree, int limit, int *count)
{
    assert(limit >= 0);
    if (tree == NULL)
    {
        return;
    }

    // Count will maintain the number of nodes that has been seen in the recursion
    descending_print_tfidf(tree->right, limit, count);

    // Exceeded the number of nodes that can be printed
    if (*count >= limit)
    {
        return;
    }
    else
    {
        // There is the possibility that two nodes could potentially have the same tfidf value
        URL curr = tree->url;
        while ((curr != NULL) && (*count < limit))
        {
            printf("%s %.6f\n", curr->url, tree->tfidf);
            ++(*count);
            curr = curr->next;
        }
    }
    descending_print_tfidf(tree->left, limit, count);
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

// Initiate the process to print the values in descending order based on the group number and within the
// group number, the tfidf value.
void descending_print_group(RBTree tree, int limit, int *count)
{
    if (tree == NULL)
    {
        return;
    }

    descending_print_group(tree->right, limit, count);
    if (*count < limit)
    {
        descending_print_tfidf(tree->internal_tree, limit, count);
    }
    else
    {
        return;
    }
    descending_print_group(tree->left, limit, count);
}

// Create a RBTree that separates nodes based on the number of elements in common with the
// command line arguments. Within each of these groups, sort by tfidf values in descending order.
void transfer_nodes(RBTree url_tree, Tree_Rep group_tree)
{
    if (url_tree == NULL)
    {
        return;
    }

    transfer_nodes(url_tree->left, group_tree);
    transfer_nodes(url_tree->right, group_tree);
    RBTree_insert_group(group_tree, url_tree->tfidf, url_tree->url);
}
