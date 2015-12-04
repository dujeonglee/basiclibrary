#ifndef _BSTREE_H_
#define _BSTREE_H_

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
        if(_left == NULL && _right == NULL){
            if(_parent){
                (_parent->_left == this?_parent->_left:_parent->_right) = NULL;
            }else{
                _tree->_root = NULL;
            }
        }else{
            bstreeelement<KEY, DATA>* target;
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
                    (_left?_right:_left)->_parent = target;
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
            if(this == _tree->_root){
                _tree->_root = target;
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
            bstreeelement<KEY, DATA>* new_child = NULL;
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
            }else{
                new_child = (key < parent->_key?parent->_left:parent->_right) = new bstreeelement<KEY, DATA>(this);
                if(new_child == NULL){
                    return false;
                }
                new_child->_key = key;
                new_child->_data = data;
                new_child->_parent = parent;
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

    friend class bstreeelement<KEY, DATA>;
};

#endif
