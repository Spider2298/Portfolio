#ifndef DIST_H
#define DIST_H

//contain all the information recquired to calculate the minimum way
//to rank all the elements
//the 'arr' field contains the cost of placing element c at position p
//indexed as arr[c][p]
//the 'names' field contains the names of the elements
//no_elements self explanatory
typedef struct _data {
    double** arr;
    int no_elements;
    char** names;
} *data;

//allocate and return a pointer to a data struct
data gen_tree(int argc, char** argv);

//free all memory associated with the struct
void drop(data d);

#endif
