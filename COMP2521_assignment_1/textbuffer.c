#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "textbuffer.h"

//each node points to a line of text and points to the next node
struct _line_node {
    char* line;
    struct _line_node* next;
};
typedef struct _line_node* line_node;

//representations of the text buffer also points to stack which contain states to undo and redo with
struct textbuffer {
    struct _line_node* head;
    int length;
    struct stack_buffer* undo_stack;
    struct stack_buffer* redo_stack;
};

//holds the state of the tb that may be desired to be restored
struct stack_buffer {
    struct _line_node* head;
    int length;
    struct stack_buffer* next;
};

//return the length of the undo or redo stack
static int stack_length(struct stack_buffer* curr);

//remove the oldest item off the undo stack
static void trim_stack(struct stack_buffer* curr);

//save the state of the TB and push onto undo stack
static void push_state(TB tb);

//free all memory in the undo stack
static void destroy_stack(struct stack_buffer** stack_head);

//append match nodes to the match node list and return the new head
static matchNode* append_match(matchNode* head, int line, int char_index);

//malloc with error checking
static void* mmalloc(int size);

//strdup with error checking
static char* mstrdup(char* src);

//create a node to contain a line of text
static line_node create_node(char* line);

//allocate memory for a tb and return a pointer to it
static struct textbuffer* create_buffer(line_node head, int length);

//return a copy of a linked list of line nodes
static line_node list_copy(line_node head);

//free all memory in a linked list of line nodes
static void list_delete(line_node head);

//test whether a given special symbol should be modified
static int char_present(char* line, char c);

//calculate how much more memory we will need for the rich text for a given line
static int formRichText_counter(char* line);

/* Allocate a new textbuffer whose contents is initialised with the text given
 * in the array.
 */
TB newTB (char text[]) {
    //check for invalid inputs
    //create a copy of the input text, iterate over the string until a newline is found,
    //set the newline to zero, then strdup starting from just after the previous newline
    //(or from the beginning of the string)
    if (text == NULL) {
        fprintf(stderr,"Pointer to string was NULL.\n");
        abort();
    }
    char* text_copy = mstrdup(text);
    char* to_free = text_copy;
    int length = 0;
    line_node prev = NULL;
    line_node head = NULL;
    int string_index = 0;
    int curr_string_start = 0;
    while (text_copy[string_index] != 0) {				//split string an create nodes out of lines
        if (text_copy[string_index] == '\n') {
            text_copy[string_index] = 0;
            length += 1;
            line_node new_node = create_node(text_copy+curr_string_start);
            if (!head) head = new_node;
            else prev->next = new_node;
            prev = new_node;
            curr_string_start = string_index + 1;
        } 
        string_index++;
    }
    struct textbuffer* new_buffer = create_buffer(head,length);
    free(to_free);
    return new_buffer;
}

/* Free the memory occupied by the given textbuffer.  It is an error to access
 * the buffer afterwards.
 */
void releaseTB (TB tb) {
    //check inputs are valid
    //call functions to free all the memory associated with the buffer
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    destroy_stack(&(tb->redo_stack));
    destroy_stack(&(tb->undo_stack));
    list_delete(tb->head);
    free(tb);
}

/* Allocate and return an array containing the text in the given textbuffer.
 * add a prefix corrosponding to line number iff showLineNumbers == TRUE
 */
char *dumpTB (TB tb, int showLineNumbers) {
    //check inputs are valid
    //first traverse the buffer to determine how much memory will be needed for the dump
    //more complex calculations to determine size recquired if show line numbers is true
    //malloc memory of the appropriate size, then loop through the buffer and sequentially copy
    //in strings as recquired, allowing space for newlines
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    char* separator = ". ";
    int start_index = 1;
    int total_length = 1;
    int extra_spaces = (showLineNumbers?2 + strlen(separator):1);
    int line_number = start_index;
    int digit_upto = start_index;
    for (line_node curr = tb->head; curr != NULL; curr = curr->next) {		//calculate size of memory recquired
        if (line_number/digit_upto == 10 && showLineNumbers) {
            digit_upto*=10;
            extra_spaces += 1;
        }
        total_length += (strlen(curr->line) + extra_spaces);
        line_number += 1;
    }
    char* arr = (char*) mmalloc(sizeof(char)*total_length);
    int i = 0;
    line_number = start_index;
    char temp_num[10] = {0};
    for (line_node curr = tb->head; curr != NULL; curr = curr->next) {		//loop back through, copying into string
        if (showLineNumbers) {
            snprintf(temp_num,10,"%d",line_number);
            strcpy(arr+i,temp_num);
            i+=(strlen(temp_num));
            strcpy(arr+i,separator);
            i+= (strlen(separator));
        }
        strcpy(arr+i,curr->line);
        i += (strlen(curr->line) + 1);
        
        arr[i-1] = '\n';
        line_number+=1;
    }
    arr[i] = 0;

    return arr;
}

/* Return the number of lines of the given textbuffer.
 */
int linesTB (TB tb){
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    return tb->length;
}

/* Add a given prefix to all lines between pos1 and pos2
 *
 * - The program is to abort() with an error message if line 'pos1' or line
 *   'pos2' is out of range.  The first line of a textbuffer is at position 0.
 */
void addPrefixTB (TB tb, int pos1, int pos2, char* prefix){
    //check for invalid inputs
    //since this action changes the state of buffer but isn't availble for undo and redo,
    //free the undo and redo stacks
    //for all lines with position between pos1 and pos2, malloc memory of the appropriate size,
    //copy the new strings into this memory, free old memeory and make the buffer point at the new
    //memory
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }

    if (prefix == NULL) {
        fprintf(stderr,"Pointer to input string was NULL.\n");
        abort();
    } else if (pos1 < 0 || pos2 >= tb->length || pos1 > pos2) {
        fprintf(stderr,"Range of lines to prefix out of range of textbuffer.\n");
        abort();
    } else if (prefix[0] == 0) {
        return;
    }

    destroy_stack(&(tb->redo_stack));
    destroy_stack(&(tb->undo_stack));

    int line = 0;
    line_node curr = tb->head;

    while (curr != NULL) {

        if (line >= pos1 && line <= pos2) {								//malloc, copy, switch, free for appropriate lines
            int line_length = strlen(prefix) + strlen(curr->line) + 1;
            char* new_line = (char*) mmalloc(line_length);
            strcpy(new_line,prefix);
            strcat(new_line,curr->line);
            free(curr->line);
            curr->line = new_line;
        }

        line++;
        curr = curr->next;
    }


}


/* Merge 'tb2' into 'tb1' at line 'pos'.
 *
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' can not be used anymore (as if we had used
 *   releaseTB() on it).
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void mergeTB (TB tb1, int pos, TB tb2){
    //check for invalid inputs and for a few special cases where the solution is trivial
    //push the state of the buffer onto the undo stack
    //otherwise obtain pointers to the ends of both lists, and then find the correct location to insert in tb1
    //'patch' in the nodes from tb2 and then free all memory associated with tb2 (apart from the list it used to point to)
    if (tb1 == NULL || tb2 == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    push_state(tb1);
    if (pos < 0 || pos > tb1->length) {
        fprintf(stderr,"Start line to merge out of range of textbuffer.\n");
        abort();
    }
    if (tb1 == tb2) return;							
    if (linesTB(tb1) == 0 && linesTB(tb2) != 0) {				//trivial cases
        tb1->head = tb2->head;
        tb1->length = tb2->length;
        tb2->head = NULL;
        releaseTB(tb2);
        return;
    } else if (linesTB(tb1) != 0 && linesTB(tb2) == 0) {
        releaseTB(tb2);
        return;
    } else if (linesTB(tb1) == 0 && linesTB(tb2) == 0) {
        tb2->head = NULL;
        releaseTB(tb2);
        return;
    }

    line_node tb1_last = tb1->head;								//obtain last nodes
    while (tb1_last->next != NULL) tb1_last = tb1_last->next;
    line_node tb2_last = tb2->head;
    while (tb2_last->next != NULL) tb2_last = tb2_last->next;


    if (pos == 0) {												//merge as recquired
        tb2_last->next = tb1->head;
        tb1->head = tb2->head;
    } else if (pos == tb1->length) {
        tb1_last->next = tb2->head;
    } else {
        line_node curr1 = tb1->head;
        pos--;
        while (pos-- > 0) curr1 = curr1->next;
        line_node next1 = curr1->next;
        curr1->next = tb2->head;
        tb2_last->next = next1;
    }
    
    tb1->length += tb2->length;
    tb2->head = NULL;
    releaseTB(tb2);
}

/* Copy 'tb2' into 'tb1' at line 'pos'.
 *
 * - Afterwards line 0 of 'tb2' will be line 'pos' of 'tb1'.
 * - The old line 'pos' of 'tb1' will follow after the last line of 'tb2'.
 * - After this operation 'tb2' is unmodified and remains usable independent
 *   of 'tb1'.
 * - The program is to abort() with an error message if 'pos' is out of range.
 */
void pasteTB (TB tb1, int pos, TB tb2) {
    //almost indentical to mergeTB except merge a copy of the list that tb2 points to with tb1's list
    //and don't free the memory associated with tb2
    if (tb1 == NULL || tb2 == NULL) {
        fprintf(stderr,"pointer to at least one tb was NULL\n");
        abort();
    }
    push_state(tb1);
    if (pos < 0 || pos > tb1->length) {
        fprintf(stderr,"Start line to paste out of range of textbuffer.\n");
        abort();
    }
    if (linesTB(tb1) == 0 && linesTB(tb2) != 0) {					//trivial cases
        tb1->head = list_copy(tb2->head);
        tb1->length = tb2->length;
        return;
    } else if (linesTB(tb1) != 0 && linesTB(tb2) == 0) {
        return;
    } else if (linesTB(tb1) == 0 && linesTB(tb2) == 0) {
        return;
    }

    line_node tb2_head_copy = list_copy(tb2->head);					//make copy

    line_node tb1_last = tb1->head;									//obtain last nodes
    while (tb1_last->next != NULL) tb1_last = tb1_last->next;
    line_node tb2_last = tb2_head_copy;
    while (tb2_last->next != NULL) tb2_last = tb2_last->next;


    if (pos == 0) {													//merge as recquired
        tb2_last->next = tb1->head;
        tb1->head = tb2_head_copy;
    } else if (pos == tb1->length) {
        tb1_last->next = tb2_head_copy;
    } else {
        line_node curr1 = tb1->head;
        pos--;
        while (pos-- > 0) curr1 = curr1->next;
        line_node next1 = curr1->next;
        curr1->next = tb2_head_copy;
        tb2_last->next = next1;
    }
    
    tb1->length += tb2->length;

}

/* Cut the lines between and including 'from' and 'to' out of the textbuffer
 * 'tb'.
 *
 * - The result is a new textbuffer (much as one created with newTB()).
 * - The cut lines will be deleted from 'tb'.
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range.
 */
TB cutTB (TB tb, int from, int to){
    //check for invalid inputs
    //push the state of the buffer onto the undo stack
    //traverse the list and obtain pointers to the node before the start of the cut
    //(start_gap or tb->head) and the final node in the cut sequence (prev)
    //then point start_gap or tb->head to prev->next and set prev-> next = NULL
    //this 'reroutes' the orginal list around the cut sequence
    //then create a new buffer and point it at the head of the cut sequence
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    push_state(tb);
    if (from < 0 || to >= tb->length || from > to) {
        fprintf(stderr,"Range of lines to cut out of range of textbuffer.\n");
        abort();
    }
    
    int curr_line = 0;
    line_node curr = tb->head;
    line_node head = NULL;
    line_node start_gap = NULL;
    line_node prev = NULL;

    while (curr != NULL && curr_line <= to) {					//find node before start of cut sequence and node at end
        if (curr_line >= from) {
            if (head == NULL) head = curr;
        } else {
            start_gap = curr;
        }
        curr_line++;
        prev = curr;
        curr = curr->next;
    }
    if (start_gap) start_gap->next = curr;						//reroute original list
    else tb->head = curr;										//cut sequence begins at start of list edge case
    prev->next = NULL;											//ensure new list correctly terminated

    tb->length -= (to-from+1);

    struct textbuffer* new_buffer = create_buffer(head,to-from+1);

    return new_buffer;

}

/*  Return a linked list of Match nodes of all the matches of string search
 *  in tb
 *
 * - The textbuffer 'tb' will remain unmodified.
 * - The user is responsible of freeing the returned list
 */
Match searchTB (TB tb, char* search){
    //check for invalid inputs and trivial cases
    //use the string.h function 'strstr' to find pointers to matches of the search string
    //use pointer arithmetic to find the charIndex of the match and append the match to the match list
    //and then begin searching again after the match
    //do this for the whole buffer
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    if (search == NULL) {
        fprintf(stderr,"Recieved NULL string.  Aborting...\n");
        abort();
    } else if (search[0] == 0) {
        return NULL;
    }
    
    matchNode* head = NULL;
    
    line_node curr = tb->head;
    int line_number  = 1;
    int line_pos = 0;
    char* curr_line = curr->line;
    while (curr != NULL) {											//for each line use strstr to find matches
        char* ptr = strstr(curr_line+line_pos,search);				//continue applying strstr to remaining string
        if (ptr) {													//until no more matches are found
            int char_index = (ptr - curr->line);
            head = append_match(head, line_number, char_index);
            line_pos = (char_index + strlen(search));
        } else {
            curr = curr->next;
            if (curr) {
                curr_line = curr->line;
                line_pos = 0;
            }
            line_number++;
        }

    }

    return head;
}

/* Remove the lines between and including 'from' and 'to' from the textbuffer
 * 'tb'.
 *
 * - The program is to abort() with an error message if 'from' or 'to' is out
 *   of range.
 */
void deleteTB (TB tb, int from, int to){
    //check for invalid inputs
    //push the state of the buffer onto the undo stack
    //loop through the buffer, if the node is in the correct range to be deleted, free 
    //memory associated with it, and make the node before it point to the node after it
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    push_state(tb);
    if (from < 0 || to >= tb->length || from > to) {
        fprintf(stderr,"Range of lines to delete out of range of textbuffer.\n");
        abort();
    }

    int curr_line = 0;

    line_node curr = tb->head;
    line_node prev = NULL;

    while (curr != NULL && curr_line <= to) {					//for each each line that needs to be deleted
        if (curr_line >= from) {								//make the node pointng to it point to the node after
            line_node next = curr->next;
            free(curr->line);
            free(curr);
            if (prev) prev->next = next;
            else tb->head = next;								//no previous node edge case, need to modify head
            curr = next;
            tb->length--;
        } else {
            prev = curr;
            curr = curr->next;
        }
        curr_line++;
    }
}



/* Search every line of tb for each occurrence of a set of specified subsitituions
 * and alter them accordingly
 *
 * refer to spec for table.
 */
void formRichText (TB tb){
    //check for errors, initalize strings containing the substitutions
    //call the function to calculate the ammount of extra memory recquired
    //and malloc the extra memory
    //if there is nothing to substitute (add_length ==0), continue
    //if we have a '#' special symbol (add_length ==8), the solution is trivial
    //otherwise loop through the line, copying in substitutions as recquired
    //this function uses the same logic as formRichText_counter to determine whether
    //substitions are recquired, but it also makes the substituions
    //see helper functions for more details on this logic
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    destroy_stack(&(tb->redo_stack));
    char* sc1_start = "<b>";
    char* sc1_end = "</b>";
    char* sc2_start = "<i>";
    char* sc2_end = "</i>";
    char* sc3_start = "<h1>";
    char* sc3_end = "</h1>";

    for (line_node curr = tb->head; curr != NULL; curr = curr->next) {
        int add_length = formRichText_counter(curr->line);
        if (add_length == 0) continue;											//nothing to do here
        int src_line_length = strlen(curr->line);
        int dest_counter = 0;
        char sign = 0;
        char* new_line = (char*) mmalloc(src_line_length + add_length + 1);


        if (add_length == 8) {													//'#' case
            strcpy(new_line,sc3_start);
            strcat(new_line,curr->line+1);
            strcat(new_line,sc3_end);
        } else {
            for (int src_counter = 0; src_counter < src_line_length; src_counter++) {
                if (sign == 0 && (curr->line[src_counter] == '*' || curr->line[src_counter] == '_')
                && char_present(curr->line+src_counter,curr->line[src_counter])) {			//check if this char is
                    sign = curr->line[src_counter];											//going to be substituted
                    char* symbol = (sign=='*'?sc1_start:sc2_start);
                    strcpy(new_line+dest_counter,symbol);									//perform substitution
                    dest_counter+=3;
                } else if (curr->line[src_counter] == sign) {								//close tag
                    char* symbol = (sign=='*'?sc1_end:sc2_end);
                    strcpy(new_line+dest_counter,symbol);									//perform substitution		
                    dest_counter+=4;
                    sign = 0;
                } else {
                    new_line[dest_counter] = curr->line[src_counter];						//simply copy over normal chars
                    dest_counter++;
                }
            }
        }
        new_line[src_line_length+add_length] = 0;
        free(curr->line);	
        curr->line = new_line;
    }
}




void undoTB (TB tb) {
    //check that the inputs are valid
    //if there are previous states in the undo stack, pop off the most recent state,
    //make the tb point to the head of the list that is being restored
    //make the item popped of the stack point to tb's (old) list
    //push this item onto the redo stack
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    if (tb->undo_stack == NULL) return;
    struct stack_buffer* state = tb->undo_stack;					//pop
    tb->undo_stack = state->next;		
    line_node old_head = tb->head;									//store copies of values in node
    int old_length = tb->length;
    tb->head = state->head;											//set new values
    tb->length = state->length;
    state->head = old_head;											//push popped node onto redo stack with old values
    state->length = old_length;						
    state->next = tb->redo_stack;
    tb->redo_stack = state;
}

void redoTB (TB tb) {
    //check that the inputs are valid
    //if there are previous states in the redo stack, pop off the most recent state,
    //make the tb point to the head of the list that is being restored
    //make the item popped of the stack point to tb's (old) list
    //push this item onto the undo stack
    if (tb == NULL) {
        fprintf(stderr,"pointer to tb was NULL\n");
        abort();
    }
    if (tb->redo_stack == NULL) return;
    struct stack_buffer* state = tb->redo_stack;				//pop
    tb->redo_stack = state->next;
    line_node old_head = tb->head;								//store copies of values in node
    int old_length = tb->length;
    tb->head = state->head;										//set new values
    tb->length = state->length;
    state->head = old_head;										//push popped node onto redo stack with old value
    state->length = old_length;
    state->next = tb->undo_stack;
    tb->undo_stack = state;
}


//HELPER FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//check whether a given special character should be substituted by first checking
//that the next character isn't 0 (character is at end of string) or itself (c)
//then, beginning from 2 positions down, see if there are any characters
//that match this one (==c), terminate at end of string
//if no match found, return false
static int char_present(char* line, char c) {
    if (line[1] == 0 || line[1] == c) return 0;
    for (int i = 2; line[i] != 0; i++) {
        if (line[i] == c) return 1;
    }
    return 0;
}

//if there is a hash at the beginning, return a size of 8 (strlen("<h1>") +strlen(</h1>) - strlen("#") = 8)
//otherwise move through the string until we find a special character
//use the char_present function to determine whether this special character will be substituted
//if this value will be substituted, set sign to '*' or '-' and then seach for the matching special character
//continue searching for special characters to substitute until end of string

static int formRichText_counter(char* line) {

    if (line[0] == '#') return 8;

    int line_length = strlen(line);
    int add_length = 0;
    char sign = 0;
    
    for (int i = 0; i < line_length; i++) {
        if (sign == 0) {
            if ((line[i] == '*' || line[i] == '_') && char_present(line+i,line[i])) {
                sign = line[i];
            }
        } else if (line[i] == sign) {
            add_length += 5;
            sign = 0;
        }
    }
    return add_length;
}


static int stack_length(struct stack_buffer* curr) {
    int length = 0;
    while (curr != NULL) {
        length++;
        curr = curr->next;
    }
    return length;
}

static void trim_stack(struct stack_buffer* curr) {
    while (curr->next->next != NULL) {
        curr = curr->next;
    }
    list_delete(curr->next->head);
    free(curr->next);
    curr->next = NULL;
}

static void push_state(TB tb) {
    destroy_stack(&(tb->redo_stack));
    if (stack_length(tb->undo_stack) >= 10) {
        trim_stack(tb->undo_stack);
    }
    struct stack_buffer* new_tb = mmalloc(sizeof(struct textbuffer));
    new_tb->length = tb->length;
    new_tb->head = list_copy(tb->head);
    new_tb->next = tb->undo_stack;
    tb->undo_stack = new_tb;
}

static void destroy_stack(struct stack_buffer** stack_head) { 

    struct stack_buffer* next, *curr = (*stack_head);
    while (curr != NULL) {
        next = curr->next;
        list_delete(curr->head);
        free(curr);
        curr = next;
    }
    *stack_head = NULL;
}


static matchNode* append_match(matchNode* head, int line, int char_index) {
    matchNode* new_node = malloc(sizeof(matchNode));
    new_node->lineNumber = line;
    new_node->charIndex = char_index;
    new_node->next = NULL;
    if (head) {
        matchNode* end = head;
        while (end->next) end = end->next;
        end->next = new_node;
        return head;
    } else {
        return new_node;
    }
}

static void* mmalloc(int size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr,"Could not allocate memory.  Aborting...\n");
        abort();
    }
    return ptr;
}

static char* mstrdup(char* src) {
    char* ptr = strdup(src);
    if (!ptr) {
        fprintf(stderr,"Could not allocate memory.  Aborting...\n");
        abort();
    }
    return ptr;
}

static line_node create_node(char* line) {
    line_node new_node = (line_node) mmalloc(sizeof(struct _line_node));
    new_node->line = mstrdup(line);
    new_node->next = NULL;
    return new_node;
}

static struct textbuffer* create_buffer(line_node head, int length) {
    struct textbuffer* new_buffer = (struct textbuffer*) mmalloc(sizeof(struct textbuffer));
    new_buffer->head = head;
    new_buffer->length = length;
    new_buffer->redo_stack = NULL;
    new_buffer->undo_stack = NULL;
    return new_buffer;
}

static line_node list_copy(line_node head) {
    line_node src_curr = head;
    line_node prev = NULL;
    line_node new_head = NULL;
    while (src_curr != NULL) {
        line_node new_node = create_node(src_curr->line);
        if (prev) prev->next = new_node;
        else new_head = new_node;
        prev = new_node;
        src_curr = src_curr->next;
    }
    return new_head;
}

static void list_delete(line_node head) {
    line_node next = NULL;
    while (head != NULL) {
        next = head->next;
        free(head->line);
        free(head);
        head = next;
    }
}




static void test1a(void);
static void test1b(void);
static void test2a(void);
static void test2b(void);
static void test3a(void);
static void test3b(void);
static void test3c(void);
static void test5a(void);
static void test5b(void);
static void test5c(void);
static void test5d(void);
static void test5e(void);
static void test6a(void);
static void test6b(void);
static void test6c(void);
static void test7a(void);
static void test7b(void);
static void test7c(void);
static void test8a(void);
static void test8b(void);
static void test8c(void);
static void test8d(void);
static void test8e(void);


#include <assert.h>

/* Your whitebox tests
 */
void whiteBoxTests() {
    test1a();
    test1b();	
    test2a();
    test2b();
    test3a();
    test3b();
    test3c();
    test5a();
    test5b();
    test5c();
    test5d();
    test5e();
    test6a();
    test6b();
    test6c();
    test7a();
    test7b();
    test7c();
    test8a();
    test8b();
    test8c();
    test8d();
    test8e();
    
}

static void test1a(void) {
    //test the node list is created correctly
    TB tb = newTB("");
    assert(tb->head == NULL);
    releaseTB(tb);
    printf("test1a passed!\n");
}

static void test1b(void) {
    //test the node list is created correctly
    TB tb = newTB("hello world\n");
    assert(tb->head->next == NULL);
    releaseTB(tb);
    printf("test1b passed!\n");
}

static void test2a(void) {
    //test the prefix function maintains the list
    TB tb = newTB("world\n");
    addPrefixTB(tb,0,0,"hello ");
    assert(tb->head->next == NULL);
    releaseTB(tb);
    printf("test2a passed!\n");
}

static void test2b(void) {
    //test the prefix function maintains the list
    TB tb = newTB("world\n"
                    "there\n");
    addPrefixTB(tb,0,1,"hello ");
    assert(tb->head->next->next == NULL);
    releaseTB(tb);
    printf("test2b passed!\n");
}


static void test3a(void) {
    //make sure the delete function maintains the validity of the list
    TB tb = newTB("world\n"
                    "there\n");
    line_node node_2 = tb->head->next;
    deleteTB(tb,0,0);
    assert(tb->head->next == NULL);
    assert(tb->head == node_2);
    releaseTB(tb);
    printf("test2b passed!\n");
}

static void test3b(void) {
    //make sure the delete function maintains the validity of the list
    TB tb = newTB("world\n"
                    "there\n");
    line_node node_1 = tb->head;
    deleteTB(tb,1,1);
    assert(tb->head->next == NULL);
    assert(tb->head == node_1);
    releaseTB(tb);
    printf("test3b passed!\n");
}

static void test3c(void) {
    //make sure the delete function maintains the validity of the list
    TB tb = newTB("world\n"
                    "there\n");
    deleteTB(tb,0,1);
    assert(tb->head == NULL);
    releaseTB(tb);
    printf("test3c passed!\n");
}

static void test5a(void) {
    //test that the program cuts a single line at beginning correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    line_node node_1 = tb1->head;
    TB tb2 = cutTB(tb1,0,0);
    assert(tb2->head->next == NULL);
    assert(tb2->head == node_1);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test5a passed!\n");
}

static void test5b(void) {
    //test that the program cuts a single line in the middle correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    line_node node_2 = tb1->head->next;
    TB tb2 = cutTB(tb1,1,1);
    assert(tb2->head->next == NULL);
    assert(tb2->head == node_2);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test5b passed!\n");
}

static void test5c(void) {
    //test that the program cuts a single line at the end correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    line_node node_4 = tb1->head->next->next->next;
    TB tb2 = cutTB(tb1,3,3);
    assert(tb2->head->next == NULL);
    assert(tb2->head == node_4);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test5c passed!\n");
}

static void test5d(void) {
    //test that the program cuts multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    line_node node_2 = tb1->head->next;
    line_node node_3 = tb1->head->next->next;
    TB tb2 = cutTB(tb1,1,2);
    assert(tb2->head->next->next == NULL);
    assert(tb2->head == node_2);
    assert(tb2->head->next == node_3);
    assert(tb1->head->next->next == NULL);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test5d passed!\n");
}

static void test5e(void) {
    //test that the program cuts multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    line_node node_1 = tb1->head;
    line_node node_2 = tb1->head->next;
    line_node node_3 = tb1->head->next->next;
    line_node node_4 = tb1->head->next->next->next;
    TB tb2 = cutTB(tb1,0,3);
    assert(tb2->head->next->next->next->next == NULL);
    assert(tb2->head == node_1);
    assert(tb2->head->next == node_2);
    assert(tb2->head->next->next == node_3);
    assert(tb2->head->next->next->next == node_4);
    assert(tb1->head == NULL);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test5e passed!\n");
}

static void test6a(void) {
    //test that the program merges buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    mergeTB(tb1,2,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head->next->next == node_3);
    assert(tb1->head->next->next->next == node_4);
    releaseTB(tb1);
    printf("test6a passed!\n");
}

static void test6b(void) {
    //test that the program merges buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    mergeTB(tb1,0,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head == node_3);
    assert(tb1->head->next == node_4);
    releaseTB(tb1);
    printf("test6b passed!\n");
}

static void test6c(void) {
    //test that the program merges buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    mergeTB(tb1,1,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head->next == node_3);
    assert(tb1->head->next->next == node_4);
    releaseTB(tb1);
    printf("test6c passed!\n");
}

static void test7a(void) {
    //test that the program pastes buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    pasteTB(tb1,2,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head->next->next != node_3);
    assert(tb1->head->next->next->next != node_4);
    releaseTB(tb1);
    assert(tb2->head == node_3);
    assert(tb2->head->next == node_4);
    releaseTB(tb2);
    printf("test7a passed!\n");
}

static void test7b(void) {
    //test that the program pastes buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    pasteTB(tb1,0,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head != node_3);
    assert(tb1->head->next != node_4);
    releaseTB(tb1);
    assert(tb2->head == node_3);
    assert(tb2->head->next == node_4);
    releaseTB(tb2);
    printf("test7b passed!\n");
}

static void test7c(void) {
    //test that the program pastes buffers correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n");
    TB tb2 = newTB("line_3\n"
                    "line_4\n");
    line_node node_3 = tb2->head;
    line_node node_4 = tb2->head->next;
    pasteTB(tb1,1,tb2);
    assert(tb1->head->next->next->next->next == NULL);
    assert(tb1->head->next != node_3);
    assert(tb1->head->next->next != node_4);
    releaseTB(tb1);
    assert(tb2->head == node_3);
    assert(tb2->head->next == node_4);
    releaseTB(tb2);
    printf("test7c passed!\n");
}


static void test8a(void) {
    //test that the program deletes a single line at beginning correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb1,0,0);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb1);
    printf("test8a passed!\n");
}

static void test8b(void) {
    //test that the program deletes a single line in the middle correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb1,1,1);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb1);
    printf("test8b passed!\n");
}

static void test8c(void) {
    //test that the program deletes a single line at the end correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb1,3,3);
    assert(tb1->head->next->next->next == NULL);
    releaseTB(tb1);
    printf("test8c passed!\n");
}

static void test8d(void) {
    //test that the program deletes multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb1,1,2);
    assert(tb1->head->next->next == NULL);
    releaseTB(tb1);
    printf("test8d passed!\n");
}

static void test8e(void) {
    //test that the program deletes multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb1,0,3);
    assert(tb1->head == NULL);
    releaseTB(tb1);
    printf("test8e passed!\n");
}
