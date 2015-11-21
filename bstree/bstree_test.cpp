#include <stdio.h>
#include <stdlib.h>

#include "bstree.h"

int main(void){
    bstree<int, int> tree;

    for(int i = 0 ; i < 500 ; i++){
        int data = rand()%500;
        if(tree.insert(data, data) == true){
            printf("%d is added\n", data);
        }
    }
    tree.print();
    for(int i = 0 ; i < 500 ; i++){
        int data = rand()%500;
        if(tree.remove(data) == true){
            printf("%d is removed\n", data);
        }
    }
    tree.print();
    return 0;
}
