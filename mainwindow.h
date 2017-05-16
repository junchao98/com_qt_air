#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QSqlDatabase>
#include <QStringList>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
     void readMyCom();
     void on_pushButton_close_clicked();
     void data_kill();

     void on_pushButton_wdata_clicked();

     void on_pushButton_2_clicked();

private:

    Ui::MainWindow *ui;

   QTimer *timer;//（用于计时）
   QTimer *timer_data;//（用于计时）
   QSerialPort *my_serialPort;//(实例化一个指向串口的指针，可以用于访问串口)
   QByteArray requestData;//（用于存储从串口那读取的数据）
   bool open_flag=1;

   struct _sensor_data{
       QString  temp;
       QString  humi;
       QString  sun;
       QString  press;
       QString rain;
       bool nova;

   };
    _sensor_data sensor_data;
     QSqlDatabase db;

     bool db_checke;
     int id_max;

};

#endif // MAINWINDOW_H
