#ifndef SQLCONSUMER_H
#define SQLCONSUMER_H

#include <QObject>
#include <QSharedPointer>
#include <lockfreequeue.h>

class SqlConsumer : public QObject
{
    Q_OBJECT
public:
    explicit SqlConsumer(QSharedPointer<LockFreeQueue<QString>> queue, bool *controlFlag, QObject *parent=0);
    ~SqlConsumer();

public slots:
    void consume();

signals:
    void consumeDone(QThread *currentThread, quint64 count, quint64 sum, SqlConsumer *sqlConsumer);

private:
    QSharedPointer<LockFreeQueue<QString>> queue;
    volatile bool *controlFlag;
    quint64 count;
};

#endif // SQLCONSUMER_H
