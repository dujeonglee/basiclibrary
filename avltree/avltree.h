#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#define BALANCED_DELETION

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
        avltreeelement<KEY, DATA>* adjustment_child;
        avltreeelement<KEY, DATA>* adjustment_parent;

        if(_left == NULL && _right == NULL){
            adjustment_child = this;
            adjustment_parent = _parent;
            if(_parent){
                (_parent->_left == this?_parent->_left:_parent->_right) = NULL;
            }else{
                _tree->_root = NULL;
            }
        }else{
            avltreeelement<KEY, DATA>* target;
#ifdef BALANCED_DELETION
            if(_balance_factor > 0){
#else
            if(1){
#endif
                target = (_left?_left:_right);
                while((_left?target->_right:target->_left) != NULL){
                    target = (_left?target->_right:target->_left);
                }
            }else{
                target = (_right?_right:_left);
                while((_right?target->_left:target->_right) != NULL){
                    target = (_right?target->_left:target->_right);
                }
            }
            target->_balance_factor = _balance_factor;

            if(target->_parent == this){
                adjustment_parent = target;
                if(adjustment_parent->_left || adjustment_parent->_right){
                    adjustment_child = (adjustment_parent->_left?adjustment_parent->_left:adjustment_parent->_right);
                }else{
                    adjustment_child = NULL;
                    adjustment_parent->_balance_factor = _balance_factor + (adjustment_parent==_left?-1:1);
                }
                if(_parent){
                    (_parent->_left == this?_parent->_left:_parent->_right) = target;
                }
                target->_parent = _parent;

#ifdef BALANCED_DELETION
                if(_balance_factor > 0){
#else
                if(1){
#endif
                    if((_left?_right:_left)){
                        (_left?_right:_left)->_parent = target;
                    }
                    (_left?target->_right:target->_left) = (_left?_right:_left);
                }else{
                    if((_right?_left:_right)){
                        (_right?_left:_right)->_parent = target;
                    }
                    (_right?target->_left:target->_right) = (_right?_left:_right);
                }
            }else{
                adjustment_child = target;
                adjustment_parent = target->_parent;
#ifdef BALANCED_DELETION
                if(_balance_factor > 0){
#else
                if(1){
#endif
                    (_left?target->_parent->_right:target->_parent->_left) = (_left?target->_left:target->_right);
                    if((_left?target->_parent->_right:target->_parent->_left)){
                        (_left?target->_parent->_right:target->_parent->_left)->_parent = target->_parent;
                    }
                }else{
                    (_right?target->_parent->_left:target->_parent->_right) = (_right?target->_right:target->_left);
                    if((_right?target->_parent->_left:target->_parent->_right)){
                        (_right?target->_parent->_left:target->_parent->_right)->_parent = target->_parent;
                    }
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
            if(this == _tree->_root){
                _tree->_root = target;
            }
        }
        while(adjustment_parent != NULL){
            if(adjustment_child){
                adjustment_parent->_balance_factor += (adjustment_child->_key < adjustment_parent->_key?-1:1);
            }
            if( adjustment_parent->_balance_factor == 1 || adjustment_parent->_balance_factor == -1 ){
                break;
            }
            if(adjustment_parent->_balance_factor == 2 || adjustment_parent->_balance_factor == -2 ){
                avltreeelement<KEY, DATA>* const grand_parent = adjustment_parent;
                if(adjustment_parent->_balance_factor == 2){
                    avltreeelement<KEY, DATA>* const parent = grand_parent->_left;
                    if(parent->_balance_factor == 1 || parent->_balance_factor == 0){
                        _tree->_right_rotation(grand_parent, parent);
                        if(parent->_balance_factor == 1){
                            grand_parent->_balance_factor = 0;
                            parent->_balance_factor = 0;

                            adjustment_parent = parent->_parent;
                            adjustment_child = parent;
                        }else{ // parent->_balance_factor == 0
                            grand_parent->_balance_factor = 1;
                            parent->_balance_factor = -1;
                            break;
                        }
                    }else{ // adjustment_parent->_left && adjustment_parent->_left->_right
                        avltreeelement<KEY, DATA>* const child = parent->_right;

                        _tree->_left_rotation(parent, child);
                        _tree->_right_rotation(grand_parent, child);
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

                        adjustment_parent = child->_parent;
                        adjustment_child = child;
                    }
                }else{ // adjustment_parent->_balance_factor == -2
                    avltreeelement<KEY, DATA>* const parent = grand_parent->_right;
                    if(parent->_balance_factor == -1|| parent->_balance_factor == 0){
                        _tree->_left_rotation(grand_parent, parent);
                        if(parent->_balance_factor == -1){
                            grand_parent->_balance_factor = 0;
                            parent->_balance_factor = 0;

                            adjustment_parent = parent->_parent;
                            adjustment_child = parent;
                        }else{ // parent->_balance_factor == 0
                            grand_parent->_balance_factor = -1;
                            parent->_balance_factor = 1;
                            break;
                        }
                    }else{ // adjustment_parent->_right && adjustment_parent->_right->_left
                        avltreeelement<KEY, DATA>* const child = parent->_left;

                        _tree->_right_rotation(parent, child);
                        _tree->_left_rotation(grand_parent, child);
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
                        adjustment_parent = child->_parent;
                        adjustment_child = child;
                    }
                }
            }else{
                adjustment_child = adjustment_parent;
                adjustment_parent = adjustment_parent->_parent;
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

    friend class avltreeelement<KEY, DATA>;
};

#endif
