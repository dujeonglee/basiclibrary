#include <stdio.h>
#include <stdlib.h>

#include "avltree.h"

int main(void){
    avltree<int, int> tree;

    for(int i = 0 ; i < 10000000 ; i++){
        int data = rand();
        if(tree.insert(data,data) == true){
            //printf("Add %d\n", data);
        }
    }
    printf("size %u\n", tree.size());
    //tree.print();
    return 0;
}
