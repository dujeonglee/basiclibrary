# basic data structures and libraries
Implementation of basic data structures and libraries for own study.

## Header only data structures
  - Linked List: Queue and Stack functions are provided.
  - Binary Search Tree: Insert, find, remove functions are provided.
  - AVL Tree: Insert, find, remove functions are provided.

    - AVL Tree banchmark results against std::map
      - Primitive types
      
        1 Sequential insert keys from 0 to 19999999
          - avltree : 8.220326 sec
          - std::map : 18.710377 sec

        2 Lookup keys from 0 to 19999999
          - avltree : 7.364360 sec
          - std::map : 11.209790 sec

        3 Delete all elements
          - avltree : 4.774075 sec
          - std::map : 14.209195 sec

      - Non-primitive types
      
        1 Sequential insert from 0 to 19999999
          - avltree : 14.797637 sec
          - std::map : 18.878864 sec

        2 Lookup 0 to 19999999
          - avltree : 7.413985 sec
          - std::map : 11.157555 sec

        3 Delete all element
          - avltree : 8.253727 sec
          - std::map : 14.500771 sec

## Libraries
  - Single-shot timer: C++11 based timer library. g++ version should be >= g++-4.9 (note: g++ compilers <=g++-4.8 has a bugs for try_lock_for.).
