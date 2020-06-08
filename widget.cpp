#include "widget.h"
#include "ui_widget.h"

#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(&sqlTooBusiness, &SqlTooBusiness::updateState, this, &Widget::onUpdateState);
    connect(&sqlTooBusiness, &SqlTooBusiness::done, this, &Widget::onDone);

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
    QApplication::processEvents();
}

void Widget::on_pushButton_Start_clicked()
{
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    quint64 an = ui->spinBox_ProducerCount->text().toInt()*ui->spinBox_ProduceCount->text().toULongLong()-1;

    consumeTValue = produceTValue = an*(an+1)/2;

    sqlTooBusiness.start(ui->spinBox_ConsumerCount->text().toInt(),
                         ui->spinBox_ProducerCount->text().toInt(),
                         ui->spinBox_ProduceCount->text().toULongLong(),
                         &consumeTValue,
                         &produceTValue);
}

void Widget::onUpdateState(QString queueUsageRate)
{
    ui->lineEdit_QueueUsageRate->setText(queueUsageRate);
    ui->label_CRValue->setText(QString::number(consumeTValue));
    ui->label_PRValue->setText(QString::number(produceTValue));
}

void Widget::onDone()
{
    if(!ui->pushButton_Stop->isEnabled()){
        ui->pushButton_Start->setEnabled(true);
    }

    if(consumeTValue != produceTValue){
        QMessageBox::warning(this, "", "");
    }else{
        if(ui->pushButton_Stop->isEnabled()){
            QTimer::singleShot(100, this, &Widget::on_pushButton_Start_clicked);
        }
    }
}
