#include "sqltoobusiness.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>


SqlTooBusiness::SqlTooBusiness(QObject *parent) :
    QObject(parent)
{
    queue = QSharedPointer<LOCKFREEQUEUE<int>>(new LOCKFREEQUEUE<int>(4096));
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

bool SqlTooBusiness::start(int consumerThreadCount, int producerThreadCount, quint64 perProducerThreadProduceCount, quint64 *consumeTValue, quint64 *produceTValue)
{
    if(running){
        return true;
    }

    running = true;

    semaphore.release(consumerThreadCount + producerThreadCount);

    qDebug()<<QDateTime::currentDateTime();

    for(int i = 0; i<consumerThreadCount; i++){
        QThread *consumer = new QThread();
        SqlConsumer *sqlConsumer = new SqlConsumer(queue, &running, consumeTValue);
        sqlConsumer->moveToThread(consumer);
        connect(consumer, SIGNAL(started()), sqlConsumer, SLOT(consume()));
        connect(consumer, SIGNAL(finished()), sqlConsumer, SLOT(deleteLater()));
        connect(consumer, SIGNAL(finished()), consumer, SLOT(deleteLater()));
        connect(consumer, &QThread::finished, this, &SqlTooBusiness::threadDone);
        consumer->start();
    }

    for(quint64 i = 0; i<static_cast<quint64>(producerThreadCount); i++){
        QThread *producer = new QThread();
        SqlProducer *sqlProducer = new SqlProducer(queue, i*perProducerThreadProduceCount, (i+1)*perProducerThreadProduceCount, const_cast<bool*>(&running), produceTValue);
        sqlProducer->moveToThread(producer);
        connect(producer, SIGNAL(started()), sqlProducer, SLOT(onProduce()));
        connect(producer, SIGNAL(finished()), sqlProducer, SLOT(deleteLater()));
        connect(producer, SIGNAL(finished()), producer, SLOT(deleteLater()));
        connect(producer, &QThread::finished, this, &SqlTooBusiness::threadDone);
        producer->start();
    }

    return true;
}

void SqlTooBusiness::stop()
{
    running = false;
}

void SqlTooBusiness::onTimeout()
{
    emit updateState(QString::number(queue->getUsageRage()));
}

void SqlTooBusiness::threadDone()
{
    semaphore.acquire();
    if(semaphore.available() == 0){
        qDebug()<<QDateTime::currentDateTime();
        emit done();
    }
}
