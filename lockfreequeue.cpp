#include "lockfreequeue.h"

#include <exception>
#include <Windows.h>
#include <QDebug>

//https://coolshell.cn/articles/8239.html

template <class T>
LockFreeQueue<T>::LockFreeQueue(int capacity)
{
    this->capacity = capacity;
    head = tail = 0;
    if(capacity > 0){
        try{
            queue = new QString*[capacity];
            for(int i = 0; i < capacity; i++){
                queue[i] = NULL;
            }
        }catch(const std::bad_alloc &){
            queue = NULL;
        }
    }else{
        queue = NULL;
    }
}

template <class T>
LockFreeQueue<T>::~LockFreeQueue()
{
    if(queue != NULL){
        for(int i = 0; i < capacity; i++){
            if(queue[i] != NULL)
                delete queue[i];
        }
        delete[] queue;
    }
}

template<class T>
double LockFreeQueue<T>::getUsageRage()
{
    double value = tail - head;
    if(value < 0){
        value += capacity;
    }
    return value/capacity;
}

template <class T>
bool LockFreeQueue<T>::check()
{
    return queue != NULL;
}

template<class T>
void LockFreeQueue<T>::init()
{
    if(queue != NULL){
        for(int i = 0; i < capacity; i++){
            if(queue[i] != NULL){
                delete queue[i];
                queue[i] = NULL;
            }
        }
        head = tail = 0;
    }
}

template <class T>
bool LockFreeQueue<T>::enqueue(const T &t)
{
    T *value = new T(t);
    int i = 0;//如果T1线程在用CAS更新tail值的之前，线程停掉或是挂掉了或是长时间睡眠，防止其它线程进入死循环
    while(true){
        int index = tail;
        int oldIndex = index;

        if(i > 3){//如果T1线程在用CAS更新tail值的之前，线程停掉或是挂掉了或是长时间睡眠，防止其它线程进入死循环
//            qDebug()<<"enqueue";
            while (queue[index] != NULL) {
                index = (index + 1) % capacity;
            }
            i = 0;
        }

        if((index + 1) % capacity == head){
            InterlockedCompareExchange((LONG*)&tail, index, oldIndex);
            return false;
        }

        if(InterlockedCompareExchange((LONG*)&queue[index], (LONG)value, NULL) == NULL){
            InterlockedCompareExchange((LONG*)&tail, (index + 1) % capacity, oldIndex);
            return true;
        }else{
            i++;
        }
    }
}

template <class T>
T LockFreeQueue<T>::dequeue(bool *ok)
{
    int i = 0;//如果T1线程在用CAS更新head值的之前，线程停掉或是挂掉了或者长时间睡眠，防止其它线程进入死循环
    while (true) {
        int index = head;
        int oldIndex = index;

        if(i > 3){//如果T1线程在用CAS更新head值的之前，线程停掉或是挂掉了或者长时间睡眠，防止其它线程进入死循环
//            qDebug()<<"dequeue";
            while(queue[index] == NULL){
                index = (index + 1) % capacity;
            }
            i= 0;
        }

        if(index == tail){
            InterlockedCompareExchange((LONG*)&head, index, oldIndex);

            if(ok != 0){
                *ok = false;
            }

            return T();
        }

        T *value = queue[index];
        if(value == NULL){
            i++;
            continue;
        }
        if(InterlockedCompareExchange((LONG*)&queue[index], NULL, (LONG)value) == (LONG)value){
            InterlockedCompareExchange((LONG*)&head, (index + 1) % capacity, oldIndex);

            if(ok != 0){
                *ok = true;
            }

            T t(*value);
            delete value;
            return t;
        }else{
            i++;
        }
    }
}
