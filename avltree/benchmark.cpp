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

#ifdef NONPRIMITIVE_KEY
    printf("NON-PRIMITIVE\n");
#else
    printf("PRIMITIVE\n");
#endif
    printf("Sequential insert from 0 to 19999999\n");
    start = clock();
    for (unsigned int i = 0 ; i < 20000000 ; i++)
    {
        avl_tree.insert(i, i);
    }
    end = clock();
    printf("AVL : %f sec\n", (double)(end-start)/(double)CLOCKS_PER_SEC);

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

    unsigned int depth;
    bool valid;
    while(avl_tree.size() < 10240){
        avl_tree.insert(rand()%10240, 0);
        avl_tree.check_max_depth_and_validity(&depth, &valid);
        if(valid == false){
            printf("Tree does not meet AVL constraints\n");
            return 0;
        }
    }
    printf("ADD Done %u %u\n", depth, avl_tree.size());
    while(avl_tree.size() > 0){
        avl_tree.remove(rand()%10240);
        avl_tree.check_max_depth_and_validity(&depth, &valid);
        if(valid == false){
            printf("Tree does not meet AVL constraints\n");
            return 0;
        }
    }
    printf("Delete Done %u %u\n", depth, avl_tree.size());
}
