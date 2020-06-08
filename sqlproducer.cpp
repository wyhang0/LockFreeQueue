#include "sqlproducer.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>

#include "cas.h"

SqlProducer::SqlProducer(QSharedPointer<LOCKFREEQUEUE<int>> queue, quint64 startValue, quint64 endValue, bool *controlFlag, quint64 *tValue, QObject *parent) :
    QObject(parent), queue(queue), startValue(startValue), endValue(endValue), controlFlag(controlFlag), tValue(tValue)
{

}

SqlProducer::~SqlProducer()
{
    qDebug()<<"~SqlProducer()";
}

void SqlProducer::onProduce()
{
    do{
        for(quint64 i=startValue; i<endValue; i++){
            int *value = new int(i);

            while(*controlFlag){
                if(queue->enqueue(value)){
                    ATOMIC_SUB(tValue, i);
                    break;
                }else{
                    QThread::msleep(1);
                    continue;
                }
            }
        }
    }while(false);

    QThread::currentThread()->quit();
}
