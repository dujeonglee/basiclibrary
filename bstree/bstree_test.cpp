#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bstree.h"

int main(void){
    bstree<int, int> tree;

    for(int i = 0 ; i < 1000000 ; i++){
        int data = rand();
        if(tree.insert(data,data) == true){
            //printf("Add %d\n", data);
        }
    }
    tree.clear();
    return 0;
}
