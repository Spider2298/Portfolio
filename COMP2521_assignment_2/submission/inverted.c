#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BST.h"
#include "read_data.h"


//using the lists returned from read_data.h construct the BST
Tree read_input(void);


int main(int argc, char** argv) {
	//make sure there is a collection.txt file in the directory
	FILE* test = fopen("collection.txt","r");
	if(test == NULL) {
		fprintf(stderr,"%s: Please include a collection.txt file in the executable's directory.\n",argv[0]);
		abort();
	} else fclose(test);
	
    Tree t = read_input();
    display_in_order(t);
    drop_tree(t);
    return 0;
}

//helper function for the read_input function
void read_words(Tree t, char* url) {

    //read the content of the url file
    Rep list = read_data(url);				
    
    //insert each word (curr->info), indicating it is contained by url
    for (Data curr = list->data_list; curr != NULL; curr = curr->next) {
        insertAVL(t,curr->info,url);		
    }
    // Free allocated memory
    free_rep(list);
}

//using the lists returned from read_data.h construct the BST
Tree read_input(void) {
    
    // Read data from colleciton.txt
    Rep list = read_collection();
    
    // Create an AVL Tree
    Tree t = create_tree();

    // Read data from the URL file and insert it into the tree
    for (Data curr = list->data_list; curr != NULL; curr = curr->next) {
        read_words(t,curr->info);
    }
    
    // Free allocated memory
    free_rep(list);
    
    return t;
}
