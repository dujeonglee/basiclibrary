#ifndef _BSTREE_H_
#define _BSTREE_H_
#include <vector>

template <class KEY, class DATA> class bstreeelement;
template <class KEY, class DATA> class bstree;

template <class KEY, class DATA> class bstreeelement{
private:
    DATA _data;
    KEY _key;
    bstreeelement<KEY, DATA>* _parent;
    bstreeelement<KEY, DATA>* _left;
    bstreeelement<KEY, DATA>* _right;
    bstree<KEY, DATA>* _tree;

    bstreeelement<KEY, DATA>(bstree<KEY, DATA>* t){
        _parent = NULL;
        _left = NULL;
        _right = NULL;
        _tree = t;
        _tree->_size++;
    }

    ~bstreeelement<KEY, DATA>(){
        if(_left){
            bstreeelement<KEY, DATA>* largest_in_left;
            largest_in_left = _left;
            while(largest_in_left->_right != NULL){
                largest_in_left = largest_in_left->_right;
            }
            if(largest_in_left == _left){
                if(_parent){
                    (_parent->_left==this?_parent->_left:_parent->_right) = largest_in_left;
                }
                largest_in_left->_parent = _parent;

                if(_right){
                    _right->_parent = largest_in_left;
                }
                largest_in_left->_right = _right;
            }else{
                largest_in_left->_parent->_right = largest_in_left->_left;
                if(largest_in_left->_parent->_right){
                    largest_in_left->_parent->_right->_parent = largest_in_left->_parent;
                }

                largest_in_left->_parent = _parent;
                if(largest_in_left->_parent){
                    (largest_in_left->_parent->_left == this?largest_in_left->_parent->_left:largest_in_left->_parent->_right) = largest_in_left;
                }
                largest_in_left->_left = _left;
                if(largest_in_left->_left){
                    largest_in_left->_left->_parent = largest_in_left;
                }
                largest_in_left->_right = _right;
                if(largest_in_left->_right){
                    largest_in_left->_right->_parent = largest_in_left;
                }
            }
            if(this == _tree->_root){
                _tree->_root = largest_in_left;
            }
        }else if(_right){
            bstreeelement<KEY, DATA>* smallest_in_right;
            smallest_in_right = _right;
            while(smallest_in_right->_left != NULL){
                smallest_in_right = smallest_in_right->_left;
            }
            if(smallest_in_right == _right){
                if(_parent){
                    (_parent->_left==this?_parent->_left:_parent->_right) = smallest_in_right;
                }
                smallest_in_right->_parent = _parent;

                if(_left){
                    _left->_parent = smallest_in_right;
                }
                smallest_in_right->_left = _left;
            }else{
                smallest_in_right->_parent->_left = smallest_in_right->_right;
                if(smallest_in_right->_parent->_left){
                    smallest_in_right->_parent->_left->_parent = smallest_in_right->_parent;
                }

                smallest_in_right->_parent = _parent;
                if(smallest_in_right->_parent){
                    (smallest_in_right->_parent->_left == this?smallest_in_right->_parent->_left:smallest_in_right->_parent->_right) = smallest_in_right;
                }
                smallest_in_right->_left = _left;
                if(smallest_in_right->_left){
                    smallest_in_right->_left->_parent = smallest_in_right;
                }
                smallest_in_right->_right = _right;
                if(smallest_in_right->_right){
                    smallest_in_right->_right->_parent = smallest_in_right;
                }
            }
            if(this == _tree->_root){
                _tree->_root = smallest_in_right;
            }
        }else{
            if(_parent){
                (_parent->_left == this?_parent->_left:_parent->_right) = NULL;
            }else{
                _tree->_root = NULL;
            }
        }
        _tree->_size--;
    }
public:
    bstreeelement<KEY, DATA> &operator = (const bstreeelement<KEY, DATA> &other) {
        if (this != &other) {
            _data = other._data;
        }
        return *this;
    }

    friend class bstree<KEY, DATA>;
};

template <class KEY, class DATA> class bstree {
private:
    bstreeelement<KEY, DATA>* _root;
    unsigned int _size;
    void _inorder(bstreeelement<KEY, DATA>* node){
        if(node->_left){
            _inorder(node->_left);
        }
        printf("Key %d Data %d Parent %d LeftCh %d RightCh %d\n",
               node->_key,
               node->_data,
               (node->_parent==NULL?-1:node->_parent->_key),
               (node->_left==NULL?-1:node->_left->_key),
               (node->_right==NULL?-1:node->_right->_key));
        if(node->_right){
            _inorder(node->_right);
        }
    }

public:
    bstree<KEY, DATA>(){
        _root = NULL;
        _size = 0;
    }
    ~bstree<KEY, DATA>(){
        if(_size > 0){
            clear();
        }
    }

    bool insert(const KEY key, const DATA data){
        if(_root == NULL){
            _root = new bstreeelement<KEY, DATA>(this);
            if(_root == NULL){
                return false;
            }
            _root->_key = key;
            _root->_data = data;
        }else{
            bstreeelement<KEY, DATA>* parent;
            parent = _root;
            while(
                    (
                      (key < parent->_key && parent->_left == NULL)
                      ||
                      (key > parent->_key && parent->_right == NULL)
                      ||
                      key == parent->_key
                    )
                    ==
                    false
                 ){
                parent = (key < parent->_key?parent->_left:parent->_right);
            }
            if(key == parent->_key){
                return false;
            }else if(key < parent->_key){
                parent->_left = new bstreeelement<KEY, DATA>(this);
                if(parent->_left == NULL){
                    return false;
                }
                parent->_left->_key = key;
                parent->_left->_data = data;
                parent->_left->_parent = parent;
            }else{
                parent->_right = new bstreeelement<KEY, DATA>(this);
                if(parent->_right == NULL){
                    return false;
                }
                parent->_right->_key = key;
                parent->_right->_data = data;
                parent->_right->_parent = parent;
            }
        }
        return true;
    }

    bool remove(const KEY key){
        if(_root == NULL){
            return false;
        }
        bstreeelement<KEY, DATA>* target;
        target = _root;
        while(key != target->_key){
            target = (key < target->_key?target->_left:target->_right);
            if(target == NULL){
                return false;
            }
        }
        delete target;
        return true;
    }

    DATA* find(const KEY key){
        if(_root == NULL){
            return NULL;
        }
        bstreeelement<KEY, DATA>* target;
        target = _root;
        while(key != target->_key){
            target = (key < target->_key?target->_left:target->_right);
            if(target == NULL){
                return NULL;
            }
        }
        return (&target->_data);
    }

    unsigned int size(){
        return _size;
    }

    void clear(){
        while(_size){
            delete _root;
        }
    }

    void print(){
        if(_root){
            _inorder(_root);
        }
    }

    friend class bstreeelement<KEY, DATA>;
};

#endif
