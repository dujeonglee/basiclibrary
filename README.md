# basiclibrary
basic data structure libraries for own study

Performace analysis
 - avltree VS std::map: avltree shows dramatic performance against the conventional std::map. The results are as in the following. 

    * PRIMITIVE type (unsigned char, unsigned short, ...)<br>
     : Sequential insert keys from 0 to 19999999<br>
       -> avltree : 8.220326 sec<br>
       -> std::map : 18.710377 sec<br>

     : Lookup keys from 0 to 19999999
       -> avltree : 7.364360 sec
       -> std::map : 11.209790 sec

     : Delete all elements
       -> avltree : 4.774075 sec
       -> std::map : 14.209195 sec

    * NON-PRIMITIVE (structure, classes, ...)
     : Sequential insert from 0 to 19999999
       -> avltree : 14.797637 sec
       -> std::map : 18.878864 sec

     : Lookup 0 to 19999999
       -> avltree : 7.413985 sec
       -> std::map : 11.157555 sec

     : Delete all element
       -> avltree : 8.253727 sec
       -> std::map : 14.500771 sec

