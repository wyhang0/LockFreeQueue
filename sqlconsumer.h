#ifndef SQLCONSUMER_H
#define SQLCONSUMER_H

#include <QObject>
#include <QSharedPointer>

#include "common.h"
#include "lockfreequeue.h"
#include "lockfreelinkqueue.h"

class SqlConsumer : public QObject
{
    Q_OBJECT
public:
    SqlConsumer(QSharedPointer<LOCKFREEQUEUE<int>> queue, bool *controlFlag, quint64 *tValue, QObject *parent=0);
    ~SqlConsumer();

public slots:
    void consume();

private:
    QSharedPointer<LOCKFREEQUEUE<int>> queue;
    bool *controlFlag;
    volatile quint64 *tValue;
};

#endif // SQLCONSUMER_H
