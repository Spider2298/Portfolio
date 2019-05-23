/*
Author: Michael Ellis
This program finds solutions to the 'Numbers Round' in the game show 'Letters and Numbers'

In this game, the contestant is assigned 6 numbers and a target number, then by adding, subtracting, multiplying or 
dividing these numbers they must get a value as close to the target as possible.
This program attempts to find the closest restult by a brute-force method and returns the solution in reverse-polish notation.

*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 6
#define CONST 512
#define LARGE_NEGATIVE_NUMBER -10000000

typedef struct data {
    int numbers[SIZE];
    int opt[2*SIZE - 1];
    int used[SIZE];
    int curr[2*SIZE - 1];
    int currOperators[SIZE - 1];
    int operators[4];
    int value;
    int elements;
    int optElements;
    int solved;
    int target;
    int min;
    int invalid_sequence;
} *Data;



int eval(int a, int b, char op, Data p);

int eval_polish(int numbers[], Data p);

void recursively_enumerate_all_permutations(Data p, int pos, int diff, int remNumbers);

void recursively_insert_operators(Data p, int pos);

void test_answer(Data p);

void copy(int* dest, int* src);

int main(void) {
    
    int numbers[SIZE];
    char operators[SIZE-1];
    
    struct data p;
    p.value = 0;
    p.solved = 0;
    p.invalid_sequence = 0;

    p.operators[0] = '+';
    p.operators[1] = '-';
    p.operators[2] = '/';
    p.operators[3] = '*';
    printf("Insert 6 numbers from which the target must be obtained.\n");
    for (int i = 0; i < SIZE; i++) {
        scanf("%d", p.numbers+i);
        p.used[i] = 0;   
    }
    printf("Insert the target number.\n");
    scanf("%d",&p.target);
    p.min = p.target;
    for (int i = 0; i < 2*SIZE - 1; i++) {
        p.opt[i] = 0;
        p.curr[i] = 0;
    }
    
    
    
    
    //int test[] = {1000, 100, 10, -2, -2, -1, -1 , -1, -1, -1, -1};
    //int test2[] = {15, 7, 1, 1, '+' - CONST, '-' - CONST, '/' - CONST, 3, '*' - CONST, 4, 1, 1, '+' - CONST, '+' - CONST, '-' - CONST};
    
    //printf("done: %d\n",eval_polish(test, operators));
    for (int i = 1; i <= SIZE; i++) {
        p.elements = i;
        recursively_enumerate_all_permutations(&p, 0, 0, SIZE);
    }
    printf("Diff: %d\n", p.min);
    
    if (p.min != p.target) {
        for (int i = 0; i < p.optElements*2-1; i++) {
            if (p.opt[i] > 0) printf("%d, ", p.opt[i]);
            else printf("%c, ", p.opt[i] + CONST);
        }
        putchar('\n');
    }
    
}

void recursively_enumerate_all_permutations(Data p, int pos, int diff, int remNumbers) {
    //printf("pos: %d\n", pos);
    if (p->solved) return;
    if (pos == p->elements*2 - 1) {
        //test inserting all possible combinations of operators into the gaps
        recursively_insert_operators(p, 0);
        return;
    }
    
    //if the number of operands - operators >= 2
    if (diff >= 2) {
        p->curr[pos] = -2;
        recursively_enumerate_all_permutations(p, pos+1, diff-1, remNumbers);
    }

    //if the number of operands to place is greater than 0
    if (remNumbers > 0) {
        for (int i = 0; i < SIZE; i++) {
            if (!p->used[i]) {
                p->curr[pos] = p->numbers[i];
                p->used[i] = 1;
                recursively_enumerate_all_permutations(p, pos+1, diff+1, remNumbers-1);
                p->used[i] = 0;
            }
        }
    }
}

void recursively_insert_operators(Data p, int pos) {
    
    if (pos == p->elements - 1) {
        test_answer(p);
        return;
    }
    
    /*
    for (int i = 0; i < p->elements*2-1; i++) {
        printf("%d, ", p->curr[i]);
    }
    putchar('\n');
    */

    /////////////if we haven't filled in all operators yet
    for (int i = 0; i < 4; i++) {
        p->currOperators[pos] = p->operators[i];
        recursively_insert_operators(p, pos+1);
    }
}

void test_answer(Data p) {
    /*
        for (int i = 0; i < 5; i++) {
            printf("%c, ", p->currOperators[i]);
        }
        putchar('\n');
        */
        
        //////////insert all operators into the curr array
        int j = 0;
        for (int i = 0; i < 2*SIZE - 1; i++) {
            if (p->curr[i] < -1) {
                p->curr[i] = p->currOperators[j++] - CONST;
            }
        }
        /*
        for (int i = 0; i < p->elements*2-1; i++) {
            if (p->curr[i] > 0) printf("%d, ", p->curr[i]);
            else printf("%c, ", p->curr[i] + CONST);
        }
        putchar('\n');
        printf("eval: %d\n", eval_polish(p->curr, p));
        
        //assert(0);
        */
        
        ////////////check how good this solution is
        int diff = p->target - eval_polish(p->curr, p);
        if (p->invalid_sequence) {
            p->invalid_sequence = 0;
            return;
        }
        if (diff < 0) diff*=-1;
        
        if (diff == 0) p->solved = 1;
        
        if (diff < p->min) {
            p->min = diff;
            copy(p->opt, p->curr);
            p->optElements = p->elements;
        }
        return;
}


int rec_eval_polish(int numbers[], int* pos, Data p) {
    //printf("%d\n", *pos);
    if (numbers[*pos] > 0) {
        //printf("returning: %d\n", numbers[*pos]);
        return numbers[*pos];
    } else {
        char operator = numbers[*pos] + CONST;
        (*pos)--;
        int b = rec_eval_polish(numbers, pos, p);
        (*pos)--;
        int a = rec_eval_polish(numbers, pos, p);
        //printf("hello\n");
        //printf("%d: returning: %d\n", *pos+2, eval(a, b, operator));
        return eval(a, b, operator, p);
    }
    
}


int eval_polish(int numbers[], Data p) {
    int pos = 2*p->elements - 2;
    return rec_eval_polish(numbers, &pos, p);
}

int eval(int a, int b, char op, Data p) {
    switch (op) {
        case '+':
            return a+b;
        case '-':
            return a-b;
        case '*':
            return a*b;
        case '/':
            
            if (b==0 || a%b!=0) {
                p->invalid_sequence = 1;
                return 0;
            }
            return a/b;
        default:
            return -1;
    }
}


void copy(int* dest, int* src) {
    for (int i = 0; i < 2*SIZE - 1; i++) dest[i] = src[i];
}
