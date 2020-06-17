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
    QList<quint64> allValues;

    int repeatTimes = 0;

    while(true){
        if(!*controlFlag){
            break;
        }
        if(!queue->dequeue(&value)){
            if(*tValue == 0){
                break;
            }

            repeatTimes++;

            if(repeatTimes == 100)
                break;

            QThread::msleep(1);
            continue;
        }

        repeatTimes = 0;

        allValues<<*value;

        ATOMIC_SUB(tValue, *value);

        delete value;
    }

    quint64 preValue, currentValue, rangeStart, rangeEnd;
    bool firstEnter;
    for(int i=0; i<4; i++){//4为界面上生产者线程数
        firstEnter = false;
        preValue = currentValue = allValues[0];
        rangeStart = i*1000000;//1000000为界面上每个生产者产品的个数
        rangeEnd = (i+1)*1000000;
        for(int j=0; j<allValues.count(); j++){
            currentValue = allValues[j];
            if(currentValue>=rangeStart && currentValue<rangeEnd){
                if(!firstEnter){
                    firstEnter = true;
                    preValue = currentValue;
                    continue;
                }
                if(preValue > currentValue){
                    qDebug()<<QThread::currentThreadId()<<"start range: "<<rangeStart<<"end range: "<<rangeEnd<<"preValue: "<<preValue<<"currentValue: "<<currentValue;
                }
                preValue = currentValue;
            }
        }
    }

    QThread::currentThread()->quit();
}
