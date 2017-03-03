#ifndef _AVLTREE_H_
#define _AVLTREE_H_

/*
 * BALANCED_DELETION (Default = Enabled): Delete a node in consideration of balance factor to minimize number of rotations.
 * NONPRIMITIVE_KEY (Default = Disabled): Support a non-primitive type for key value.
 * DEBUG_FUNCTIONS (Default = Disabled): Support debugging functions
 */

#define BALANCED_DELETION
//#define NONPRIMITIVE_KEY
//#define DEBUG_FUNCTIONS

#include <exception>
#include <functional>
#include <string.h>

template <class KEY, class DATA> class AVLTreeElement;
template <class KEY, class DATA> class AVLTree;

#ifdef NONPRIMITIVE_KEY
template<class KEY>
bool less(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))<0;
}

template<class KEY>
bool greater(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))>0;
}

template<class KEY>
bool equal(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))==0;
}

template<class KEY>
void copy(KEY* const key1, const KEY* const key2){
    memcpy(key1, key2, sizeof(KEY));
}
#endif

template <class KEY, class DATA> class AVLTreeElement
{
private:
    DATA m_Data;
    KEY m_Key;
    AVLTreeElement<KEY, DATA>* m_Parent;
    AVLTreeElement<KEY, DATA>* m_Left;
    AVLTreeElement<KEY, DATA>* m_Right;
    char m_BalanceFactor;
    AVLTree<KEY, DATA> m_Tree;

    AVLTreeElement<KEY, DATA>(AVLTree<KEY, DATA>& t){
        m_Parent = nullptr;
        m_Left = nullptr;
        m_Right = nullptr;
        m_BalanceFactor = 0;
        m_Tree = t;
        m_Tree.m_Size++;
    }

    ~AVLTreeElement<KEY, DATA>(){
        AVLTreeElement<KEY, DATA>* adjustment_child;
        AVLTreeElement<KEY, DATA>* adjustment_parent;

        if(m_Left == nullptr && m_Right == nullptr){
            adjustment_child = this;
            adjustment_parent = m_Parent;
            if(m_Parent){
                (m_Parent->m_Left == this?m_Parent->m_Left:m_Parent->m_Right) = nullptr;
            }else{
                m_Tree.m_Root = nullptr;
            }
        }else{
            AVLTreeElement<KEY, DATA>* target;
#ifdef BALANCED_DELETION
            if(m_BalanceFactor > 0){
#else
            if(1){
#endif
                target = (m_Left?m_Left:m_Right);
                while((m_Left?target->m_Right:target->m_Left) != nullptr){
                    target = (m_Left?target->m_Right:target->m_Left);
                }
            }else{
                target = (m_Right?m_Right:m_Left);
                while((m_Right?target->m_Left:target->m_Right) != nullptr){
                    target = (m_Right?target->m_Left:target->m_Right);
                }
            }
            target->m_BalanceFactor = m_BalanceFactor;

            if(target->m_Parent == this){
                adjustment_parent = target;
                if(adjustment_parent->m_Left || adjustment_parent->m_Right){
                    adjustment_child = (adjustment_parent->m_Left?adjustment_parent->m_Left:adjustment_parent->m_Right);
                }else{
                    adjustment_child = nullptr;
                    adjustment_parent->m_BalanceFactor = m_BalanceFactor + (adjustment_parent==m_Left?-1:1);
                }
                if(m_Parent){
                    (m_Parent->m_Left == this?m_Parent->m_Left:m_Parent->m_Right) = target;
                }
                target->m_Parent = m_Parent;

#ifdef BALANCED_DELETION
                if(m_BalanceFactor > 0){
#else
                if(1){
#endif
                    if((m_Left?m_Right:m_Left)){
                        (m_Left?m_Right:m_Left)->m_Parent = target;
                    }
                    (m_Left?target->m_Right:target->m_Left) = (m_Left?m_Right:m_Left);
                }else{
                    if((m_Right?m_Left:m_Right)){
                        (m_Right?m_Left:m_Right)->m_Parent = target;
                    }
                    (m_Right?target->m_Left:target->m_Right) = (m_Right?m_Left:m_Right);
                }
            }else{
                adjustment_child = target;
                adjustment_parent = target->m_Parent;
#ifdef BALANCED_DELETION
                if(m_BalanceFactor > 0){
#else
                if(1){
#endif
                    (m_Left?target->m_Parent->m_Right:target->m_Parent->m_Left) = (m_Left?target->m_Left:target->m_Right);
                    if((m_Left?target->m_Parent->m_Right:target->m_Parent->m_Left)){
                        (m_Left?target->m_Parent->m_Right:target->m_Parent->m_Left)->m_Parent = target->m_Parent;
                    }
                }else{
                    (m_Right?target->m_Parent->m_Left:target->m_Parent->m_Right) = (m_Right?target->m_Right:target->m_Left);
                    if((m_Right?target->m_Parent->m_Left:target->m_Parent->m_Right)){
                        (m_Right?target->m_Parent->m_Left:target->m_Parent->m_Right)->m_Parent = target->m_Parent;
                    }
                }
                target->m_Parent = m_Parent;
                if(target->m_Parent){
                    (target->m_Parent->m_Left == this?target->m_Parent->m_Left:target->m_Parent->m_Right) = target;
                }
                target->m_Left = m_Left;
                if(target->m_Left){
                    target->m_Left->m_Parent = target;
                }
                target->m_Right = m_Right;
                if(target->m_Right){
                    target->m_Right->m_Parent = target;
                }
            }
            if(this == m_Tree.m_Root){
                m_Tree.m_Root = target;
            }
        }
        while(adjustment_parent != nullptr){
            if(adjustment_child){
#ifdef NONPRIMITIVE_KEY
                adjustment_parent->_balance_factor += (less<KEY>(adjustment_child->_key, adjustment_parent->_key)?-1:1);
#else
                adjustment_parent->m_BalanceFactor += (adjustment_child->m_Key < adjustment_parent->m_Key?-1:1);
#endif
            }
            if( adjustment_parent->m_BalanceFactor == 1 || adjustment_parent->m_BalanceFactor == -1 ){
                break;
            }
            if(adjustment_parent->m_BalanceFactor == 2 || adjustment_parent->m_BalanceFactor == -2 ){
                AVLTreeElement<KEY, DATA>* const grand_parent = adjustment_parent;
                if(adjustment_parent->m_BalanceFactor == 2){
                    AVLTreeElement<KEY, DATA>* const parent = grand_parent->m_Left;
                    if(parent->m_BalanceFactor == 1 || parent->m_BalanceFactor == 0){
                        m_Tree.RightRotation(grand_parent, parent);
                        if(parent->m_BalanceFactor == 1){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;

                            adjustment_parent = parent->m_Parent;
                            adjustment_child = parent;
                        }else{ // parent->_balance_factor == 0
                            grand_parent->m_BalanceFactor = 1;
                            parent->m_BalanceFactor = -1;
                            break;
                        }
                    }else{ // adjustment_parent->_left && adjustment_parent->_left->_right
                        AVLTreeElement<KEY, DATA>* const child = parent->m_Right;

                        m_Tree.LeftRotation(parent, child);
                        m_Tree.RightRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = -1;
                                parent->m_BalanceFactor = 0;
                                child->m_BalanceFactor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = 1;
                                child->m_BalanceFactor = 0;
                            }

                        }

                        adjustment_parent = child->m_Parent;
                        adjustment_child = child;
                    }
                }else{ // adjustment_parent->_balance_factor == -2
                    AVLTreeElement<KEY, DATA>* const parent = grand_parent->m_Right;
                    if(parent->m_BalanceFactor == -1|| parent->m_BalanceFactor == 0){
                        m_Tree.LeftRotation(grand_parent, parent);
                        if(parent->m_BalanceFactor == -1){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;

                            adjustment_parent = parent->m_Parent;
                            adjustment_child = parent;
                        }else{ // parent->_balance_factor == 0
                            grand_parent->m_BalanceFactor = -1;
                            parent->m_BalanceFactor = 1;
                            break;
                        }
                    }else{ // adjustment_parent->_right && adjustment_parent->_right->_left
                        AVLTreeElement<KEY, DATA>* const child = parent->m_Left;

                        m_Tree.RightRotation(parent, child);
                        m_Tree.LeftRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = -1;
                                child->m_BalanceFactor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->m_BalanceFactor = 1;
                                parent->m_BalanceFactor = 0;
                                child->m_BalanceFactor = 0;
                            }
                        }
                        adjustment_parent = child->m_Parent;
                        adjustment_child = child;
                    }
                }
            }else{
                adjustment_child = adjustment_parent;
                adjustment_parent = adjustment_parent->m_Parent;
            }
        }
        m_Tree.m_Size--;
    }
public:
    AVLTreeElement<KEY, DATA> &operator = (const AVLTreeElement<KEY, DATA> &other) {
        if (this != &other) {
            m_Data = other.m_Data;
        }
        return *this;
    }

    friend class AVLTree<KEY, DATA>;
};

template <class KEY, class DATA> class AVLTree {
public:
    enum TraversalMode
    {
        PREORDER,
        INORDER,
        POSTORDER
    };

private:
    AVLTreeElement<KEY, DATA>* m_Root;
    unsigned int m_Size;

    void LeftRotation(AVLTreeElement<KEY, DATA>* const parent, AVLTreeElement<KEY, DATA>* const child){
        if(parent->m_Right != child){
            return;
        }

        child->m_Parent = parent->m_Parent;
        if(child->m_Parent){
            (child->m_Parent->m_Left == parent?child->m_Parent->m_Left:child->m_Parent->m_Right) = child;
        }else{
            m_Root = child;
        }

        parent->m_Right = child->m_Left;
        if(parent->m_Right){
            parent->m_Right->m_Parent = parent;
        }

        child->m_Left = parent;
        parent->m_Parent = child;
    }

    void RightRotation(AVLTreeElement<KEY, DATA>* parent, AVLTreeElement<KEY, DATA>* child){
        if(parent->m_Left != child){
            return;
        }

        child->m_Parent = parent->m_Parent;
        if(child->m_Parent){
            (child->m_Parent->m_Left == parent?child->m_Parent->m_Left:child->m_Parent->m_Right) = child;
        }else{
            m_Root = child;
        }

        parent->m_Left = child->m_Right;
        if(parent->m_Left){
            parent->m_Left->m_Parent = parent;
        }

        child->m_Right = parent;
        parent->m_Parent = child;
    }

public:
    AVLTree<KEY, DATA>(){
        m_Root = nullptr;
        m_Size = 0;
    }
    ~AVLTree<KEY, DATA>(){
        if(m_Size > 0){
            Clear();
        }
    }

    bool Insert(const KEY& key, const DATA& data){
        if(m_Root == nullptr){
            try{
            m_Root = new AVLTreeElement<KEY, DATA>(*this);
            }catch(const std::bad_alloc& ex){
                m_Root = nullptr;
                return false;
            }

#ifdef NONPRIMITIVE_KEY
            copy<KEY>(&_root->_key, &key);
#else
            m_Root->m_Key = key;
#endif
            m_Root->m_Data = data;
        }else{
            AVLTreeElement<KEY, DATA>* grand_parent = nullptr;
            AVLTreeElement<KEY, DATA>* parent = m_Root;
            AVLTreeElement<KEY, DATA>* child = nullptr;
            while(
                  (
          #ifdef NONPRIMITIVE_KEY
                      (less<KEY>(key, parent->_key) && parent->_left == nullptr)
          #else
                      (key < parent->m_Key && parent->m_Left == nullptr)
          #endif
                      ||
          #ifdef NONPRIMITIVE_KEY
                      (greater<KEY>(key, parent->_key) && parent->_right == nullptr)
          #else
                      (key > parent->m_Key && parent->m_Right == nullptr)
          #endif
                      ||
          #ifdef NONPRIMITIVE_KEY
                      (equal<KEY>(key, parent->_key))
          #else
                      key == parent->m_Key
          #endif
                      )
                  ==
                  false
                  ){
#ifdef NONPRIMITIVE_KEY
                parent = (less<KEY>(key, parent->_key)?parent->_left:parent->_right);
#else
                parent = (key < parent->m_Key?parent->m_Left:parent->m_Right);
#endif
            }
#ifdef NONPRIMITIVE_KEY
            if(equal<KEY>(key, parent->_key)){
#else
            if(key == parent->m_Key){
#endif
                return false;
            }else{
                try{
#ifdef NONPRIMITIVE_KEY
                child = (less<KEY>(key, parent->_key)?parent->_left:parent->_right) = new avltreeelement<KEY, DATA>(*this);
#else
                child = (key < parent->m_Key?parent->m_Left:parent->m_Right) = new AVLTreeElement<KEY, DATA>(*this);
#endif
                }catch(const std::bad_alloc& ex){
#ifdef NONPRIMITIVE_KEY
                      child = (less<KEY>(key, parent->_key)?parent->_left:parent->_right) = nullptr;
#else
                      child = (key < parent->m_Key?parent->m_Left:parent->m_Right) = nullptr;
#endif
                      return false;
                }
#ifdef NONPRIMITIVE_KEY
                copy<KEY>(&child->_key, &key);
#else
                child->m_Key = key;
#endif
                child->m_Data = data;
                child->m_Parent = parent;
                while(parent != nullptr){
                    parent->m_BalanceFactor += (parent->m_Left == child?1:-1);
                    if(parent->m_BalanceFactor == 2 || parent->m_BalanceFactor == -2 || parent->m_BalanceFactor == 0){
                        break;
                    }
                    child = parent;
                    parent = parent->m_Parent;
                }
                if(parent && parent->m_BalanceFactor != 0){
                    grand_parent = parent;
                    parent = (grand_parent->m_BalanceFactor == 2?grand_parent->m_Left:grand_parent->m_Right);
                    child = (parent->m_BalanceFactor == 1?parent->m_Left:parent->m_Right);
                    if(grand_parent->m_Left == parent && parent->m_Right == child){
                        LeftRotation(parent, child);
                        RightRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = -1;
                                parent->m_BalanceFactor = 0;
                                child->m_BalanceFactor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = 1;
                                child->m_BalanceFactor = 0;
                            }
                        }
                    }
                    if(grand_parent->m_Left == parent && parent->m_Left == child){
                        RightRotation(grand_parent, parent);
                        grand_parent->m_BalanceFactor = 0;
                        parent->m_BalanceFactor = 0;
                    }
                    if(grand_parent->m_Right == parent && parent->m_Left == child){
                        RightRotation(parent, child);
                        LeftRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = -1;
                                child->m_BalanceFactor = 0;
                            }else{ // child->_balance_factor == -1
                                grand_parent->m_BalanceFactor = 1;
                                parent->m_BalanceFactor = 0;
                                child->m_BalanceFactor = 0;
                            }
                        }
                    }
                    if(grand_parent->m_Right == parent && parent->m_Right == child){
                        LeftRotation(grand_parent, parent);
                        grand_parent->m_BalanceFactor = 0;
                        parent->m_BalanceFactor = 0;
                    }
                }
            }
        }
        return true;
    }

    bool Remove(const KEY& key){
        if(m_Root == nullptr){
            return false;
        }
        AVLTreeElement<KEY, DATA>* target;
        target = m_Root;
#ifdef NONPRIMITIVE_KEY
        while(!equal<KEY>(key, target->_key)){
#else
        while(key != target->m_Key){
#endif
#ifdef NONPRIMITIVE_KEY
            target = (less<KEY>(key, target->_key)?target->_left:target->_right);
#else
            target = (key < target->m_Key?target->m_Left:target->m_Right);
#endif
            if(target == nullptr){
                return false;
            }
        }
        delete target;
        return true;
    }

    DATA& GetRef(const KEY& key){
        if(m_Root == nullptr){
            throw std::string("No such elements");
        }
        AVLTreeElement<KEY, DATA>* target;
        target = m_Root;
#ifdef NONPRIMITIVE_KEY
        while(!equal<KEY>(key, target->_key)){
#else
        while(key != target->m_Key){
#endif
#ifdef NONPRIMITIVE_KEY
            target = (less<KEY>(key, target->_key)?target->_left:target->_right);
#else
            target = (key < target->m_Key?target->m_Left:target->m_Right);
#endif
            if(target == nullptr){
                throw std::string("No such elements");
            }
        }
        return target->m_Data;
    }

    DATA* GetPtr(const KEY& key){
        if(m_Root == nullptr){
            return nullptr;
        }
        AVLTreeElement<KEY, DATA>* target;
        target = m_Root;
#ifdef NONPRIMITIVE_KEY
        while(!equal<KEY>(key, target->_key)){
#else
        while(key != target->m_Key){
#endif
#ifdef NONPRIMITIVE_KEY
            target = (less<KEY>(key, target->_key)?target->_left:target->_right);
#else
            target = (key < target->m_Key?target->m_Left:target->m_Right);
#endif
            if(target == nullptr){
                return nullptr;
            }
        }
        return (&target->m_Data);
    }

    unsigned int Size(){
        return m_Size;
    }

    void Clear(){
        while(m_Size){
            delete m_Root;
        }
    }

#ifdef DEBUG_FUNCTIONS
public:
    void check_max_depth_and_validity(unsigned int* depth, bool* valid){
        (*depth) = 0;
        (*valid) = true;
        if(_root){
            (*depth) = _calculate_max_depth_and_balance_factor(_root, valid);
        }
    }

private:
    unsigned int _calculate_max_depth_and_balance_factor(avltreeelement<KEY, DATA>* node, bool* result){
        unsigned int left = 0;
        unsigned int right = 0;
        if(node->_left){
            left = _calculate_max_depth_and_balance_factor(node->_left, result);
        }
        if(node->_right){
            right = _calculate_max_depth_and_balance_factor(node->_right, result);
        }
        (*result) = (*result) && (left>right?left-right < 2:right-left < 2);
        return 1+(left>right?left:right);
    }
#endif
private:
    void Traversal(AVLTreeElement<KEY, DATA>* node, std::function <void (DATA&)> func, TraversalMode mode){
        if(mode==PREORDER)
        {
            func(node->m_Data);
        }
        if(node->m_Left){
            Traversal(node->m_Left, func, mode);
        }
        if(mode==INORDER)
        {
            func(node->m_Data);
        }
        if(node->m_Right){
            Traversal(node->m_Right, func, mode);
        }
        if(mode==POSTORDER)
        {
            func(node->m_Data);
        }
    }

public:
    void DoSomethingOnAllData(std::function <void (DATA&)> func, TraversalMode mode = INORDER){
        if(m_Root){
            Traversal(m_Root, func, mode);
        }
    }
    friend class AVLTreeElement<KEY, DATA>;
};

#endif
