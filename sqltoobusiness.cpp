#include "sqltoobusiness.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>

SqlTooBusiness::SqlTooBusiness(QObject *parent) :
    QObject(parent)
{
    consumerAllConsumeCount = producerAllProduceCount = 0;
    queue = QSharedPointer<LockFreeQueue<QString>>(new LockFreeQueue<QString>(6000));
    running = false;

    thread = new QThread();
    this->moveToThread(thread);
    thread->start();

    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    timer.start(1);
}

SqlTooBusiness::~SqlTooBusiness()
{
    stop();
    thread->quit();
    thread->wait();
    delete thread;
}

bool SqlTooBusiness::start(int consumerThreadCount, int producerThreadCount, quint64 perProducerThreadProduceCount)
{
    if(running){
        return true;
    }

    if(!queue->check()){
        running = false;
        return false;
    }

    qDebug()<<QDateTime::currentDateTime();
    running = true;
    consumerAllConsumeCount = producerAllProduceCount = 0;
    sqlConsumerThreadCount = sqlProducerThreadCount = 0;
    sumValue = 0;

    queue->init();
    for(int i = 0; i<consumerThreadCount; i++){
        QThread *consumer = new QThread();
        SqlConsumer *sqlConsumer = new SqlConsumer(queue, &running);
        sqlConsumer->moveToThread(consumer);
        connect(consumer, SIGNAL(started()), sqlConsumer, SLOT(consume()));
        connect(sqlConsumer, SIGNAL(consumeDone(QThread*,quint64,quint64,SqlConsumer*)), this, SLOT(onConsumeDone(QThread*,quint64,quint64,SqlConsumer*)));
        connect(consumer, SIGNAL(finished()), consumer, SLOT(deleteLater()));
        consumer->start();

        sqlConsumerThreadCount++;
    }

    for(int i = 0; i<producerThreadCount; i++){
        QThread *producer = new QThread();
        SqlProducer *sqlProducer = new SqlProducer(queue, i*perProducerThreadProduceCount, (i+1)*perProducerThreadProduceCount, &running);
        sqlProducer->moveToThread(producer);
        connect(producer, SIGNAL(started()), sqlProducer, SLOT(onProduce()));
        connect(sqlProducer, SIGNAL(produceDone(QThread*,quint64,SqlProducer*,bool)), this, SLOT(onProduceDone(QThread*,quint64,SqlProducer*,bool)));
        connect(sqlProducer, SIGNAL(produceDone(QThread*,quint64,SqlProducer*)), this, SLOT(onProduceDone(QThread*,quint64,SqlProducer*)));
        connect(producer, SIGNAL(finished()), producer, SLOT(deleteLater()));
        producer->start();

        sqlProducerThreadCount++;
    }

    return true;
}

void SqlTooBusiness::stop()
{
    running = false;
}

void SqlTooBusiness::onConsumeDone(QThread *currentThread, quint64 count, quint64 sum, SqlConsumer *sqlConsumer)
{
    sumValue += sum;

    consumerAllConsumeCount += count;
    sqlConsumerThreadCount--;

    delete sqlConsumer;
    currentThread->quit();

    if(sqlConsumerThreadCount == 0){//正常结束
        running = false;
        emit execSqlCount(consumerAllConsumeCount, producerAllProduceCount, sumValue);
        qDebug()<<QDateTime::currentDateTime();
    }
}

void SqlTooBusiness::onProduceDone(QThread* currentThread, quint64 count, SqlProducer *sqlProducer, bool isStopping)
{
    if(!isStopping){
        if(sqlProducerThreadCount == 1){
            sqlProducer->emitProduce("END", sqlConsumerThreadCount);
        }else{
            producerAllProduceCount += count;
            sqlProducerThreadCount--;
            currentThread->quit();
            delete sqlProducer;
        }
    }
}

void SqlTooBusiness::onProduceDone(QThread *currentThread, quint64 count, SqlProducer *sqlProducer)
{
    producerAllProduceCount += count;
    sqlProducerThreadCount--;
    currentThread->quit();
    delete sqlProducer;
    if(sqlConsumerThreadCount == 0){//停止时生产者最后停止
        emit execSqlCount(consumerAllConsumeCount, producerAllProduceCount, sumValue);
    }
}

void SqlTooBusiness::onTimeout()
{
    emit updateState(QString::number(queue->getUsageRage()));
}
