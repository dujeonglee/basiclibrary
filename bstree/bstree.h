#ifndef _BSTREE_H_
#define _BSTREE_H_

/*
 * NONPRIMITIVE_KEY: Support a non-primitive type for key value.
 */

#ifdef NONPRIMITIVE_KEY
#include <string.h>
#endif

template <class KEY, class DATA> class bstreeelement;
template <class KEY, class DATA> class bstree;

#ifdef NONPRIMITIVE_KEY
template<class KEY>
bool less(const KEY key1, const KEY key2){
    return memcmp(&key1, &key2, sizeof(KEY))<0;
}

template<class KEY>
bool greater(const KEY key1, const KEY key2){
    return memcmp(&key1, &key2, sizeof(KEY))>0;
}

template<class KEY>
bool equal(const KEY key1, const KEY key2){
    return memcmp(&key1, &key2, sizeof(KEY))==0;
}

template<class KEY>
void copy(KEY* const key1, const KEY* const key2){
    memcpy(key1, key2, sizeof(KEY));
}
#endif

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
#ifdef NONPRIMITIVE_KEY
            copy<KEY>(&_root->_key, &key);
#else
            _root->_key = key;
#endif
            _root->_data = data;
        }else{
            bstreeelement<KEY, DATA>* parent;
            bstreeelement<KEY, DATA>* new_child = NULL;
            parent = _root;
            while(
                  (
#ifdef NONPRIMITIVE_KEY
                      (less<KEY>(key, parent->_key) && parent->_left == NULL)
#else
                      (key < parent->_key && parent->_left == NULL)
#endif
                      ||
#ifdef NONPRIMITIVE_KEY
                      (greater<KEY>(key, parent->_key)  && parent->_right == NULL)
#else
                      (key > parent->_key && parent->_right == NULL)
#endif
                      ||
#ifdef NONPRIMITIVE_KEY
                      equal<KEY>(key, parent->_key)
#else
                      key == parent->_key
#endif
                      )
                  ==
                  false
                  ){
#ifdef NONPRIMITIVE_KEY
                parent = (less<KEY>(key, parent->_key)?parent->_left:parent->_right);
#else
                parent = (key < parent->_key?parent->_left:parent->_right);
#endif
            }
#ifdef NONPRIMITIVE_KEY
            if(equal<KEY>(key, parent->_key)){
#else
            if(key == parent->_key){
#endif
                return false;
            }else{
#ifdef NONPRIMITIVE_KEY
                new_child = (less<KEY>(key, parent->_key)?parent->_left:parent->_right) = new bstreeelement<KEY, DATA>(this);
#else
                new_child = (key < parent->_key?parent->_left:parent->_right) = new bstreeelement<KEY, DATA>(this);
#endif
                if(new_child == NULL){
                    return false;
                }
#ifdef NONPRIMITIVE_KEY
                copy<KEY>(&new_child->_key, &key);
#else
                new_child->_key = key;
#endif
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
#ifdef NONPRIMITIVE_KEY
        while(!equal<KEY>(key, target->_key)){
#else
        while(key != target->_key){
#endif
#ifdef NONPRIMITIVE_KEY
            target = (less<KEY>(key, target->_key)?target->_left:target->_right);
#else
            target = (key < target->_key?target->_left:target->_right);
#endif
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
#ifdef NONPRIMITIVE_KEY
        while(!equal<KEY>(key, target->_key)){
#else
        while(key != target->_key){
#endif
#ifdef NONPRIMITIVE_KEY
            target = (less<KEY>(key, target->_key)?target->_left:target->_right);
#else
            target = (key < target->_key?target->_left:target->_right);
#endif
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
        while(_root){
            delete _root;
        }
    }

    friend class bstreeelement<KEY, DATA>;
};

#endif
