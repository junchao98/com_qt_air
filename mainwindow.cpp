#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include  <stdlib.h>



#include <QSqlDatabase>
#include <QStringList>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>



#define DATA_LEN 520

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    foreach( const QSerialPortInfo &Info,QSerialPortInfo::availablePorts())//读取串口信息
          {
                  qDebug() << "portName    :"  << Info.portName();//调试时可以看的串口信息
                  qDebug() << "Description   :" << Info.description();
                  qDebug() << "Manufacturer:" << Info.manufacturer();

              QSerialPort serial;
              serial.setPort(Info);

             if( serial.open( QIODevice::ReadWrite) )//如果串口是可以读写方式打开的
              {
                ui->comboBox_portName->addItem(Info.portName() );//在comboBox那添加串口号
                 serial.close();//然后自动关闭等待人为开启（通过那个打开串口的PushButton）
             }
          }



    id_max=0;
    db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=D:/test.mdb");
    db_checke= db.open();

    if(db_checke){
     qDebug() << "db open  ok!!";

    }





}

MainWindow::~MainWindow()
{
   // my_serialPort->close();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{



    my_serialPort = new QSerialPort(this);

    if(ui->comboBox_portName->currentText()=="")goto error1;
    my_serialPort->setPortName(ui->comboBox_portName->currentText());


    my_serialPort->open(QIODevice::ReadWrite);
    my_serialPort->setBaudRate(  ui->comboBox_baudRate->currentText().toInt() );//波特率
    my_serialPort->setDataBits( QSerialPort::Data8 );//数据字节，8字节
    my_serialPort->setParity( QSerialPort::NoParity );//校验，无
    my_serialPort->setFlowControl( QSerialPort::NoFlowControl );//数据流控制,无
    my_serialPort->setStopBits( QSerialPort::OneStop );//一位停止位

       timer = new QTimer(this);
       connect( timer, SIGNAL( timeout() ), this, SLOT( readMyCom() ) );
       timer->start(500);//每秒读一次

       timer_data = new QTimer(this);
       connect( timer_data , SIGNAL( timeout() ), this, SLOT( data_kill() ) );       //数据处理
       //timer_data ->start(1000);//每秒读一次




       open_flag = 1;


  error1:
        qDebug()<< "打开失败"<<"\n";
}


void  MainWindow::data_kill()          //数据跟新
{

    QString tem;
    QSqlQuery query(db);



    // tem = QString("%1").arg((sensor_data.humi)); //
     ui->label_humi->setText(tem);

     query.prepare("INSERT INTO humi (id, data, w_time) "
                         "VALUES (:id, :data, :w_time)");
           query.bindValue(":id", 122);
           query.bindValue(":data", "Bart");
           query.bindValue(":w_time", tem);
           query.exec();


    tem = QString("%1").arg((sensor_data.press));
    ui->label_press  ->setText(tem);

    query.prepare("INSERT INTO press (id, data, w_time) "
                        "VALUES (:id, :data, :w_time)");
          query.bindValue(":id", 122);
          query.bindValue(":data", "Bart");
          query.bindValue(":w_time", tem);
          query.exec();


     tem = QString("%1").arg((sensor_data.sun));
     ui->label_sun ->setText(tem);

     query.prepare("INSERT INTO sun (id, data, w_time) "
                         "VALUES (:id, :data, :w_time)");
           query.bindValue(":id", 122);
           query.bindValue(":data", "Bart");
           query.bindValue(":w_time", tem);
           query.exec();



}


void MainWindow::readMyCom()
{
    if(open_flag){

   QString re_data;

  // QByteArray _ch_data;
  char ch_data[DATA_LEN+1]={0};


   my_serialPort->readLine(ch_data,DATA_LEN);

   if(strlen(ch_data)>20){

   re_data=QString(QLatin1String(ch_data));

   ui->textBrowser_old->append(re_data);

   }else{goto LESSDATA;}


   for(int i=0;i<DATA_LEN;i++)
   {
    if(ch_data[i]=='B'&&ch_data[i+1]=='G'){

        for(int j=20;j<101;j++)
        {
           if(ch_data[i+j]=='E'&&ch_data[i+j+1]=='D')break;

           if(j==100)goto BADDATA;

        }
        for(int j=0;j<94;j++){
            if(ch_data[j] == ',')ch_data[j]='\0';
        }

         qDebug()<< "get";
        /*
       //for(int j=0; j <= 95 ;j++){
          qDebug()<<"日期："<<ch_data[i+21]<<ch_data[i+22]<<ch_data[i+23]<<ch_data[i+24];
          //qDebug()<<"温度："<<ch_data[i+25]<<ch_data[i+26]<<"\\"<<ch_data[i+27]<<ch_data[i+28];
          qDebug()<<"温度："<<ch_data[i+47]<<ch_data[i+48]<<ch_data[i+49];
          qDebug()<<"湿度："<<ch_data[i+56]<<ch_data[i+57]<<ch_data[i+58];

        */

        ui->label_date->setText(QString(ch_data+21));      //时间




        if(ui->comboBox__num->currentText() ==  QString(ch_data+3)){     //节点



            if(QString(ch_data+12) == "YTHM"){     //温湿度

                 qDebug()<<"温湿度";
             re_data = QString(ch_data+60);
             sensor_data.humi=re_data;
             ui->label_humi->setText(re_data);


             re_data = QString(ch_data+47+4);
             sensor_data.temp =re_data;
             ui->label_temp->setText(re_data);

             sensor_data.nova=1;


             if(QString(ch_data+69) == "1"){                    //stat
                 ui->label_v_stat ->setText("正常");
             } else{

                  ui->label_v_stat ->setText( QString(ch_data+74) +"-"+ QString(ch_data+87));
             }

            }

            if(QString(ch_data+12) == "YTMP"){      //温度

                qDebug()<<"温度";
                if(QString(ch_data+73) =="2" ){

                    ui->label_temp->setText("data_error");
                }else{
                re_data = QString(ch_data+47+4);
                sensor_data.temp =re_data;
                ui->label_temp->setText(re_data);

                sensor_data.nova=1;

                }
            }


            if(QString(ch_data+12) == "YTBR"){      //雨量

                     qDebug()<<"雨量";
                if(QString(ch_data+68) == "2"){

                    ui->label_rain->setText("data_error");

                }
               re_data = QString(ch_data+47+4);
                sensor_data.rain =re_data;
               ui->label_rain->setText(re_data);

               sensor_data.nova=1;
            }


            if(QString(ch_data+12) == "YWPD"){      //风速向

                 qDebug()<<"风速向";
                if(QString(ch_data+73) =="####" ){ //待定

                    ui->label_win_v_2->setText("data_error");
                }else{

                re_data = QString(ch_data+47+4);    //风向
                sensor_data.win_v =re_data;
                ui->label_win_v_2->setText(re_data);


                re_data = QString(ch_data+59);   //风速度
                sensor_data.win =re_data;
                ui->label_win->setText(re_data);


                sensor_data.nova=1;

                }
            }


         }

        }



    }





   }


    //
    return;

 BADDATA:
    qDebug()<<"bad data";
   return;

 LESSDATA:
   return;


}




void MainWindow::on_pushButton_close_clicked()
{
    my_serialPort->close();
    open_flag=0;
}

void MainWindow::on_pushButton_wdata_clicked()
{

    QStringList tables;
    QString tabName,sqlString;
    QTextStream out(stdout);
    QSqlQuery q(sqlString);


      if(db_checke){


                  //读数据库中的表
                  QStringList tables;
                  QString tabName,sqlString;
                  tables = db.tables(QSql::Tables);
                  //读表中记录
                  for (int i = 0; i < tables.size(); ++i){
                      tabName = tables.at(i);
                      qDebug()<<tabName;
                      sqlString = "select * from " + tabName;
                      QSqlQuery q(sqlString);
                      //QSqlQuery q("select * from product");
                      QSqlRecord rec = q.record();
                      int fieldCount = rec.count();
                      qDebug() << "Number of columns: " << fieldCount;

                      QString fieldName;
                      for(int j=0;j<fieldCount;j++){
                          fieldName = rec.fieldName(j);
                          out<<fieldName<<"\t";
                      }
                      out<<endl;
                      while(q.next()){
                        for(int i=0;i<fieldCount;i++){
                                  if(q.value(i).toInt() > id_max)id_max=q.value(i).toInt()+1;
                                   qDebug() <<"id_max="<< id_max;
                                  out<<q.value(i).toString();
                                  out<<"\t";
                              }
                              out<<endl;
                          }



                }

      }  // if


      //QSqlQuery query;



}

void MainWindow::on_pushButton_2_clicked()
{


    qDebug() << "test !!!!!!";


}
