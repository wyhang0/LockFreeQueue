#ifndef SQLTOOBUSINESS_H
#define SQLTOOBUSINESS_H

#include <QSharedPointer>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QSemaphore>

#include "common.h"
#include "lockfreelinkqueue.h"
#include "lockfreequeue.h"
#include "sqlconsumer.h"
#include "sqlproducer.h"

class SqlTooBusiness : public QObject
{
    Q_OBJECT
public:
    explicit SqlTooBusiness(QObject *parent = 0);
    ~SqlTooBusiness();

    bool start(int consumerThreadCount, int producerThreadCount, quint64 perProducerThreadProduceCount, quint64 *consumeTValue, quint64 *produceTValue);
    void stop();

signals:
    void updateState(QString queueUsageRate);
    void done();

protected slots:
    void onTimeout();
    void threadDone();

private:
    QSharedPointer<LOCKFREEQUEUE<quint64>> queue;//缓冲队列
    bool running;

    QThread *thread;
    QTimer timer;
    QSemaphore semaphore;
    QElapsedTimer elapsedTimer;
};

#endif // SQLTOOBUSINESS_H
