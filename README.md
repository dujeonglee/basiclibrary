# basic data structures and libraries
Implementation of basic data structures and libraries for own study.

## Header only data structures
  - Linked List: Queue and Stack functions are provided.
  - Binary Search Tree: Insert, find, remove functions are provided.
  - AVL Tree: Insert, find, remove functions are provided.
## Libraries
  - Single-shot timer: C++11 based timer library. g++ version should be >= g++-4.9 (note: g++ compilers <=g++-4.8 has a bugs for try_lock_for.).
  - ThreadPool: C++ based thread pool library.

#ifndef HEAP_H
#define HEAP_H
#include <iostream>
#include <functional>
#include <utility>
#include <exception>

template <class K, class V>
class heap
{
    template <class KEY, class VALUE>
    class element
    {
        private:
            KEY _key;
            VALUE _value;
        public:
            element() = delete;
            element(KEY key, VALUE value) : _key(key), _value(value){}
            element(KEY&& key, VALUE&& value) : _key(std::move(key)), _value(std::move(value)){}

            const KEY key(){return _key;}
            VALUE& value(){return _value;}
    };

private:
    element<K, V>*** _heap;
    size_t _max_heights;
    size_t _current_height;
    size_t _elements;
    std::function<bool(K, K)> _cmp;

public:
    heap() = delete;
    heap(std::function<bool(K, K)> cmp_func, size_t max_height = 5)
    {
        bool allocation_success = true;
        _max_heights = max_height;
        _current_height = 0;
        _elements = 0;
        _cmp = cmp_func;

        try
        {
            _heap = new element<K, V>**[_max_heights]();
        }
        catch (std::exception ex)
        {
            _heap = nullptr;
            allocation_success = false;
        }
        if(!allocation_success)
        {
            return;
        }
        for(size_t ht = 0 ; ht < _max_heights ; ht++)
        {
            try
            {
                _heap[ht] = new element<K, V>*[ht+1]();
            }
            catch (std::exception ex)
            {
                _heap[ht] = nullptr;
                allocation_success = false;
            }
        }
        if(!allocation_success)
        {
            for(size_t ht = 0 ; ht < _max_heights ; ht++)
            {
                if(_heap[ht] != nullptr)
                {
                    delete [] _heap[ht];
                }
            }
            delete [] _heap;
        }
    }

    bool push(K k, V v)
    {
        // Check if current level is full
        const size_t HT_1 = 0x1;
        if( (HT_1 << (_current_height + 1)) - 1 < _elements + 1)
        {
            _current_height++;
        }
        if(_current_height >= _max_heights)
        {
            _current_height--;
            return false;
        }

        const size_t HEAP_HT = _current_height;
        const size_t HEAP_IDX = _elements - ((HT_1<<HEAP_HT)-1);
        try
        {
            _heap[HEAP_HT][HEAP_IDX] = new element<K, V>(k, v);
            _elements++;

            size_t current_ht = HEAP_HT;
            size_t current_idx = HEAP_IDX;
            size_t parent_ht = current_ht - 1;
            size_t parent_idx = current_idx>>1;
            while(current_ht > 0)
            {
                if(_cmp(_heap[current_ht][current_idx]->key(), _heap[parent_ht][parent_idx]->key()) == true)
                {
                    break;
                }
                else
                {
                    element<K, V>* tmp = _heap[current_ht][current_idx];
                    _heap[current_ht][current_idx] = _heap[parent_ht][parent_idx];
                    _heap[parent_ht][parent_idx] = tmp;
                    
                    current_ht = parent_ht;
                    current_idx = parent_idx;
                    parent_ht = current_ht - 1;
                    parent_idx = current_idx>>1;
                }
            }
            return true;
        }
        catch (std::exception ex)
        {
            _heap[HEAP_HT][HEAP_IDX] = nullptr;
            return false;
        }
    }

    V& pick()
    {
        return _heap[0][0]->value();
    }

    void pop()
    {
        if(_elements > 0)
        {
            _elements--;
            if(_elements == 0)
            {
                delete _heap[0][0];
                _heap[0][0] = nullptr;
                _current_height = 0;
                return;
            }
            else
            {
                const size_t last_ht = _current_height;
                const size_t last_idx = _elements - ((0x1<<last_ht)-1);
                _heap[0][0] = _heap[last_ht][last_idx];
                delete _heap[last_ht][last_idx];
                _heap[last_ht][last_idx] = nullptr;
                if(last_idx == 0)
                {
                    _current_height--;
                }

                size_t current_ht = 0;
                size_t current_idx = 0;
                size_t children_ht = current_ht+1;
                size_t left_idx = current_idx<<1;
                size_t right_ht = (current_idx<<1)+1;

                while(current_ht < _current_height)
                {
                    if((_cmp(_heap[current_ht][current_idx]->key(), _heap[children_ht][left_idx]->key()) == false) &&
                       (_cmp(_heap[current_ht][current_idx]->key(), _heap[children_ht][right_idx]->key()) == false) )
                    {
                        break;
                    }
                    else
                    {
                        element<K, V>* tmp = _heap[current_ht][current_idx];
                        _heap[current_ht][current_idx] = _heap[parent_ht][parent_idx];
                        _heap[parent_ht][parent_idx] = tmp;
                        
                        current_ht = parent_ht;
                        current_idx = parent_idx;
                        parent_ht = current_ht - 1;
                        parent_idx = current_idx>>1;
                    }
                }
            }
        }
    }
};

#endif
