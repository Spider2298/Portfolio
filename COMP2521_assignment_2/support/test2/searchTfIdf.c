#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "RBTree.h"
#include "read_data.h"

// Calculate the TfIdf for each URL that contains a query term as a command line argument
void searchTfIdf(int argc, char** argv)
{
    // No search terms have been provided
    if (argc < 2)
    {
        fprintf(stderr, "No search terms have been provided as command line arguments\n");
        return;
    }

    // To calculate the inverse document frequency, it is necessary to determine the total
    // number of URLs available on the web which would be provided in collection.txt
    Rep all_urls = read_collection();
    int total_documents = all_urls->size;
    free_rep(all_urls);

    // Collect all of the search terms into a linked list with each node containing a separate
    // linked list to all the URLs in which the term appears.
    Content search_terms = search_index(argc, argv);

    // Create a RBTree with the key of a node as the URL of a page.
    // As the tree is formed, calculate the TfIdf value for a search term on a given page
    // and sum it to the existing tfidf value if the node exists.
    Tree_Rep url_tree = new_RBTree(); 
    Content curr = search_terms;

    // Iterate through the command line arguments
    for (; curr != NULL; curr = curr->next)
    {
        // Calculate the inverse document frequency(idf)
        double idf = log10(((double) total_documents)/curr->total);
        
        // Separate iteration that searches through URLs that contain a search term
        // Will calculate the term frequency (tf) for each URL 
        Data url = curr->url_list;
        for (; url != NULL; url = url->next)
        {
            Rep words = read_data(url->info);
            int frequency = count_instances(words->data_list, curr->str);
            double tf = ((double) frequency)/words->size;
            double tfidf = tf * idf;
            RBTree_insert_url(url_tree, tfidf, url->info);
            free_rep(words);
        }
    }
    free_search(search_terms); 

    // The TfIdf values have been calculated for all of the URLs that contain a query 
    // term at this point. It is now necessary to output the top 30 URLs in descending
    // order based on TfIdf values. The nodes will be passed into a RBTree where the nodes
    // will be sorted into groups based on the number of terms common with the command line
    // arguments and then sorted by TfIdf values.

    Tree_Rep group_tree = new_RBTree();
    transfer_nodes(url_tree->root, group_tree);
    free_RBTree(url_tree);

    // If there are less than 30 nodes, decrease the upper bound of tfidf values printed
    int limit = 30;
    if (group_tree->size < limit)
    {
        limit = group_tree->size;
    }
    int *count = malloc(sizeof(int));
    assert(count != NULL);
    *count = 0;
    descending_print_group(group_tree->root, limit, count);
    printf("%d\n", *count);
    free(count);
    free_RBTree_Group(group_tree);
}

int main(int argc, char** argv)
{
    searchTfIdf(argc, argv);
    return 0;
}
