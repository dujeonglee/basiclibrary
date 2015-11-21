#include <stdio.h>
#include <stdlib.h>

#include "bstree.h"

int main(void){
    bstree<int, int> tree;

    for(int i = 0 ; i < 10000000 ; i++){
        int data = rand()%10000000;
        tree.insert(data, data);
    }
    printf("Clear %d\n", tree.size());
    tree.clear();
    printf("Done\n");

    for(int i = 0 ; i < 10000000 ; i++){
        int data = rand()%10000000;
        tree.insert(data, data);
    }
    printf("Clear %d\n", tree.size());
    tree.clear2();
    printf("Done\n");
    return 0;
}
