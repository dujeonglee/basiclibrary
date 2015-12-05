#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

template <class T> class linkedlistelement;
template <class T> class linkedlist;

template <class T> class linkedlistelement{
public:
private:
    T _data;
    linkedlistelement<T>* _prev;
    linkedlistelement<T>* _next;
    linkedlist<T>* _list;

    linkedlistelement<T>(linkedlist<T>* h){
        _prev = NULL;
        _next = NULL;
        _list = h;
        _list->_size++;
    }

    ~linkedlistelement<T>(){
        if(_next == NULL){
            if(_prev == NULL){
                _list->_head = NULL;
                _list->_tail = NULL;
            }else{
                _prev->_next = NULL;
                _list->_tail = _prev;
            }
        }else{
            if(_prev == NULL){
                _next->_prev = NULL;
                _list->_head = _next;
            }else{
                _prev->_next = _next;
                _next->_prev = _prev;
            }
        }
        _list->_size--;
    }
public:
    linkedlistelement<T> &operator = (const linkedlistelement<T> &other) {
        if (this != &other) {
            _data = other._data;
        }
        return *this;
    }

    friend class linkedlist<T>;
};

template <class T> class linkedlist {
private:
    linkedlistelement<T>* _head;
    linkedlistelement<T>* _tail;
    unsigned int _size;
public:
    linkedlist<T>(){
        _head = NULL;
        _tail = NULL;
        _size = 0;
    }
    ~linkedlist<T>(){
        if(_size > 0){
            clear();
        }
    }

    bool push_front(const T data){
        linkedlistelement<T>* const new_element = new linkedlistelement<T>(this);
        if(new_element == NULL){
            return false;
        }
        if(_head == NULL){
            new_element->_data = data;
            _head = _tail = new_element;
        }else{
            _head->_prev = new_element;
            new_element->_data = data;
            new_element->_next = _head;
            _head = new_element;
        }
        return true;
    }

    bool push_back(const T data){
        linkedlistelement<T>* const new_element = new linkedlistelement<T>(this);
        if(new_element == NULL){
            return false;
        }
        if(_tail == NULL){
            new_element->_data = data;
            _head = _tail = new_element;
        }else{
            _tail->_next = new_element;
            new_element->_data = data;
            new_element->_prev = _tail;
            _tail = new_element;
        }
        return true;
    }

    bool push_at(const int pos, const T data){
        if(pos == 0){
            return push_front(data);
        }else if((unsigned int)pos < _size){
            linkedlistelement<T>* new_element = NULL;
            linkedlistelement<T>* element_position = NULL;
            int i = 0;

            if(_size - (unsigned int)pos > _size/2){
                for(i = 0, element_position = _head ; i < pos-1 ; i++){
                    element_position = element_position->_next;
                }
            }else{
                for(i = 0, element_position = _tail ; i < _size - pos ; i++){
                    element_position = element_position->_prev;
                }
            }
            new_element = new linkedlistelement<T>(this);
            if(new_element == NULL){
                return false;
            }
            new_element->_data = data;
            new_element->_prev = element_position;
            new_element->_next = element_position->_next;
            new_element->_prev->_next = new_element;
            new_element->_next->_prev = new_element;
            return true;
        }else if((unsigned int)pos == _size){
            return push_back(data);
        }
        return false;
    }

    bool pop_front(T* ret){
        if(_head == NULL){
            return false;
        }
        if(ret){
            (*ret) = _head->_data;
        }
        delete _head;
        return true;
    }

    bool pop_back(T* ret){
        if(_tail == NULL){
            return false;
        }
        if(ret){
            (*ret) = _tail->_data;
        }
        delete _tail;
        return true;
    }

    bool pop_at(int pos, T* ret){
        if(pos == 0){
            return pop_front(ret);
        }else if((unsigned int)pos < _size - 1){
            linkedlistelement<T>* element_position = NULL;
            int i = 0;

            if(_size - (unsigned int)pos > _size/2){
                for(i = 0, element_position = _head ; i < pos ; i++){
                    element_position = element_position->_next;
                }
            }else{
                for(i = 0, element_position = _tail ; i < (_size-1)-pos ; i++){
                    element_position = element_position->_prev;
                }
            }
            if(ret){
                (*ret) = element_position->_data;
            }
            delete element_position;
            return true;
        }else if((unsigned int)pos == _size - 1){
            return pop_back(ret);
        }
        return false;
    }

    T* pick_front(){
        return (_head?&_head->_data:NULL);
    }

    T* pick_back(){
        return (_tail?&_tail->_data:NULL);
    }

    T* pick_at(const unsigned int position){
        if(position == 0){
            return pick_front();
        }else if(position < _size - 1){
            linkedlistelement<T>* element_position = NULL;
            int i = 0;

            if(_size - position > _size/2){
                for(i = 0, element_position = _head ; i < position ; i++){
                    element_position = element_position->_next;
                }
            }else{
                for(i = 0, element_position = _tail ; i < (_size-1)-position ; i++){
                    element_position = element_position->_prev;
                }
            }
            return &element_position->_data;
        }else if(position == _size - 1){
            return pick_back();
        }
        return NULL;
    }

    T* operator [](const unsigned int index) {
        return pick_at(index);
    }

    unsigned int size(){
        return _size;
    }

    void clear(){
        while(pop_back(NULL));
    }

    friend class linkedlistelement<T>;
};

#endif
