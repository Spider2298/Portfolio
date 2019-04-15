#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "textbuffer.h"

#define TRUE 1
#define FALSE 0


void free_search(Match curr) {
    Match next;
    while(curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

int size_search(Match curr) {
    int length = 0;
    while(curr != NULL) {
        curr = curr->next;
        length++;
    }
    return length;
}

void display_search(Match curr) {
    while (curr != NULL) {
        printf("line: %d, char_index: %d\n",curr->lineNumber,curr->charIndex);
        curr = curr->next;
    }
}


void test1a(void);
void test1b(void);
void test2a(void);
void test3a(void);
void test3b(void);
void test3c(void);
void test3d(void);
void test4a(void);
void test4b(void);
void test4c(void);
void test4d(void);
void test4e(void);
void test4f(void);
void test5a(void);
void test5b(void);
void test5c(void);
void test5d(void);
void test5e(void);
void test5f(void);
void test5g(void);
void test5h(void);
void test6a(void);
void test6b(void);
void test6c(void);
void test6d(void);
void test6e(void);
void test7a(void);
void test7b(void);
void test8a(void);
void test8b(void);
void test8c(void);
void test8d(void);
void test8e(void);
void test9a(void);
void test9b(void);
void test9c(void);
void test9d(void);
void test9e(void);
void test9f(void);
void test10a(void);
void test10b(void);
void test10c(void);



int main(int argc, char *argv[]) {
    printf("running blackbox tests...\n");
    test1a();
    test1b();
    test2a();
    test3a();
    test3b();
    test3c();
    test3d();
    test4a();
    test4b();
    test4c();
    test4d();
    test4e();
    test4f();
    test5a();
    test5b();
    test5c();
    test5d();
    test5e();
    test5f();
    test5g();
    test5h();
    test6a();
    test6b();
    test6c();
    test6d();
    test6e();
    test7a();
    test7b();
    test8a();
    test8b();
    test8c();
    test8d();
    test8e();
    test9a();
    test9b();
    test9c();
    test9d();
    test9e();
    test9f();
    test10a();
    test10b();
    test10c();
    printf("blackbox tests complete.\n");
    //printf("running whitebox tests...\n");
    //whiteBoxTests();
    //printf("whitebox tests complete.\n");
    return EXIT_SUCCESS;
}


void test1a(void) {
    //test that the buffer can create a single line of text and dump it correctly
    char text[] = "A line of text\n";
    TB tb = newTB(text);
    assert(linesTB(tb) == 1);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp(text,text_dump) == 0);
    assert(linesTB(tb) == 1);
    free(text_dump);
    releaseTB(tb);
    printf("test1a passed!\n");
}

void test1b(void) {
    //test that the program can create a multiple lines of text and dump it correctly
    char text[] = "A line of text\nAnother line of text\n";
    TB tb = newTB(text);
    assert(linesTB(tb) == 2);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp(text,text_dump) == 0);
    assert(linesTB(tb) == 2);
    free(text_dump);
    releaseTB(tb);
    printf("test1b passed!\n");
}

void test2a(void) {
    //test that the program handles edge cases relating to empty strings and single newline characters correctly
    TB tb1 = newTB("");
    assert(linesTB(tb1) == 0);
    TB tb2 = newTB("\n");
    assert(linesTB(tb2) == 1);
    TB tb3 = newTB("\n\n");
    assert(linesTB(tb3) == 2);
    releaseTB(tb1); releaseTB(tb2); releaseTB(tb3);
    printf("test2a passed!\n");
}

void test3a(void) {
    //test that the program prefixes single lines of text correctly
    TB tb = newTB("suffix\n");
    addPrefixTB(tb,0,0,"hello");
    assert(linesTB(tb) == 1);
    char* text_dump = dumpTB(tb, 0);
    assert(strcmp("hellosuffix\n",text_dump) == 0);
    assert(linesTB(tb) == 1);
    releaseTB(tb);
    free(text_dump);
    printf("test3a passed!\n");
}

void test3b(void) {
    //test that the program prefixes multiple lines of text correctly
    TB tb = newTB("1\n2\n3\n");
    addPrefixTB(tb,0,1,"hello");
    assert(linesTB(tb) == 3);
    char* text_dump = dumpTB(tb, 0);
    assert(strcmp("hello1\nhello2\n3\n",text_dump) == 0);
    assert(linesTB(tb) == 3);
    releaseTB(tb);
    free(text_dump);
    printf("test3b passed!\n");
}

void test3c(void) {
    //test that the program does nothing if an empty string is passed in to prefix
    TB tb = newTB("1\n2\n3\n");
    addPrefixTB(tb,0,1,"");
    assert(linesTB(tb) == 3);
    char* text_dump = dumpTB(tb, 0);
    assert(strcmp("1\n2\n3\n",text_dump) == 0);
    assert(linesTB(tb) == 3);
    releaseTB(tb);
    free(text_dump);
    printf("test3c passed!\n");
}

void test3d(void) {
    //test that the program prefixes multiple lines of text correctly
    TB tb = newTB("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    addPrefixTB(tb,0,14,"a");
    char* text_dump = dumpTB(tb, 0);
    assert(strcmp("a\na\na\na\na\na\na\na\na\na\na\na\na\na\na\n",text_dump) == 0);
    assert(linesTB(tb) == 15);
    releaseTB(tb);
    free(text_dump);
    printf("test3d passed!\n");
}

void test4a(void) {
    //test that the program merges two buffers correctly at the head
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    mergeTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_3\nline_4\nline_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    printf("test4a passed!\n");
}

void test4b(void) {
    //test that the program merges two buffers correctly in the middle
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    mergeTB(tb1,1,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_3\nline_4\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    printf("test4b passed!\n");
}

void test4c(void) {
    //test that the program merges two buffers correctly at the end
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    mergeTB(tb1,2,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    printf("test4c passed!\n");
}

void test4d(void) {
    //test that the program merges empty and non-empty buffers correctly
    TB tb1 = newTB("");
    TB tb2 = newTB("line_3\nline_4\n");

    mergeTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    releaseTB(tb1);
    free(text_dump);
    printf("test4d passed!\n");
}

void test4e(void) {
    //test that the program merges two buffers correctly at the end
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("");

    mergeTB(tb1,2,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    releaseTB(tb1);
    free(text_dump);
    printf("test4e passed!\n");
}

void test4f(void) {
    //test that the program merges two buffers correctly at the end
    TB tb1 = newTB("");
    TB tb2 = newTB("");

    mergeTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("",text_dump) == 0);
    assert(linesTB(tb1) == 0);
    releaseTB(tb1);
    free(text_dump);
    printf("test4f passed!\n");
}

void test4g(void) {
    //test that the program merges small buffers correctly
    TB tb1 = newTB("line_1\n");
    TB tb2 = newTB("line_2\n");

    mergeTB(tb1,1,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    releaseTB(tb1);
    free(text_dump);
    printf("test4g passed!\n");
}

void test4h(void) {
    //test that the program merges small buffers correctly
    TB tb1 = newTB("line_1\n");
    TB tb2 = newTB("line_2\n");

    mergeTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_2\nline_1\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    releaseTB(tb1);
    free(text_dump);
    printf("test4h passed!\n");
}

void test5a(void) {
    //test that the program pastes two buffers correctly at the head
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    pasteTB(tb1,0,tb2);
    assert(linesTB(tb1) == 4);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_3\nline_4\nline_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb2) == 2);
    releaseTB(tb2);
    free(text_dump);
    printf("test5a passed!\n");
}

void test5b(void) {
    //test that the program pastes two buffers correctly in the middle
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    pasteTB(tb1,1,tb2);
    assert(linesTB(tb1) == 4);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_3\nline_4\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb2) == 2);
    releaseTB(tb2);
    free(text_dump);
    printf("test5b passed!\n");
}

void test5c(void) {
    //test that the program pastes two buffers correctly at the end
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("line_3\nline_4\n");

    pasteTB(tb1,2,tb2);
    assert(linesTB(tb1) == 4);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 4);
    releaseTB(tb1);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb2) == 2);
    releaseTB(tb2);
    free(text_dump);
    printf("test5c passed!\n");
}

void test5d(void) {
    //test that the program pastes empty and non-empty buffers correctly
    TB tb1 = newTB("");
    TB tb2 = newTB("line_3\nline_4\n");

    pasteTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    assert(linesTB(tb2) == 2);
    releaseTB(tb2);
    releaseTB(tb1);
    free(text_dump);
    printf("test5d passed!\n");
}

void test5e(void) {
    //test that the program pastes empty and non-empty buffers correctly
    TB tb1 = newTB("line_1\nline_2\n");
    TB tb2 = newTB("");

    pasteTB(tb1,2,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    assert(linesTB(tb2) == 0);
    releaseTB(tb2);
    releaseTB(tb1);
    free(text_dump);
    printf("test5e passed!\n");
}

void test5f(void) {
    //test that the program pastes empty buffers correctly
    TB tb1 = newTB("");
    TB tb2 = newTB("");

    pasteTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("",text_dump) == 0);
    assert(linesTB(tb1) == 0);
    assert(linesTB(tb2) == 0);
    releaseTB(tb2);
    releaseTB(tb1);
    free(text_dump);
    printf("test5f passed!\n");
}

void test5g(void) {
    //test that the program pastes small buffers correctly
    TB tb1 = newTB("line_1\n");
    TB tb2 = newTB("line_2\n");

    pasteTB(tb1,1,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    assert(linesTB(tb2) == 1);
    releaseTB(tb2);
    releaseTB(tb1);
    free(text_dump);
    printf("test5g passed!\n");
}

void test5h(void) {
    //test that the program pastes small buffers correctly
    TB tb1 = newTB("line_1\n");
    TB tb2 = newTB("line_2\n");

    pasteTB(tb1,0,tb2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_2\nline_1\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    assert(linesTB(tb2) == 1);
    releaseTB(tb2);
    releaseTB(tb1);
    free(text_dump);
    printf("test5h passed!\n");
}

void test6a(void) {
    //test that the program deletes a single line at beginning correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb,0,0);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp("line_2\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb) == 3);
    free(text_dump);
    releaseTB(tb);
    printf("test6a passed!\n");
}

void test6b(void) {
    //test that the program deletes a single line in the middle correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb,1,1);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp("line_1\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb) == 3);
    free(text_dump);
    releaseTB(tb);
    printf("test6b passed!\n");
}

void test6c(void) {
    //test that the program deletes a single line at the end correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb,3,3);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp("line_1\nline_2\nline_3\n",text_dump) == 0);
    assert(linesTB(tb) == 3);
    free(text_dump);
    releaseTB(tb);
    printf("test6c passed!\n");
}

void test6d(void) {
    //test that the program deletes a multiples lines of text correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb,1,2);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp("line_1\nline_4\n",text_dump) == 0);
    assert(linesTB(tb) == 2);
    free(text_dump);
    releaseTB(tb);
    printf("test6d passed!\n");
}

void test6e(void) {
    //test that the program deletes a multiples lines of text correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    deleteTB(tb,0,3);
    char* text_dump = dumpTB(tb,0);
    assert(strcmp("",text_dump) == 0);
    assert(linesTB(tb) == 0);
    free(text_dump);
    releaseTB(tb);
    printf("test6e passed!\n");
}

void test7a(void) {
    //test that the program dumps multiples lines of text with line numbers correctly
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    char* text_dump = dumpTB(tb,1);
    assert(strcmp("1. line_1\n2. line_2\n3. line_3\n4. line_4\n",text_dump) == 0);
    assert(linesTB(tb) == 4);
    free(text_dump);
    releaseTB(tb);
    printf("test7a passed!\n");
}

void test7b(void) {
    //test that the program dumps multiples lines of text with line numbers correctly
    TB tb = newTB("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    char* text_dump = dumpTB(tb,1);
    assert(strcmp("1. \n2. \n3. \n4. \n5. \n6. \n7. \n8. \n9. \n10. \n11. \n12. \n13. \n14. \n15. \n",text_dump) == 0);
    assert(linesTB(tb) == 15);
    free(text_dump);
    releaseTB(tb);
    printf("test7b passed!\n");
}

void test8a(void) {
    //test that the program cuts a single line at beginning correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    TB tb2 = cutTB(tb1,0,0);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_2\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 3);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_1\n",text_dump) == 0);
    assert(linesTB(tb2) == 1);
    free(text_dump);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test8a passed!\n");
}

void test8b(void) {
    //test that the program cuts a single line in the middle correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    TB tb2 = cutTB(tb1,1,1);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 3);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_2\n",text_dump) == 0);
    assert(linesTB(tb2) == 1);
    free(text_dump);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test8b passed!\n");
}

void test8c(void) {
    //test that the program cuts a single line at the end correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    TB tb2 = cutTB(tb1,3,3);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_2\nline_3\n",text_dump) == 0);
    assert(linesTB(tb1) == 3);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_4\n",text_dump) == 0);
    assert(linesTB(tb2) == 1);
    free(text_dump);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test8c passed!\n");
}

void test8d(void) {
    //test that the program cuts multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    TB tb2 = cutTB(tb1,1,2);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("line_1\nline_4\n",text_dump) == 0);
    assert(linesTB(tb1) == 2);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_2\nline_3\n",text_dump) == 0);
    assert(linesTB(tb2) == 2);
    free(text_dump);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test8d passed!\n");
}

void test8e(void) {
    //test that the program cuts multiples lines of text correctly
    TB tb1 = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    TB tb2 = cutTB(tb1,0,3);
    char* text_dump = dumpTB(tb1,0);
    assert(strcmp("",text_dump) == 0);
    assert(linesTB(tb1) == 0);
    free(text_dump);
    text_dump = dumpTB(tb2,0);
    assert(strcmp("line_1\nline_2\nline_3\nline_4\n",text_dump) == 0);
    assert(linesTB(tb2) == 4);
    free(text_dump);
    releaseTB(tb2);
    releaseTB(tb1);
    printf("test8e passed!\n");
}

void test9a(void) {
    //test that the program finds the correct number of matches in the buffer
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    Match head = searchTB(tb,"line");
    assert(size_search(head) == 4);
    free_search(head);
    releaseTB(tb);
    printf("test9a passed!\n");
}

void test9b(void) {
    //test that the program finds the correct number of matches in the buffer
    TB tb = newTB("line_1\n"
                    "line_2\n"
                    "line_3\n"
                    "line_4\n");
    Match head = searchTB(tb,"_");
    assert(size_search(head) == 4);
    free_search(head);
    releaseTB(tb);
    printf("test9b passed!\n");
}

void test9c(void) {
    //test that the program finds the correct number of matches in the buffer
    TB tb = newTB("ii1\n"
                    "ii2\n"
                    "ii3\n"
                    "ii4\n");
    Match head = searchTB(tb,"i");
    assert(size_search(head) == 8);
    free_search(head);
    releaseTB(tb);
    printf("test9c passed!\n");
}

void test9d(void) {
    //test that the program finds the correct number of matches in the buffer
    TB tb = newTB("IIii1\n"
                    "ii2\n"
                    "ii3\n"
                    "ii__i4\n");
    Match head = searchTB(tb,"i");
    assert(size_search(head) == 9);
    free_search(head);
    releaseTB(tb);
    printf("test9d passed!\n");
}

void test9e(void) {
    //test that the program finds matches greedily
    TB tb = newTB("ababab1\n"
                    "abab\n");
    Match head = searchTB(tb,"abab");
    assert(size_search(head) == 2);
    free_search(head);
    releaseTB(tb);
    printf("test9d passed!\n");
}

void test9f(void) {
    //test that the program gives the correct positions for mathces in the buffer
    TB tb = newTB("i__i1Ii\n"
                    "i2\n"
                    "i3\n"
                    "iii4\n");
    Match head = searchTB(tb,"i");
    int expected_results[8][2] = { {1,0}, {1,3}, {1,6}, {2,0}, {3,0}, {4,0}, {4,1}, {4,2} };
    assert(size_search(head) == 8);
    int i = 0;
    Match curr = head;
    while (i < 8) {
        assert(expected_results[i][0] == curr->lineNumber);
        assert(expected_results[i][1] == curr->charIndex);
        i++;
        curr = curr->next;
    }
    free_search(head);
    releaseTB(tb);
    printf("test9e passed!\n");
}

void test10a(void) {
    //test that the program formats rich text correctly
    TB tb = newTB("#hello\n"
                    "*hello again*\n"
                    "_bye_\n");
    formRichText(tb);
    char* text_dump =  dumpTB(tb,0);
    char* expected_text = "<h1>hello</h1>\n"
                            "<b>hello again</b>\n"
                            "<i>bye</i>\n";
    //printf("exp: %s",expected_text);
    //printf("act: %s",text_dump);
    assert(strcmp(expected_text,text_dump) == 0);
    assert(linesTB(tb) == 3);
    free(text_dump);
    releaseTB(tb);
    printf("test10a passed!\n");
}

void test10b(void) {
    //test that the program formats rich text correctly with multiples substitutions on the same line
    TB tb = newTB("#hello*what*\n"
                    "*hello again*_i_*\n"
                    "_bye_\n");
    formRichText(tb);
    char* text_dump =  dumpTB(tb,0);
    char* expected_text = "<h1>hello*what*</h1>\n"
                            "<b>hello again</b><i>i</i>*\n"
                            "<i>bye</i>\n";
    //printf("exp: %s",expected_text);
    //printf("act: %s",text_dump);
    assert(strcmp(expected_text,text_dump) == 0);
    assert(linesTB(tb) == 3);
    free(text_dump);
    releaseTB(tb);
    printf("test10b passed!\n");
}


void test10c(void) {
    //test that the program formats rich text correctly with edge cases
    TB tb = newTB("*h*ello**\n"
                    "he*ll_o_ again__\n"
                    "***\n"
                    "**ads*\n"
                    "*_*_*_*_*_*_\n"
                    "**hello\n");
    formRichText(tb);
    char* text_dump =  dumpTB(tb,0);
    char* expected_text = "<b>h</b>ello**\n"
                            "he*ll<i>o</i> again__\n"
                            "***\n"
                            "*<b>ads</b>\n"
                            "<b>_</b><i>*</i><b>_</b><i>*</i>\n"  //taken from discussion forums
                            "**hello\n"; 
    //printf("exp: %s\n",expected_text);
    //printf("act: %s\n",text_dump);

    assert(strcmp(expected_text,text_dump) == 0);
    assert(linesTB(tb) == 6);
    free(text_dump);
    releaseTB(tb);
    printf("test10c passed!\n");
}
