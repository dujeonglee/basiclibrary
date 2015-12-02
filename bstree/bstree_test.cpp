#include <stdio.h>
#include <stdlib.h>

#include "bstree.h"

int main(void){
    bstree<int, int> tree;

    for(int i = 0 ; i < 1000000 ; i++){
        int data = rand()%10000000;
    //for(int i = 0 ; i < 3 ; i++){
        //int data = i;
        tree.insert(data, data);
    }
    printf("Clear %d\n", tree.size());
    tree.clear();
    printf("Done\n");
    return 0;
}
