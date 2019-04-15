#ifndef READ_H
#define READ_H

struct data
{
    char *info;
    struct data* next;
};

typedef struct data* Data;

struct data_rep
{
    struct data* data_list;
    int size;
};

typedef struct data_rep* Rep;

struct content_search
{
    char *str;
    int total;
    struct data* url_list;
    struct content_search* next;
};

typedef struct content_search* Content;

// Read URLs from collection.txt to form the graph vertices
Rep read_collection(void);

// Read outlinks from a given url.txt file
Rep read_links(char *source);

// Read data from a url.txt file
Rep read_data(char *source);

// Return a count for the instances of a word in a url.txt file
int count_instances(Data words, char *str);

// Read URLs from the invertedIndex.txt file that match the search terms supplied as 
// arguments
Content search_index(int count, char **search_terms);

// Free the linked list of Data nodes
void free_data(Data head);

// Free the linked list of Data + structure
void free_rep(Rep rep);

// Free the linked list of struct content_search nodes
void free_search(Content head);

#endif
