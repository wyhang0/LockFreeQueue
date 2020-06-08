#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

template <class T>
class LockFreeQueue
{
public:
    LockFreeQueue(int capacity);
    ~LockFreeQueue();

    double getUsageRage();

    //线程安全
    bool enqueue(const T* const t);
    //线程安全
    bool dequeue(T **t);

private:
    T **_queue;
    int _capacity;
    volatile int _tail;
    volatile int _head;
};

#include "lockfreequeue.cpp"

#endif // LOCKFREEQUEUE_H
