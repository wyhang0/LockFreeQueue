#ifndef LOCKFREELINKQUEUE_H
#define LOCKFREELINKQUEUE_H

template <class T>
class Lockfreelinkqueue
{
public:
    Lockfreelinkqueue(int);
    ~Lockfreelinkqueue();

    double getUsageRage();

    bool enqueue(const T* const t);
    bool dequeue(T **t);

    void log(void (*func)(T *t));

protected:
    struct Node
    {
        T *_t;
        Node *_next;
    };

private:
    volatile Node* _head;
    volatile Node* _tail;
};

#include "lockfreelinkqueue.cpp"

#endif // LOCKFREELINKQUEUE_H
