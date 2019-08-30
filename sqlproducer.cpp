#include "sqlproducer.h"

#include <QThread>
#include <QDateTime>
#include <Windows.h>
#include <QDebug>

SqlProducer::SqlProducer(QSharedPointer<LockFreeQueue<QString>> queue, quint64 startValue, quint64 endValue, bool *controlFlag, QObject *parent) :
    queue(queue), startValue(startValue), endValue(endValue), controlFlag(controlFlag), QObject(parent)
{
    connect(this, SIGNAL(produce(QString,int)), this, SLOT(onProduce(QString,int)));
}

SqlProducer::~SqlProducer()
{
    qDebug()<<"~SqlProducer()";
}

void SqlProducer::emitProduce(QString str, int frequency)
{
    emit produce(str, frequency);
}

void SqlProducer::onProduce()
{
    count = 0;

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    do{
        for(quint64 i=startValue; i<endValue; i++){
            if(!*controlFlag){
                break;
            }
            QString value = QString::number(i);
            while(*controlFlag){
                if(queue->enqueue(value)){
                    count++;
                    break;
                }else{
                    Sleep(1);
                    continue;
                }
            }
        }
    }while(false);


    bool isStopping = !*controlFlag;
    emit produceDone(QThread::currentThread(), count, this, isStopping);
    if(isStopping){
        emit produceDone(QThread::currentThread(), count, this);
    }
}

void SqlProducer::onProduce(QString str, int frequency)
{
    while(*controlFlag && frequency > 0){
        if(queue->enqueue(str)){
            frequency--;
        }else{
            Sleep(1);
            continue;
        }
    }

    emit produceDone(QThread::currentThread(), count, this);
}
