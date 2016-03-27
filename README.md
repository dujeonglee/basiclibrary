# basiclibrary
basic data structure libraries for own study

Performace analysis
 - linked list and bstree do not have much room for performance improvement due to their simplicty. Therefore, I do not make analysis on linked list and bstree.<br>
 - For avltree, it shows outstanding performance against the conventional std::map. The results are as in the following.<br>

    * PRIMITIVE type (unsigned char, unsigned short, ...)<br>
     : Sequential insert keys from 0 to 19999999<br>
       -> avltree : 8.220326 sec<br>
       -> std::map : 18.710377 sec<br>

     : Lookup keys from 0 to 19999999<br>
       -> avltree : 7.364360 sec<br>
       -> std::map : 11.209790 sec<br>

     : Delete all elements<br>
       -> avltree : 4.774075 sec<br>
       -> std::map : 14.209195 sec<br>

    * NON-PRIMITIVE (structure, classes, ...)<br>
     : Sequential insert from 0 to 19999999<br>
       -> avltree : 14.797637 sec<br>
       -> std::map : 18.878864 sec<br>

     : Lookup 0 to 19999999<br>
       -> avltree : 7.413985 sec<br>
       -> std::map : 11.157555 sec<br>

     : Delete all element<br>
       -> avltree : 8.253727 sec<br>
       -> std::map : 14.500771 sec<br>

