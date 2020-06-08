#include "sqlconsumer.h"

#include <QDateTime>
#include <QThread>
#include <QDebug>

#include "cas.h"

SqlConsumer::SqlConsumer(QSharedPointer<LOCKFREEQUEUE<int>> queue, bool *controlFlag, quint64 *tValue, QObject *parent) :
    QObject(parent), queue(queue), controlFlag(controlFlag), tValue(tValue)
{

}

SqlConsumer::~SqlConsumer()
{
    qDebug()<<"~SqlConsumer()";
}

void SqlConsumer::consume()
{
    int *value;

    while(true){
        if(!*controlFlag){
            break;
        }
        if(!queue->dequeue(&value)){
            if(*tValue == 0)
                break;
            QThread::msleep(1);
            continue;
        }
//        qDebug()<<*value;
        ATOMIC_SUB(tValue, *value);

        delete value;
    }

    *controlFlag = false;
    QThread::currentThread()->quit();
}
