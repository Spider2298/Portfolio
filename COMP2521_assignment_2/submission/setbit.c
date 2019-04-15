#include <stdio.h>


void setbit(int used, int index) {
    used = used | (1 << index);
}



int main(void) {
    int x = 0;
    
    x = x | (1 << 4);
    
    printf("%d\n",x);
}
