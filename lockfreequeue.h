#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

template <class T>
class LockFreeQueue
{
public:
    LockFreeQueue(int capacity = 512);
    ~LockFreeQueue();

    double getUsageRage() const;

    //多线程安全
    bool enqueue(const T* const t);
    //多线程安全
    bool dequeue(T **t);

    //参数为函数指针或者类的静态成员函数
    void log(void (*func)(T *t));

private:
    T **_queue;
    int _capacity;
    volatile int _tail;
    volatile int _head;
};

#include "lockfreequeue.cpp"

#endif // LOCKFREEQUEUE_H
