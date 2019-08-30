#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include <QString>

/**
 * @brief The LockFreeQueue class
 * sizeof(void *)==32正常运行，sizeof(void *)==64更换InterlockedCompareExchange为InterlockedCompareExchange64
 */
template <class T>
class LockFreeQueue
{
public:
    LockFreeQueue(int capacity);
    ~LockFreeQueue();

    double getUsageRage();
    bool check();
    void init();

    //线程安全
    bool enqueue(const T &t);
    //线程安全
    T dequeue(bool *ok = 0);

private:
    T **queue;
    int capacity;
    int tail;
    int head;
};

#include "lockfreequeue.cpp"

#endif // LOCKFREEQUEUE_H
