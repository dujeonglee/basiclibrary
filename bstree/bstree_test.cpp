#include <stdio.h>
#include "bstree.h"

int main(void){
    bstree<int, int> tree;

    tree.insert(5, 3);
    tree.insert(2, 4);
    tree.insert(8, 8);
    tree.print();

    tree.remove(5);
    tree.print();
    printf("F %d\n", *tree.find(8));
    printf("F %d\n", *tree.find(2));
    printf("F %d\n", (tree.find(5)==NULL?-1:*tree.find(5)));
    return 0;
}
