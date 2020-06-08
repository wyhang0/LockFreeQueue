#ifndef SQLPRODUCER_H
#define SQLPRODUCER_H

#include <QObject>
#include <QSharedPointer>

#include "common.h"
#include "lockfreequeue.h"
#include "lockfreelinkqueue.h"

class SqlProducer : public QObject
{
    Q_OBJECT
public:
    SqlProducer(QSharedPointer<LOCKFREEQUEUE<int>> queue, quint64 startValue, quint64 endValue, bool *controlFlag, quint64 *tValue, QObject *parent=0);
    virtual ~SqlProducer();

public slots:
    void onProduce();

private:
    QSharedPointer<LOCKFREEQUEUE<int>> queue;
    quint64 startValue;
    quint64 endValue;
    bool *controlFlag;
    volatile quint64 *tValue;
};

#endif // SQLPRODUCER_H
