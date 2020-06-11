#include "lockfreelinkqueue.h"

#include <stdlib.h>

#include "cas.h"

//https://coolshell.cn/articles/8239.html
//一对一可以做到无锁，一对多、多对一、多对多有问题

template <class T>
Lockfreelinkqueue<T>::Lockfreelinkqueue(int)
{
    _head = _tail = new Node;
    _head->_t = NULL;
    _head->_next = NULL;
}

template<class T>
Lockfreelinkqueue<T>::~Lockfreelinkqueue()
{
    Node *tmp;
    while (_head)
    {
      tmp = _head->_next;
      delete _head->_t;
      delete _head;
      _head = tmp;
    }
}

template<class T>
double Lockfreelinkqueue<T>::getUsageRage()
{
    return 1;
}

template<class T>
bool Lockfreelinkqueue<T>::enqueue(const T* const t)
{
    //有bug
    return false;


    Node *node = new Node;
    node->_t = NULL;
    node->_next = NULL;

    Node *tail, *tailShadow;

    tailShadow = tail = const_cast<Node*>(_tail);
    do{
        //--bug--多线程取时，T1线程拿到的tail可能被其他dequeue线程释放掉，导致T1线程异常退出
        while(tail->_next != NULL){
            tail = tail->_next;
        }

    }while(!CAS(&(tail->_next), NULL, node));

    //把值放入节点里
    while(!CAS(&(tailShadow->_t), NULL, t)){
        tailShadow = tailShadow->_next;
    }

    //置尾结点
    CAS(&_tail, tail, node);

    return true;
}

template<class T>
bool Lockfreelinkqueue<T>::dequeue(T **t)
{
    //有bug
    return false;


    Node *head, *tail, *next;

    while(true) {
        //取出头指针，尾指针，和第一个元素的指针
        head = const_cast<Node*>(_head);
        tail = const_cast<Node*>(_tail);

        //--bug--多线程取时，T1线程拿到的head可能被其他dequeue线程释放掉，导致T1线程异常退出
        next = head->_next;
        *t = head->_t;

        // 如果是空队列
        if (head == tail && next == NULL) {
            return false;
        }

        //如果 tail 指针落后了
        if (head == tail && next != NULL) {
            continue;
        }

        //节点入队列了 1.正在赋t值 2.enqueue异常退出，没有机会赋t值
        if(*t == NULL){
            continue;
        }

        //移动 head 指针成功后，取出数据
        if (CAS(&_head, head, next)){
            break;
        }
    }

    delete head; //释放老的dummy结点

    return true;
}

template<class T>
void Lockfreelinkqueue<T>::log(void (*func)(T *))
{

}
