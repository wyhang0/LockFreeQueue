#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(&sqlTooBusiness, SIGNAL(execSqlCount(quint64, quint64, quint64)), this, SLOT(onExecSqlCount(quint64, quint64, quint64)));
    connect(&sqlTooBusiness, SIGNAL(updateState(QString)), this, SLOT(onUpdateState(QString)));

    ui->pushButton_Start->setEnabled(true);
    ui->pushButton_Stop->setEnabled(false);
}

Widget::~Widget()
{
    sqlTooBusiness.stop();
    delete ui;
}

void Widget::on_pushButton_Stop_clicked()
{
    ui->pushButton_Stop->setEnabled(false);
    sqlTooBusiness.stop();
}

void Widget::on_pushButton_Start_clicked()
{
    ui->pushButton_Start->setEnabled(false);
    sqlTooBusiness.start(ui->spinBox_ConsumerCount->text().toInt(),
                         ui->spinBox_ProducerCount->text().toInt(),
                         ui->spinBox_ProduceCount->text().toULongLong());
    quint64 an = ui->spinBox_ProducerCount->text().toInt()*ui->spinBox_ProduceCount->text().toULongLong()-1;
    ui->label_TValue->setText(QString::number(an*(an+1)/2));
    ui->pushButton_Stop->setEnabled(true);
}

void Widget::onExecSqlCount(quint64 consumerAllConsumeCount, quint64 producerAllProduceCount, quint64 sumValue)
{
    ui->lineEdit_ConsumeCount->setText(QString::number(consumerAllConsumeCount));
    ui->lineEdit_ProduceCount->setText(QString::number(producerAllProduceCount));
    ui->label_AValue->setText(QString::number(sumValue));

    ui->pushButton_Start->setEnabled(true);
    ui->pushButton_Stop->setEnabled(false);
}

void Widget::onUpdateState(QString queueUsageRate)
{
    ui->lineEdit_QueueUsageRate->setText(queueUsageRate);
}
