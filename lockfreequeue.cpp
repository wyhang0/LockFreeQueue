#include "lockfreequeue.h"

#include <stdlib.h>

#include "cas.h"

//https://coolshell.cn/articles/8239.html

template <class T>
LockFreeQueue<T>::LockFreeQueue(int capacity)
{
    if(capacity <= 0)
        capacity = 512;
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
double LockFreeQueue<T>::getUsageRage() const
{
    double value = _tail - _head;
    if(value < 0){
        value += _capacity;
    }
    return value/_capacity;
}

#if 0
//先放值再更新tail，先取值再更新head。(取值时线程异常退出可能造成取到的值未处理)
//1.多线程消息乱序入队，多线程消息乱序出队。
//2.单线程消息顺序入队，单线程消息顺序出队。(当"index == _tail"队列为空时直接返回false取数据失败)。
template <class T>
bool LockFreeQueue<T>::enqueue(const T* const t)
{
    int index;
    while(true){
        index = _tail;

        if((index + 1) % _capacity == _head){
            return false;
        }

        if(_queue[index] != NULL){//如果T1线程在用CAS更新tail值的之前，线程停掉或是挂掉了或是长时间睡眠，防止其它线程进入死循环
            CAS(&_tail, index, (index + 1) % _capacity);
            continue;
        }


        /**
          bug--数据会放在head与tail组成的队列之外，导致明明放了数据但是读不出来
          T2线程enqueue成功并且更新了tail值，T3线程dequeue取出T2放入的值并置位置为NULL;
          此时T1的index值为T2更新tail前的值，由于T3把该位置的值为NULL，导致执行“CAS(&_queue[index], NULL, t)”成功后该位置的值只有tail下次更新到该位置时才能被取出；
          解决方法:
          1.执行“CAS(&_queue[index], NULL, t)”之前先判断index和_tail是否相等，不相等则发生上述情况，需要重新读tail入队。
            这只是能减少该情况发生，如果判断和执行之间线程休眠了，还是有机会发生上述情况
          2.终极方案，在dequeue操作时如果判断队列为空，这时从该位置查找队列，如果对列有不为空的值，则重置head的值在重新dequeue，该操作最多花费当队列真为空时遍历队列的时间。
          */
        if(_queue[index] == NULL && index != _tail){
            continue;
        }

//        QThread::usleep(1);

        if(CAS(&_queue[index], NULL, t)){
            break;
        }
    }

    CAS(&_tail, index, (index + 1) % _capacity);
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
//            //单线程消息顺序出队
//            return false;

            //"bug--数据会放在head与tail组成的队列之外，导致明明放了数据但是读不出来"的解决方法2

            //方法3
            int step = 0;
            while(step < _capacity){
                if(_queue[index] != NULL){
                     if(CAS(&_head, oldIndex, index)){
                        break;
                     }else{
                         oldIndex = _head;
                         continue;
                     }
                }
                index = (index + 1) % _capacity;
                step++;
            }
            if(step < _capacity){
                continue;
            }else{
                return false;
            }

//            //方法2
//            int step = 0;
//            while(step < _capacity){
//                if(_queue[step] != NULL){
//                     if(CAS(&_head, oldIndex, step)){
//                        break;
//                     }else{
//                         oldIndex = _head;
//                         continue;
//                     }
//                }
//                step++;
//            }
//            if(step < _capacity){
//                continue;
//            }else{
//                return false;
//            }

//            //方法1
//            int step;
//            for(step = 0; step < _capacity; step++){
//                if(_queue[step] != NULL){
//                     if(CAS(&_head, oldIndex, step)){
//                        break;
//                     }else{
//                         oldIndex = _head;
//                         step = -1; //step++后从0开始
//                     }
//                }
//            }
//            if(step < _capacity){
//                continue;
//            }else{
//                return false;
//            }
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

#elif 1
//先更新tail再放值，先更新head再取值。(取值时线程异常退出可能造成取到的值未处理)
//1.多线程消息顺序入队，多线程消息乱序出队。
//2.多线程消息顺序入队，单线程消息顺序出队。(上次单线程取值之后未更新head值异常退出，再次运行单线程取值前需要更新head值到正确的地方，否则会死循环)

template <class T>
bool LockFreeQueue<T>::enqueue(const T* const t)
{
    int index;
    while(true){
        index = _tail;

        if((index + 1) % _capacity == _head){
            return false;
        }

        if(!CAS(&_tail, index, (index + 1) % _capacity)){
            continue;
        }

        //值还没有取出去，往后面放
        if(!CAS(&_queue[index], NULL, t)){
            continue;
        }

        break;
    }

    return true;
}
#if 0
//单线程消息顺序出队，(上次单线程取值之后未更新head值异常退出，再次运行单线程取值前需要更新head值到正确的地方，否则会死循环)
//int index = _head, step = 0;
//while (step < _capacity && _queue[index] == NULL) {
//    index = (index + 1) % _capacity);
//    step++;
//}
//_head = index;

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

        //1.值还没有放进去
        //2.上次单线程取值之后未更新head值异常退出，再次运行单线程取值前需要更新head值到正确的地方，否则会死循环
        if(*t == NULL){
            continue;
        }

        if(CAS(&_queue[index], *t, NULL)){
            break;
        }
    }

    CAS(&_head, oldIndex, (index + 1) % _capacity);
    return true;
}

#else
//多线程消息乱序出队
template <class T>
bool LockFreeQueue<T>::dequeue(T **t)
{
    int index, oldIndex;
    while (true) {
        index = _head;
        oldIndex = index;

        if(index == _tail){
            //值还没有放进去的，再试着读一遍
            int step = 0;
            while(step < _capacity){
                if(_queue[index] != NULL){
                     if(CAS(&_head, oldIndex, index)){
                        break;
                     }else{
                         oldIndex = _head;
                         continue;
                     }
                }
                index = (index + 1) % _capacity;
                step++;
            }
            if(step < _capacity){
                continue;
            }else{
                return false;
            }
        }

        if(!CAS(&_head, oldIndex, (index + 1) % _capacity)){
            continue;
        }

        *t = _queue[index];
        //值还没有放进去，等下次再读
        if(*t == NULL){
            continue;
        }

        if(!CAS(&_queue[index], *t, NULL)){
            continue;
        }

        break;
    }
    return true;
}
#endif

#endif

template<class T>
void LockFreeQueue<T>::log(void (*func)(T *t, int head, int tail, int index))
{
    for(int i = 0; i < _capacity; i++){
        if(_queue[i] != NULL){
            (*func)(_queue[i], _head, _tail, i);
        }
    }
}
