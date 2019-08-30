#include "sqlconsumer.h"
#include <QDateTime>
#include <QThread>
#include <Windows.h>
#include <QDebug>

SqlConsumer::SqlConsumer(QSharedPointer<LockFreeQueue<QString>> queue, bool *controlFlag, QObject *parent) :
    queue(queue), controlFlag(controlFlag), QObject(parent)
{

}

SqlConsumer::~SqlConsumer()
{
    qDebug()<<"~SqlConsumer()";
}

void SqlConsumer::consume()
{
    count=0;
    QString value;
    quint64 sum = 0;

    bool ok;
    while(true){
        if(!*controlFlag){
            break;
        }
        value = queue->dequeue(&ok);
        if(value.compare("END") == 0){
            break;
        }
        if(!ok){
            Sleep(1);
            continue;
        }

        sum += value.toULongLong();
        count++;
    }

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    emit consumeDone(QThread::currentThread(), count, sum, this);
}
