#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "strdup.h"
#include "BST.h"

#define MAX_WORD_SIZE 50

//This program works by reading the urls from the pagerankList.txt in order into a linked list
//Then for each word that is being searched for, marking every url that contains that word to being 'present'
//This is done by regenerating a tree identical to the one in invertedIndex and using the tree to return
//linked lists of urls that contain certain words

typedef struct _rank_node {
    int present;
    char* url;
    struct _rank_node* next;
} *rank_node;

//helper function to create rank nodes and return a pointer to them
rank_node create_rank_node(char* url);

//read ranks from pagerankList.txt into a linked list in order
rank_node read_ranks(char* file);

//generate an inverted index tree from the inverted index file
Tree generate_tree(char* file);

//mark the specified url in the rank linked list as containing a search term
void enable(rank_node head, char* url);

//drop the linked list of ranked urls
void drop_rank_list(rank_node head);


int main(int argc, char** argv) {
    if (argc == 1) {
    	fprintf(stderr,"Usage: %s [term1] [term2] ... [termN]\n",argv[0]);
    	abort();
    }
    rank_node rank_head = read_ranks("pagerankList.txt");
    Tree t = generate_tree("invertedIndex.txt");
    for (int i = 1; i < argc; i++) {		//loop through search terms
        char* word = argv[i];
        url_node curr = return_list(t,word);
        while (curr != NULL) {
            enable(rank_head,curr->url);
            curr = curr->next;
        }
    }
    int to_print = 30;
   	//initialize num to be the number of search terms
    int num = argc - 1;
    //first we print items in order of number of search terms they contain, and then within those groups, in order of pagerank
    while (num > 0) {
    	//print urls that contain the correct number of search terms
		for (rank_node curr = rank_head; curr != NULL && to_print > 0; curr = curr->next) {	
			if (curr->present == num) {
				printf("%s\n",curr->url);
				to_print--;
			}
		}
		num--;
	}
    drop_rank_list(rank_head);
    drop_tree(t);
}

//helper function to create rank nodes and return a pointer to them
rank_node create_rank_node(char* url) {
    rank_node new_node = malloc(sizeof(struct _rank_node));
    assert(new_node);
    new_node->present = 0;
    new_node->next = NULL;
    new_node->url = custom_strdup(url);
    return new_node;
}

//read ranks from pagerankList.txt into a linked list in order
rank_node read_ranks(char* file) {
    rank_node head = NULL;
    rank_node last = NULL;
    char url[MAX_WORD_SIZE];
    FILE* input = fopen(file,"r");
    assert(input);
    int insert = 0;
    while (fscanf(input, "%s", url) == 1) {
        if (insert == 0) {
         	//get rid of trailing comma
            url[strlen(url)-1] = 0;      
            rank_node new_node = create_rank_node(url);
            if (head) last->next = new_node;
            else head = new_node;
            last = new_node;
            //only insert every 3rd item (starting from the first item) will give us the rank
            insert = 2;					
        } else {
            insert--;
        }
    }
    fclose(input);
    return head;
}

//generate an inverted index tree from the inverted index file
Tree generate_tree(char* file) {
    Tree t = create_tree();
    char item[MAX_WORD_SIZE];
    char word[MAX_WORD_SIZE];
    FILE* input = fopen(file,"r");
    assert(input);
    int new_word = 1;
    while (fscanf(input,"%s", item) == 1) {
    		
    	if (new_word) {
    		strcpy(word,item);
    		new_word = 0;
    	} else {
    		insertAVL(t,word,item);
    	}
    	//see if we can find a newline character without running into another word
    	//if so, indicate the new item to be read is a new word
    	int c = fgetc(input);
    	while (c == ' ') c = fgetc(input);
    	if (c == '\n') new_word = 1;
    	else ungetc(c, input);
    }
    fclose(input);
    return t;
}

//mark the specified url in the rank linked list as containing a search term
void enable(rank_node curr, char* url) {
    while (curr != NULL) {
        if(strcmp(url,curr->url) == 0) {
            curr->present++;
            return;
        }
        curr = curr->next;
    }
}

//drop the linked list of ranked urls
void drop_rank_list(rank_node curr) {
    while (curr != NULL) {
        rank_node next = curr->next;
        free(curr->url);
        free(curr);
        curr = next;
    }
}


