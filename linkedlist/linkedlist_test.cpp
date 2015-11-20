#include <stdio.h>
#include "linkedlist.h"

int main(void){
    linkedlist<int> list;
    int data;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_at(3, 4);
    list.pop_at(3, &data);
    //list.push_at(1, 4);
    //list.push_at(2, 5);
    list.print();

    printf("PUSH BACK TEST: Insert integer from 1 to 10\n");
    for(int i = 1 ; i <= 10 ; i++){
        printf("%d\n", i);
        list.push_back(i);
    }
    printf("POP BACK TEST:\n");
    while(list.pop_back(&data)){
        printf("%d\n", data);
    }
    printf("PUSH BACK TEST\n");
    for(int i = 0 ; i < 10 ; i++){
        printf("%d\n", i);
        list.push_back(i);
    }
    printf("POP FRONT TEST\n");
    while(list.pop_front(&data)){
        printf("%d\n", data);
    }

    printf("PUSH FRONT TEST\n");
    for(int i = 0 ; i < 10 ; i++){
        printf("%d\n", i);
        list.push_front(i);
    }
    printf("CHECK DATA\n");
    list.print();

    printf("POP BACK TEST\n");
    while(list.pop_back(&data)){
        printf("%d\n", data);
    }
    printf("PUSH BACK TEST\n");
    for(int i = 0 ; i < 10 ; i++){
        printf("%d\n", i);
        list.push_front(i);
    }
    printf("POP FRONT TEST\n");
    while(list.pop_front(&data)){
        printf("%d\n", data);
    }

    return 0;
}
