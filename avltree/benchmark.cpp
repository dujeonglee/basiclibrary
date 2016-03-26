#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <map>
#include "avltree.h"

int main()
{
    std::clock_t start;
    std::clock_t end;
    avltree<int, int> avl_tree;
    std::map<int, int> rb_tree;

    printf("PRIMITIVE\n");
    printf("Sequential insert from 0 to 19999999\n");
    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        avl_tree.insert(i, i);
    }
    end = clock();
    printf("AVL : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);
    unsigned int depth = 0;
    avl_tree.find_max_depth(avl_tree.root(), 0, &depth);
    printf("MAX Depth - %u\n", depth);
    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        rb_tree[i] = i;
    }
    end = clock();
    printf("RB : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

    printf("Lookup 0 to 19999999\n");
    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        if(avl_tree.find(i) == false){
            printf("lookup failed\n");
            break;
        }
    }
    end = clock();
    printf("AVL : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        if(rb_tree.find(i) == rb_tree.end()){
            printf("lookup failed\n");
            break;
        }
    }
    end = clock();
    printf("RB : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

    printf("Delete all element\n");
    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        avl_tree.remove(i);
    }
    end = clock();
    printf("AVL : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        rb_tree.erase(i);
    }
    end = clock();
    printf("RB : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);
}
