#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "read_data.h"

Data new_data(char *str);
Data last_node(Data list);
char *normalise(char *str);
Rep new_rep(void);
Content new_content(char *str);

// Create a new node of type struct data. The argument has been dynamically
// allocated and will need to be freed manually by the user after reading from a txt file.
Data new_data(char *str)
{
    assert(str != NULL);
    Data new = malloc(sizeof(struct data));
    assert(new != NULL);
    new->info = str;
    new->next = NULL;
    return new;
}

// Return the last node of a linked list of type struct data.
Data last_node(Data list)
{
    assert(list != NULL);
    Data last;
    for (last = list; last->next != NULL; last = last->next)
    {
    }
    return last;
}

// Normalise a given string. This will require making all letters lower case and removing
// the last character if it is a (.), (,), (;) or (?)
char *normalise(char *str)
{
    assert(str != NULL);
    int i = 0;
    // Check the last character to see if it is an invalid punctuation mark
    int end = strlen(str) - 1;
    if ((str[end] == '.') || (str[end] == ',') || (str[end] == ';') || (str[end] == '?'))
    {
        str[end] = '\0';
    }

    // Iterate through the string and change any upper case characters to a lower case variant. 
    for (; i < strlen(str); ++i)
    {
        if (isupper(str[i]))
        {
            str[i] = tolower(str[i]);
        }
    }
    return str;
}

// Create a data structure that will hold a linked list and also the number
// of nodes in the linked list.
Rep new_rep(void)
{
    Rep new = malloc(sizeof(struct data_rep));
    assert(new != NULL);
    new->data_list = NULL;
    new->size = 0;
    return new;
}


// Read URLs from collection.txt
Rep read_collection(void)
{
    char *url; // Store the URLs that are read from collection.txt
    Rep collection = new_rep(); // Will store a linked list of URLs that will form the vertices of the graph
    FILE *fp = fopen("collection.txt\0", "r");
    assert(fp != NULL);

    while (fscanf(fp, "%ms", &url) == 1) // Continue to iterate through the file while URLs can be read
    {
        if (collection->data_list == NULL) // No linked list has been formed yet
        {
            collection->data_list = new_data(url);
        }
        else // Append to the end of the existing linked list
        {
            last_node(collection->data_list)->next = new_data(url);
        }
        ++collection->size;
    }

    fclose(fp);
    return collection;
}

// Read outlinks from a url txt file.
Rep read_links(char *source)
{
    assert(source != NULL);
    // Need enough memory for the size of the url + .txt\0
    char *file_name = malloc(sizeof(char) * (strlen(source) + 5)); 
    assert(file_name != NULL);
    *file_name = '\0';
    strcat(file_name, source);
    strcat(file_name, ".txt\0");

    char *url = NULL;
    // Store linked list of outlinks from a particular url
    Rep url_outlinks = new_rep();
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);
    char buffer[17];
    
    // Continue searching through the file until finding start of outlinks
    while (fgets(buffer, 17, fp))
    {
        if (strcmp(buffer, "#start Section-1") == 0)
        {
            break;
        }
    }

    while (fscanf(fp, "%ms", &url) == 1)
    {
        // Potential end of outlinks
        if (strcmp(url, "#end") == 0) 
        {
            // Store the current position in the file
            long pos = ftell(fp);
            char *next = NULL;
            // Check if the next term is Section-1
            if (fscanf(fp, "%ms", &next) == 1)
            {
                // If so, the end of the url outlinks has been reached
                if (strcmp(next, "Section-1") == 0)
                {
                    free(next);
                    free(url);
                    break;
                }
                // Otherwise, there are more URLs to be read
                // No linked list has been formed yet
                if (url_outlinks->data_list == NULL) 
                {
                    url_outlinks->data_list = new_data(url);
                }
                else // Append to the end of the existing linked list
                {
                    last_node(url_outlinks->data_list)->next = new_data(url);
                }
                ++url_outlinks->size;
                // Reset the file position to immediately after finding the word
                free(next);
                fseek(fp, pos, SEEK_SET);
                continue;
            }
            // fscanf has failed to read in another string
            else
            {
                free(url);
                break;
            }

        }

        // No linked list has been formed yet
        if (url_outlinks->data_list == NULL)
        {
            url_outlinks->data_list = new_data(url);
        }
        // Append to the end of the existing linked list
        else        
        {
            last_node(url_outlinks->data_list)->next = new_data(url);
        }
        ++url_outlinks->size;
    }
    fclose(fp);
    free(file_name);
    return url_outlinks;
}

// Read data from a URL text file
Rep read_data(char *source)
{
    assert(source != NULL);
    // Need enough memory for the size of the url + .txt\0
    char *file_name = malloc(sizeof(char) * (strlen(source) + 5)); 
    assert(file_name != NULL);
    *file_name = '\0';
    strcat(file_name, source);
    strcat(file_name, ".txt\0");

    char *word = NULL;
    Rep words_rep = new_rep();
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);
    char buffer[17]; 
 
    // Continue searching through the file until finding start of section 2 
    while (fgets(buffer, 17, fp))
    {
        if (strcmp(buffer, "#start Section-2") == 0)
        {
            break;
        }
    }

    while (fscanf(fp, "%ms", &word) == 1)
    {
        // Potential end of section 2
        if (strcmp(word, "#end") == 0) 
        {
            // Store the position in the file
            long pos = ftell(fp);
            char *next = NULL;
            // Check if the next term is Section-2
            if (fscanf(fp, "%ms", &next) == 1)
            {
                // If so, the end of the relevant data section has been reached
                if (strcmp(next, "Section-2") == 0)
                {
                    free(next);
                    free(word);
                    break;
                }
                // Else, append word onto the linked list of found terms
                // It is not necessary to check if the length is 0 as normalising will not affect the length
                word = normalise(word);
                if (words_rep->data_list == NULL) // No linked list has been formed yet
                {
                    words_rep->data_list = new_data(word);
                }
                else // Append to the end of the existing linked list
                {
                    last_node(words_rep->data_list)->next = new_data(word);
                }   
                ++words_rep->size;
                // Reset the file position to immediately after finding word
                free(next);
                fseek(fp, pos, SEEK_SET);
                continue;
            }
            else
            {
                free(word);
                break;
            }

        }
        word = normalise(word);
        if (strlen(word) == 0)
        {
            free(word);
            continue;
        }
        if (words_rep->data_list == NULL) // No linked list has been formed yet
        {
            words_rep->data_list = new_data(word);
        }
        else // Append to the end of the existing linked list
        {
            last_node(words_rep->data_list)->next = new_data(word);
        }
        ++words_rep->size;
    }
    free(file_name);
    fclose(fp);
    return words_rep;
}

// Return a count for the instances of a word in a url.txt file
int count_instances(Data words, char *str)
{
    Data curr = words;
    int count = 0;
    while (curr != NULL)
    {
        if (strcmp(curr->info, str) == 0)
        {
            ++count;
        }
        curr = curr->next;
    }
    return count;
}

// Free the linked list of Data nodes
void free_data(Data head)
{
    Data curr = head;
    while (curr != NULL)
    {
        Data delete = curr;
        curr = curr->next;
        free(delete->info);
        free(delete);
    }
}    

// Free the linked list of Dat + structure
void free_rep(Rep rep)
{
    assert(rep != NULL);
    if (rep->size > 0)
    {
        free_data(rep->data_list);
    }
    free(rep);
}

// Create a new Content node initialised to the str value
Content new_content(char *str)
{
    Content new = malloc(sizeof(struct content_search));
    assert(new != NULL);
    
    new->str = str;
    new->total = 0;
    new->url_list = NULL;
    new->next = NULL;
    return new;
}

// Read invertedIndex.txt and form a linked list of found search terms and the 
// URLs in which they were found. 
Content search_index(int count, char **search_terms)
{
    // Open invertedIndex.txt for reading
    FILE *fp = fopen("invertedIndex.txt", "r");
    assert(fp != NULL);
    
    char *str = NULL;
    int i = 1;
    Content results = NULL;

    // Search for each of the search terms in invertexIndex.txt from search_terms
    while (i < count) 
    {
        // Could not find the search term in the invertedIndex.txt file
        if (fscanf(fp, "%ms", &str) != 1)
        {
            Content new = new_content(search_terms[i]);
            new->next = results;
            results = new;
            ++i;
            rewind(fp);
            continue;
        }
        // Have found a search term
        // Need to create a node in the linked list of Content nodes and prepend the result
        if (strcmp(str, search_terms[i]) == 0)
        {
            Content new = new_content(str);
            new->next = results;
            results = new;
            // It will be necessary now to continue searching through the file invertexIndex.txt
            char *url = NULL;
            while (fscanf(fp, "%ms", &url) == 1) // Scan through and add URLs to the linked list 
            {

                if (results->url_list == NULL)
                {
                    results->url_list = new_data(url);
                }
                else
                {
                    last_node(results->url_list)->next = new_data(url);
                }
                ++results->total;
                //see if we can find a newline character without running into another word
				//if so, break
				int c = fgetc(fp);
				while (c == ' ') c = fgetc(fp);
				if (c == '\n' || c == -1) break;
				else ungetc(c, fp);
            }
            rewind(fp);
            ++i;
        }
        // If the search term cannot be found, free the memory allocated to the string
        else
        {
            free(str);
        }
        // Reset the file pointer to the start of the file if a search term is not found
    }
    fclose(fp);
    return results;
}

// Free the linked list returned from search_index(int count, char **search_terms)
void free_search(Content head)
{
    Content curr = head;
    while (curr != NULL)
    {
        Content to_delete = curr;
        curr = curr->next;
        // str has been dynamically allocated to the node in the linked list
        if (to_delete->url_list != NULL)
        {
            free(to_delete->str);
            free_data(to_delete->url_list);
        }
        free(to_delete);
    }
}
