#include "lockfreequeue.h"

#include <stdlib.h>

#include "cas.h"

//https://coolshell.cn/articles/8239.html

template <class T>
LockFreeQueue<T>::LockFreeQueue(int capacity)
{
    if(capacity < 0)
        capacity = 50;
    this->_capacity = capacity;
    _head = _tail = 0;

    _queue = new T*[_capacity];
    for(int i = 0; i < _capacity; i++){
        _queue[i] = NULL;
    }
}

template <class T>
LockFreeQueue<T>::~LockFreeQueue()
{
    if(_queue != NULL){
        for(int i = 0; i < _capacity; i++){
            if(_queue[i] != NULL)
                delete _queue[i];
        }
        delete[] _queue;
    }
}

template<class T>
double LockFreeQueue<T>::getUsageRage()
{
    double value = _tail - _head;
    if(value < 0){
        value += _capacity;
    }
    return value/_capacity;
}

template <class T>
bool LockFreeQueue<T>::enqueue(const T* const t)
{
    int index, oldIndex;
    while(true){
        index = _tail;
        oldIndex = index;

        if((index + 1) % _capacity == _head){
            return false;
        }

        if(_queue[index] != NULL){//如果T1线程在用CAS更新tail值的之前，线程停掉或是挂掉了或是长时间睡眠，防止其它线程进入死循环
            CAS(&_tail, oldIndex, (index + 1) % _capacity);
            continue;
        }

        if(CAS(&_queue[index], NULL, t)){
            break;
        }
    }

    CAS(&_tail, oldIndex, (index + 1) % _capacity);
    return true;
}

template <class T>
bool LockFreeQueue<T>::dequeue(T **t)
{
    int index, oldIndex;
    while (true) {
        index = _head;
        oldIndex = index;

        if(index == _tail){
            return false;
        }

        *t = _queue[index];

        //如果T1线程在用CAS更新head值的之前，线程停掉或是挂掉了或者长时间睡眠，防止其它线程进入死循环
        if(*t == NULL){
            CAS(&_head, oldIndex, (index + 1) % _capacity);
            continue;
        }

        if(CAS(&_queue[index], *t, NULL)){
            break;
        }
    }

    CAS(&_head, oldIndex, (index + 1) % _capacity);
    return true;
}
