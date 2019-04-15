#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Alternative to strdup from the string.h library
char* custom_strdup(char* str) {
    assert(str != NULL);
    char* new_str = malloc(sizeof(char) * (strlen(str) + 1));
    assert(new_str != NULL);
    *new_str = '\0';
    strcat(new_str, str);
    return new_str;
}
