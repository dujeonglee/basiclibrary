#ifndef _AVLTREE_H_
#define _AVLTREE_H_

/*
 * BALANCED_DELETION (Default = Enabled): Delete a node in consideration of balance factor to minimize number of rotations.
 * DEBUG_FUNCTIONS (Default = Disabled): Support debugging functions
 */

#define BALANCED_DELETION
//#define DEBUG_FUNCTIONS

#include <exception>
#include <functional>
#include <string>
#include <string.h>

template<class KEY>
inline bool less(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))<0;
}
template<class KEY>
inline bool greater(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))>0;
}
template<class KEY>
inline bool equal(const KEY& key1, const KEY& key2){
    return memcmp(&key1, &key2, sizeof(KEY))==0;
}
template<class KEY>
inline void copy(KEY* const key1, const KEY* const key2){
    memcpy(key1, key2, sizeof(KEY));
}

// For primitive type char
template<>
inline bool less<char>(const char& key1, const char& key2){
    return key1 < key2;
}
template<>
inline bool greater<char>(const char& key1, const char& key2){
    return key1 > key2;
}
template<>
inline bool equal<char>(const char& key1, const char& key2){
    return key1 == key2;
}
template<>
inline void copy<char>(char* const key1, const char* const key2){
    *key1 = *key2;
}

// For primitive type unsigned char
template<>
inline bool less<unsigned char>(const unsigned char& key1, const unsigned char& key2){
    return key1 < key2;
}
template<>
inline bool greater<unsigned char>(const unsigned char& key1, const unsigned char& key2){
    return key1 > key2;
}
template<>
inline bool equal<unsigned char>(const unsigned char& key1, const unsigned char& key2){
    return key1 == key2;
}
template<>
inline void copy<unsigned char>(unsigned char* const key1, const unsigned char* const key2){
    *key1 = *key2;
}

// For primitive type signed char
template<>
inline bool less<signed char>(const signed char& key1, const signed char& key2){
    return key1 < key2;
}
template<>
inline bool greater<signed char>(const signed char& key1, const signed char& key2){
    return key1 > key2;
}
template<>
inline bool equal<signed char>(const signed char& key1, const signed char& key2){
    return key1 == key2;
}
template<>
inline void copy<signed char>(signed char* const key1, const signed char* const key2){
    *key1 = *key2;
}

// For primitive type int
template<>
inline bool less<int>(const int& key1, const int& key2){
    return key1 < key2;
}
template<>
inline bool greater<int>(const int& key1, const int& key2){
    return key1 > key2;
}
template<>
inline bool equal<int>(const int& key1, const int& key2){
    return key1 == key2;
}
template<>
inline void copy<int>(int* const key1, const int* const key2){
    *key1 = *key2;
}

// For primitive type unsigned int
template<>
inline bool less<unsigned int>(const unsigned int& key1, const unsigned int& key2){
    return key1 < key2;
}
template<>
inline bool greater<unsigned int>(const unsigned int& key1, const unsigned int& key2){
    return key1 > key2;
}
template<>
inline bool equal<unsigned int>(const unsigned int& key1, const unsigned int& key2){
    return key1 == key2;
}
template<>
inline void copy<unsigned int>(unsigned int* const key1, const unsigned int* const key2){
    *key1 = *key2;
}

// For primitive type short int
template<>
inline bool less<short int>(const short int& key1, const short int& key2){
    return key1 < key2;
}
template<>
inline bool greater<short int>(const short int& key1, const short int& key2){
    return key1 > key2;
}
template<>
inline bool equal<short int>(const short int& key1, const short int& key2){
    return key1 == key2;
}
template<>
inline void copy<short int>(short int* const key1, const short int* const key2){
    *key1 = *key2;
}

// For primitive type unsigned short int
template<>
inline bool less<unsigned short int>(const unsigned short int& key1, const unsigned short int& key2){
    return key1 < key2;
}
template<>
inline bool greater<unsigned short int>(const unsigned short int& key1, const unsigned short int& key2){
    return key1 > key2;
}
template<>
inline bool equal<unsigned short int>(const unsigned short int& key1, const unsigned short int& key2){
    return key1 == key2;
}
template<>
inline void copy<unsigned short int>(unsigned short int* const key1, const unsigned short int* const key2){
    *key1 = *key2;
}

// For primitive type long int
template<>
inline bool less<long int>(const long int& key1, const long int& key2){
    return key1 < key2;
}
template<>
inline bool greater<long int>(const long int& key1, const long int& key2){
    return key1 > key2;
}
template<>
inline bool equal<long int>(const long int& key1, const long int& key2){
    return key1 == key2;
}
template<>
inline void copy<long int>(long int* const key1, const long int* const key2){
    *key1 = *key2;
}

// For primitive type unsigned long int
template<>
inline bool less<unsigned long int>(const unsigned long int& key1, const unsigned long int& key2){
    return key1 < key2;
}
template<>
inline bool greater<unsigned long int>(const unsigned long int& key1, const unsigned long int& key2){
    return key1 > key2;
}
template<>
inline bool equal<unsigned long int>(const unsigned long int& key1, const unsigned long int& key2){
    return key1 == key2;
}
template<>
inline void copy<unsigned long int>(unsigned long int* const key1, const unsigned long int* const key2){
    *key1 = *key2;
}

// For primitive type float
template<>
inline bool less<float>(const float& key1, const float& key2){
    return key1 < key2;
}
template<>
inline bool greater<float>(const float& key1, const float& key2){
    return key1 > key2;
}
template<>
inline bool equal<float>(const float& key1, const float& key2){
    return key1 == key2;
}
template<>
inline void copy<float>(float* const key1, const float* const key2){
    *key1 = *key2;
}

// For primitive type double
template<>
inline bool less<double>(const double& key1, const double& key2){
    return key1 < key2;
}
template<>
inline bool greater<double>(const double& key1, const double& key2){
    return key1 > key2;
}
template<>
inline bool equal<double>(const double& key1, const double& key2){
    return key1 == key2;
}
template<>
inline void copy<double>(double* const key1, const double* const key2){
    *key1 = *key2;
}

// For primitive type long double
template<>
inline bool less<long double>(const long double& key1, const long double& key2){
    return key1 < key2;
}
template<>
inline bool greater<long double>(const long double& key1, const long double& key2){
    return key1 > key2;
}
template<>
inline bool equal<long double>(const long double& key1, const long double& key2){
    return key1 == key2;
}
template<>
inline void copy<long double>(long double* const key1, const long double* const key2){
    *key1 = *key2;
}

// For primitive type wchar_t
template<>
inline bool less<wchar_t>(const wchar_t& key1, const wchar_t& key2){
    return key1 < key2;
}
template<>
inline bool greater<wchar_t>(const wchar_t& key1, const wchar_t& key2){
    return key1 > key2;
}
template<>
inline bool equal<wchar_t>(const wchar_t& key1, const wchar_t& key2){
    return key1 == key2;
}
template<>
inline void copy<wchar_t>(wchar_t* const key1, const wchar_t* const key2){
    *key1 = *key2;
}

// For primitive type std::string
template<>
inline bool less<std::string>(const std::string& key1, const std::string& key2){
    return key1 < key2;
}
template<>
inline bool greater<std::string>(const std::string& key1, const std::string& key2){
    return key1 > key2;
}
template<>
inline bool equal<std::string>(const std::string& key1, const std::string& key2){
    return key1 == key2;
}
template<>
inline void copy<std::string>(std::string* const key1, const std::string* const key2){
    *key1 = *key2;
}

template <class KEY, class DATA> class AVLTreeElement;
template <class KEY, class DATA> class AVLTree;
template <class KEY, class DATA> class AVLTreeElement
{
private:
    DATA m_Data;
    KEY m_Key;
    AVLTreeElement<KEY, DATA>* m_Parent;
    AVLTreeElement<KEY, DATA>* m_Left;
    AVLTreeElement<KEY, DATA>* m_Right;
    AVLTreeElement<KEY, DATA>* m_Prev;
    AVLTreeElement<KEY, DATA>* m_Next;
    signed char m_BalanceFactor;
    AVLTree<KEY, DATA>* const m_Tree;

    AVLTreeElement<KEY, DATA>(AVLTree<KEY, DATA>* t):m_Tree(t)
    {
        m_Parent = nullptr;
        m_Left = nullptr;
        m_Right = nullptr;
        if(m_Tree->m_Head == nullptr)
        {
            m_Tree->m_Head = this;
            m_Tree->m_Tail = this;
        }
        else
        {
            m_Prev = m_Tree->m_Tail;
            m_Prev->m_Next = this;
            m_Next = nullptr;
            m_Tree->m_Tail = this;
        }
        m_BalanceFactor = 0;
        m_Tree->m_Size++;
    }

    ~AVLTreeElement<KEY, DATA>(){
        AVLTreeElement<KEY, DATA>* adjustment_child;
        AVLTreeElement<KEY, DATA>* adjustment_parent;

        if(m_Tree->m_Size == 1)
        {
            m_Tree->m_Head = nullptr;
            m_Tree->m_Tail = nullptr;
        }
        else if(this == m_Tree->m_Head)
        {
            m_Tree->m_Head = m_Next;
            m_Tree->m_Head->m_Prev = nullptr;
        }
        else if(this == m_Tree->m_Tail)
        {
            m_Tree->m_Tail = m_Prev;
            m_Tree->m_Tail->m_Next = nullptr;
        }
        else
        {
            m_Prev->m_Next = m_Next;
            m_Next->m_Prev = m_Prev;
        }
        if(m_Left == nullptr && m_Right == nullptr){
            adjustment_child = this;
            adjustment_parent = m_Parent;
            if(m_Parent){
                (m_Parent->m_Left == this?m_Parent->m_Left:m_Parent->m_Right) = nullptr;
            }else{
                m_Tree->m_Root = nullptr;
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
            if(this == m_Tree->m_Root){
                m_Tree->m_Root = target;
            }
        }
        while(adjustment_parent != nullptr){
            if(adjustment_child){
                adjustment_parent->m_BalanceFactor += (less<KEY>(adjustment_child->m_Key, adjustment_parent->m_Key)?-1:1);
            }
            if( adjustment_parent->m_BalanceFactor == 1 || adjustment_parent->m_BalanceFactor == -1 ){
                break;
            }
            if(adjustment_parent->m_BalanceFactor == 2 || adjustment_parent->m_BalanceFactor == -2 ){
                AVLTreeElement<KEY, DATA>* const grand_parent = adjustment_parent;
                if(adjustment_parent->m_BalanceFactor == 2){
                    AVLTreeElement<KEY, DATA>* const parent = grand_parent->m_Left;
                    if(parent->m_BalanceFactor == 1 || parent->m_BalanceFactor == 0){
                        m_Tree->RightRotation(grand_parent, parent);
                        if(parent->m_BalanceFactor == 1){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;

                            adjustment_parent = parent->m_Parent;
                            adjustment_child = parent;
                        }else{ // parent->m_BalanceFactor == 0
                            grand_parent->m_BalanceFactor = 1;
                            parent->m_BalanceFactor = -1;
                            break;
                        }
                    }else{ // adjustment_parent->m_Left && adjustment_parent->m_Left->m_Right
                        AVLTreeElement<KEY, DATA>* const child = parent->m_Right;

                        m_Tree->LeftRotation(parent, child);
                        m_Tree->RightRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = -1;
                                parent->m_BalanceFactor = 0;
                                child->m_BalanceFactor = 0;
                            }else{ // child->m_BalanceFactor == -1
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = 1;
                                child->m_BalanceFactor = 0;
                            }

                        }

                        adjustment_parent = child->m_Parent;
                        adjustment_child = child;
                    }
                }else{ // adjustment_parent->m_BalanceFactor == -2
                    AVLTreeElement<KEY, DATA>* const parent = grand_parent->m_Right;
                    if(parent->m_BalanceFactor == -1|| parent->m_BalanceFactor == 0){
                        m_Tree->LeftRotation(grand_parent, parent);
                        if(parent->m_BalanceFactor == -1){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;

                            adjustment_parent = parent->m_Parent;
                            adjustment_child = parent;
                        }else{ // parent->m_BalanceFactor == 0
                            grand_parent->m_BalanceFactor = -1;
                            parent->m_BalanceFactor = 1;
                            break;
                        }
                    }else{ // adjustment_parent->m_Right && adjustment_parent->m_Right->m_Left
                        AVLTreeElement<KEY, DATA>* const child = parent->m_Left;

                        m_Tree->RightRotation(parent, child);
                        m_Tree->LeftRotation(grand_parent, child);
                        if(child->m_BalanceFactor == 0){
                            grand_parent->m_BalanceFactor = 0;
                            parent->m_BalanceFactor = 0;
                        }else{
                            if(child->m_BalanceFactor == 1){
                                grand_parent->m_BalanceFactor = 0;
                                parent->m_BalanceFactor = -1;
                                child->m_BalanceFactor = 0;
                            }else{ // child->m_BalanceFactor == -1
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
        m_Tree->m_Size--;
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
    AVLTreeElement<KEY, DATA>* m_Head;
    AVLTreeElement<KEY, DATA>* m_Tail;
    AVLTreeElement<KEY, DATA>* m_Iter;
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
        m_Head = nullptr;
        m_Tail = nullptr;
        m_Iter = nullptr;
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
                m_Root = new AVLTreeElement<KEY, DATA>(this);
            }catch(const std::bad_alloc& ex){
                m_Root = nullptr;
                m_Head = nullptr;
                m_Tail = nullptr;
                return false;
            }
            copy<KEY>(&m_Root->m_Key, &key);
            m_Root->m_Data = data;
        }else{
            AVLTreeElement<KEY, DATA>* grand_parent = nullptr;
            AVLTreeElement<KEY, DATA>* parent = m_Root;
            AVLTreeElement<KEY, DATA>* child = nullptr;
            while(
                  (
                      (less<KEY>(key, parent->m_Key) && parent->m_Left == nullptr)
                      ||
                      (greater<KEY>(key, parent->m_Key) && parent->m_Right == nullptr)
                      ||
                      (equal<KEY>(key, parent->m_Key))
                      )
                  ==
                  false
                  ){
                parent = (less<KEY>(key, parent->m_Key)?parent->m_Left:parent->m_Right);
            }
            if(equal<KEY>(key, parent->m_Key)){
                return false;
            }else{
                try{
                    child = (less<KEY>(key, parent->m_Key)?parent->m_Left:parent->m_Right) = new AVLTreeElement<KEY, DATA>(this);
                }catch(const std::bad_alloc& ex){
                    child = (less<KEY>(key, parent->m_Key)?parent->m_Left:parent->m_Right) = nullptr;
                    return false;
                }
                copy<KEY>(&child->m_Key, &key);
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
                            }else{ // child->m_BalanceFactor == -1
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
                            }else{ // child->m_BalanceFactor == -1
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

    bool Remove(const KEY& key, std::function <void (DATA&)> func = nullptr){
        if(m_Root == nullptr){
            return false;
        }
        AVLTreeElement<KEY, DATA>* target;
        target = m_Root;
        while(!equal<KEY>(key, target->m_Key)){
            target = (less<KEY>(key, target->m_Key)?target->m_Left:target->m_Right);
            if(target == nullptr){
                return false;
            }
        }
        if(func)
        {
            func(target->m_Data);
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
        while(!equal<KEY>(key, target->m_Key)){
            target = (less<KEY>(key, target->m_Key)?target->m_Left:target->m_Right);
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
        while(!equal<KEY>(key, target->m_Key)){
            target = (less<KEY>(key, target->m_Key)?target->m_Left:target->m_Right);
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
        if(m_Root){
            (*depth) = _calculate_max_depth_andm_BalanceFactor(m_Root, valid);
        }
    }

private:
    unsigned int _calculate_max_depth_andm_BalanceFactor(avltreeelement<KEY, DATA>* node, bool* result){
        unsigned int left = 0;
        unsigned int right = 0;
        if(node->m_Left){
            left = _calculate_max_depth_andm_BalanceFactor(node->m_Left, result);
        }
        if(node->m_Right){
            right = _calculate_max_depth_andm_BalanceFactor(node->m_Right, result);
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

    void InitIteration()
    {
        m_Iter = m_Head;
    }
    
    bool GetNextElement(DATA& data)
    {
        if(m_Iter == nullptr)
        {
            return false;
        }
        else
        {
            data = m_Iter->m_Data;
            m_Iter = m_Iter->m_Next;
            return true;
        }
    }

    DATA* GetNextElement()
    {
        if(m_Iter == nullptr)
        {
            return nullptr;
        }
        else
        {
            DATA* const ret = &m_Iter->m_Data;
            m_Iter = m_Iter->m_Next;
            return ret;
        }
    }
    friend class AVLTreeElement<KEY, DATA>;
};

#endif
