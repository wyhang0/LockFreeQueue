#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "sqltoobusiness.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_Stop_clicked();
    void on_pushButton_Start_clicked();

    void onExecSqlCount(quint64 consumerAllConsumeCount, quint64 producerAllProduceCount, quint64 sumValue);
    void onUpdateState(QString queueUsageRate);

private:
    Ui::Widget *ui;
    SqlTooBusiness sqlTooBusiness;
};

#endif // WIDGET_H
