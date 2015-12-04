#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "avltree.h"

int main(void){
    avltree<int, int> tree;

    //srand(time(NULL));
    for(int i = 0 ; i < 50 ; i++){
        int data = rand()%100;
        if(tree.insert(data,data) == true){
            //printf("Add %d\n", data);
        }
    }
/*
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
    tree.delete_root();
*/
    tree.print();
    tree.delete_root();
    tree.print();
printf("=========\n");
    tree.insert(2,2);
    tree.print();
//    while(1);
    return 0;
}
