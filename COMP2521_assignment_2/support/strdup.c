#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* custom_strdup(char* str) {
    int length = strlen(str);
    char* new_str = malloc(sizeof(char)*(length+1));
    assert(new_str);
    strcpy(new_str,str);
    return new_str;
}
