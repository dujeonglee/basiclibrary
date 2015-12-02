#ifndef _AVLTREE_H_
#define _AVLTREE_H_

template <class KEY, class DATA> class avltreeelement;
template <class KEY, class DATA> class avltree;

template <class KEY, class DATA> class avltreeelement{
private:
    DATA _data;
    KEY _key;
    avltreeelement<KEY, DATA>* _parent;
    avltreeelement<KEY, DATA>* _left;
    avltreeelement<KEY, DATA>* _right;
    char _balance_factor;
    avltree<KEY, DATA>* _tree;

    avltreeelement<KEY, DATA>(avltree<KEY, DATA>* t){
        _parent = NULL;
        _left = NULL;
        _right = NULL;
        _balance_factor = 0;
        _tree = t;
        _tree->_size++;
    }

    ~avltreeelement<KEY, DATA>(){
        if(_left == NULL && _right == NULL){
            if(_parent){
                (_parent->_left == this?_parent->_left:_parent->_right) = NULL;
            }else{
                _tree->_root = NULL;
            }
        }else{
            avltreeelement<KEY, DATA>* target;
            target = (_left?_left:_right);
            while((_left?target->_right:target->_left) != NULL){
                target = (_left?target->_right:target->_left);
            }

            if(target->_parent == this){
                if(_parent){
                    (_parent->_left == this?_parent->_left:_parent->_right) = target;
                }
                target->_parent = _parent;

                if((_left?_right:_left)){
                    (_left?_right:_right)->_parent = target;
                }
                (_left?target->_right:target->_left) = (_left?_right:_left);
            }else{
                (_left?target->_parent->_right:target->_parent->_left) = (_left?target->_left:target->_right);
                if((_left?target->_parent->_right:target->_parent->_left)){
                    (_left?target->_parent->_right:target->_parent->_left)->_parent = target->_parent;
                }

                target->_parent = _parent;
                if(target->_parent){
                    (target->_parent->_left == this?target->_parent->_left:target->_parent->_right) = target;
                }
                target->_left = _left;
                if(target->_left){
                    target->_left->_parent = target;
                }
                target->_right = _right;
                if(target->_right){
                    target->_right->_parent = target;
                }
            }
            /* TODO
            Rearrange hieght balance
            */
            if(this == _tree->_root){
                _tree->_root = target;
            }
        }
        _tree->_size--;
    }
public:
    avltreeelement<KEY, DATA> &operator = (const avltreeelement<KEY, DATA> &other) {
        if (this != &other) {
            _data = other._data;
        }
        return *this;
    }

    friend class avltree<KEY, DATA>;
};

template <class KEY, class DATA> class avltree {
private:
    avltreeelement<KEY, DATA>* _root;
    unsigned int _size;

    void _inorder(avltreeelement<KEY, DATA>* node){
        if(node->_left){
            _inorder(node->_left);
        }
        printf("[%d] parent %d left %d right %d bf %hhd\n", node->_key, (node->_parent?node->_parent->_key:-1), (node->_left?node->_left->_key:-1), (node->_right?node->_right->_key:-1), node->_balance_factor);
        if(node->_right){
            _inorder(node->_right);
        }
    }

    void _left_rotation(avltreeelement<KEY, DATA>* const parent, avltreeelement<KEY, DATA>* const child){
        if(parent->_right != child){
            return;
        }

        child->_parent = parent->_parent;
        if(child->_parent){
            (child->_parent->_left == parent?child->_parent->_left:child->_parent->_right) = child;
        }else{
            _root = child;
        }

        parent->_right = child->_left;
        if(parent->_right){
            parent->_right->_parent = parent;
        }

        child->_left = parent;
        parent->_parent = child;

    }

    void _right_rotation(avltreeelement<KEY, DATA>* parent, avltreeelement<KEY, DATA>* child){
        if(parent->_left != child){
            return;
        }

        child->_parent = parent->_parent;
        if(child->_parent){
            (child->_parent->_left == parent?child->_parent->_left:child->_parent->_right) = child;
        }else{
            _root = child;
        }

        parent->_left = child->_right;
        if(parent->_left){
            parent->_left->_parent = parent;
        }

        child->_right = parent;
        parent->_parent = child;

    }

public:
    avltree<KEY, DATA>(){
        _root = NULL;
        _size = 0;
    }
    ~avltree<KEY, DATA>(){
        if(_size > 0){
            clear();
        }
    }

    bool insert(const KEY key, const DATA data){
        if(_root == NULL){
            _root = new avltreeelement<KEY, DATA>(this);
            if(_root == NULL){
                return false;
            }
            _root->_key = key;
            _root->_data = data;
        }else{
            avltreeelement<KEY, DATA>* grand_parent = NULL;
            avltreeelement<KEY, DATA>* parent = _root;
            avltreeelement<KEY, DATA>* child = NULL;
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
            }else{
                child = (key < parent->_key?parent->_left:parent->_right) = new avltreeelement<KEY, DATA>(this);
                if(child == NULL){
                    return false;
                }
                child->_key = key;
                child->_data = data;
                child->_parent = parent;
                while(parent != NULL){
                    parent->_balance_factor += (parent->_left == child?1:-1);
                    if(parent->_balance_factor == 2 || parent->_balance_factor == -2 || parent->_balance_factor == 0){
                        break;
                    }
                    child = parent;
                    parent = parent->_parent;
                }
                if(parent && parent->_balance_factor != 0){
                    grand_parent = parent;
                    parent = (grand_parent->_balance_factor == 2?grand_parent->_left:grand_parent->_right);
                    child = (parent->_balance_factor == 1?parent->_left:parent->_right);
                    if(grand_parent->_left == parent && parent->_right == child){
                        _left_rotation(parent, child);
                        _right_rotation(grand_parent, child);
                        if(child->_balance_factor == 0){
                            grand_parent->_balance_factor = 0;
                            parent->_balance_factor = 0;
                        }else{
                            if(child->_balance_factor == 1){
                                grand_parent->_balance_factor = -1;
                                parent->_balance_factor = 0;
                                child->_balance_factor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->_balance_factor = 0;
                                parent->_balance_factor = 1;
                                child->_balance_factor = 0;
                            }
                        }
                    }
                    if(grand_parent->_left == parent && parent->_left == child){
                        _right_rotation(grand_parent, parent);
                        grand_parent->_balance_factor = 0;
                        parent->_balance_factor = 0;
                    }
                    if(grand_parent->_right == parent && parent->_left == child){
                        _right_rotation(parent, child);
                        _left_rotation(grand_parent, child);
                        if(child->_balance_factor == 0){
                            grand_parent->_balance_factor = 0;
                            parent->_balance_factor = 0;
                        }else{
                            if(child->_balance_factor == 1){
                                grand_parent->_balance_factor = 0;
                                parent->_balance_factor = -1;
                                child->_balance_factor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->_balance_factor = 1;
                                parent->_balance_factor = 0;
                                child->_balance_factor = 0;
                            }
                        }
                    }
                    if(grand_parent->_right == parent && parent->_right == child){
                        _left_rotation(grand_parent, parent);
                        grand_parent->_balance_factor = 0;
                        parent->_balance_factor = 0;
                    }
                }
            }
        }
        return true;
    }

    bool remove(const KEY key){
        if(_root == NULL){
            return false;
        }
        avltreeelement<KEY, DATA>* target;
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
        avltreeelement<KEY, DATA>* target;
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
        if(_root)
            _inorder(_root);
    }

    friend class avltreeelement<KEY, DATA>;
};

#endif
