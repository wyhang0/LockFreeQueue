#ifndef SQLTOOBUSINESS_H
#define SQLTOOBUSINESS_H

#include <QSharedPointer>
#include <QThread>
#include <QTimer>

#include "lockfreequeue.h"
#include "sqlconsumer.h"
#include "sqlproducer.h"

class SqlTooBusiness : public QObject
{
    Q_OBJECT
public:
    explicit SqlTooBusiness(QObject *parent = 0);
    ~SqlTooBusiness();

    bool start(int consumerThreadCount, int producerThreadCount, quint64 perProducerThreadProduceCount);
    void stop();

signals:
    void produce(QString str);
    void execSqlCount(quint64 consumerAllConsumeCount, quint64 producerAllProduceCount, quint64 sumValue);
    void updateState(QString queueUsageRate);

protected slots:
    void onConsumeDone(QThread* currentThread, quint64 count, quint64 sum, SqlConsumer *sqlConsumer);
    void onProduceDone(QThread* currentThread, quint64 count, SqlProducer *sqlProducer, bool isStopping);
    void onProduceDone(QThread* currentThread, quint64 count, SqlProducer *sqlProducer);
    void onTimeout();

private:
    quint64 consumerAllConsumeCount;//消费者消费的所有产品数
    quint64 producerAllProduceCount;//生产者生产的所有产品数
    QSharedPointer<LockFreeQueue<QString>> queue;//缓冲队列
    int sqlConsumerThreadCount;
    int sqlProducerThreadCount;
    bool running;

    QTimer timer;
    quint64 sumValue;

    QThread *thread;
};

#endif // SQLTOOBUSINESS_H
