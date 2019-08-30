#ifndef SQLPRODUCER_H
#define SQLPRODUCER_H

#include <QObject>
#include <QSharedPointer>
#include <lockfreequeue.h>

class SqlProducer : public QObject
{
    Q_OBJECT
public:
    explicit SqlProducer(QSharedPointer<LockFreeQueue<QString>> queue, quint64 startValue, quint64 endValue, bool *controlFlag, QObject *parent=0);
    virtual ~SqlProducer();

    void emitProduce(QString str, int frequency);

public slots:
    void onProduce();
    void onProduce(QString str, int frequency);

signals:
    void produceDone(QThread* currentThread, quint64 count, SqlProducer *sqlProducer, bool isStopping);
    void produceDone(QThread* currentThread, quint64 count, SqlProducer *sqlProducer);
    void produce(QString str, int frequency);

private:
    QSharedPointer<LockFreeQueue<QString>> queue;
    quint64 startValue;
    quint64 endValue;
    volatile bool *controlFlag;
    quint64 count;
};

#endif // SQLPRODUCER_H
