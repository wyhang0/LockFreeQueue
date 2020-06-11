#include "sqlconsumer.h"

#include <QDateTime>
#include <QThread>
#include <QDebug>

#include "cas.h"

SqlConsumer::SqlConsumer(QSharedPointer<LOCKFREEQUEUE<quint64>> queue, bool *controlFlag, quint64 *tValue, QObject *parent) :
    QObject(parent), queue(queue), controlFlag(controlFlag), tValue(tValue)
{

}

SqlConsumer::~SqlConsumer()
{
//    qDebug()<<"~SqlConsumer()";
}

void SqlConsumer::consume()
{
    quint64 *value;

    while(true){
        if(!*controlFlag){
            break;
        }
        if(!queue->dequeue(&value)){
            if(*tValue == 0){
                break;
            }

            QThread::msleep(1);
            continue;
        }

        ATOMIC_SUB(tValue, *value);

        delete value;
    }

    QThread::currentThread()->quit();
}
