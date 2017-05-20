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

#include <QMessageBox>
#include<QPaintEvent> //用于绘画事件
#include<QtGui> //引入用到的控件

#define inf 0x3f3f3f3f
#define DATA_MAX 500



 int  point_num =10;         //用折线点的个数
 int data_cache[DATA_MAX]={7,8,1,9,6,8,4,8,6,9,8,7,5};

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

                }else{
               re_data = QString(ch_data+47+4);
                sensor_data.rain =re_data;
               ui->label_rain->setText(re_data);

               sensor_data.nova=1;
                }
            }

            if(QString(ch_data+12) == "YTPS"){      //气压

                     qDebug()<<"气压";

                if(QString(ch_data+69) == "2"){

                    ui->label_press->setText("data_error");

                }else{
               re_data = QString(ch_data+47+4);
                sensor_data.press =re_data;
               ui->label_press->setText(re_data);

               sensor_data.nova=1;
                }
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






void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

   int pointx=35,pointy=230;//确定坐标轴起点坐标，这里定义(35,280)
   int width=400-pointx,height=200;//确定坐标轴宽度跟高度 上文定义画布为600X300，宽高依此而定。

   //绘制坐标轴 坐标轴原点(35，280)

   QColor backColor = qRgb(255,255,255);    //画布初始化背景色使用白色
   image.fill(backColor);//对画布进行填充

   painter.setRenderHint(QPainter::Antialiasing, true);//设置反锯齿模式，好看一点
   painter.drawRect(5,5,450-5,250-5);//外围的矩形，从(5,5)起，到(590,290)结束，周围留了5的间隙。

   painter.drawLine(pointx,pointy,width+pointx,pointy);//坐标轴x宽度为width
   painter.drawLine(pointx,pointy-height,pointx,pointy);//坐标轴y高度为height



  // srand(time(NULL));

   //获得数据中最大值和最小值、平均数
   int n=point_num;//n为数据个数
   double sum=0;
   double ave=0;
   int _ma=0;//数组里的最大值
   int _mi=inf;

  //int a[n];//数据储存在数组a中，大小为n

   //for(int i=0;i<n;i++)a[i]=rand()%40+20;

    int maxpos=0,minpos=0;

   for(int i=0;i<n;i++)
   {
       sum+=data_cache[i];
       if(data_cache[i]>_ma){
           _ma=data_cache[i];
           maxpos=i;
       }
       if(data_cache[i]<_mi){
           _mi=data_cache[i];
           minpos=i;
       }
   }
   ave=sum/n;//平均数

   double kx=(double)width/(n-1); //x轴的系数
   double ky=(double)height/_ma;//y方向的比例系数
   QPen pen,penPoint;
   pen.setColor(Qt::black);
   pen.setWidth(2);

   penPoint.setColor(Qt::blue);
   penPoint.setWidth(5);
   for(int i=0;i<n-1;i++)
   {
       //由于y轴是倒着的，所以y轴坐标要pointy-a[i]*ky 其中ky为比例系数
       painter.setPen(pen);//黑色笔用于连线
       painter.drawLine(pointx+kx*i,pointy-data_cache[i]*ky+5,pointx+kx*(i+1),pointy-data_cache[i+1]*ky+5);
       painter.setPen(penPoint);//蓝色的笔，用于标记各个点
       painter.drawPoint(pointx+kx*i,pointy-data_cache[i]*ky+5);
   }
   painter.drawPoint(pointx+kx*(n-1),pointy-data_cache[n-1]*ky+5);//绘制最后一个点

    //绘制平均线
    QPen penAve;
    penAve.setColor(Qt::red);//选择红色
    penAve.setWidth(2);
    penAve.setStyle(Qt::DotLine);//线条类型为虚线
    painter.setPen(penAve);
    painter.drawLine(pointx,pointy-ave*ky,pointx+width,pointy-ave*ky);

//    //绘制最大值和最小值
//    QPen penMaxMin;
//    penMaxMin.setColor(Qt::darkGreen);//暗绿色
//    painter.setPen(penMaxMin);
//    painter.drawText(pointx+kx*maxpos-kx,pointy-a[maxpos]*ky-5,
//                     "最大值"+QString::number(_ma));
//    painter.drawText(pointx+kx*minpos-kx,pointy-a[minpos]*ky+15,
//                     "最小值"+QString::number(_mi));

//    penMaxMin.setColor(Qt::red);
//    penMaxMin.setWidth(7);
//    painter.setPen(penMaxMin);
//    painter.drawPoint(pointx+kx*maxpos,pointy-a[maxpos]*ky);//标记最大值点
//    painter.drawPoint(pointx+kx*minpos,pointy-a[minpos]*ky);//标记最小值点


   //绘制刻度线
   QPen penDegree;
   penDegree.setColor(Qt::black);
   penDegree.setWidth(2);
   painter.setPen(penDegree);
   //画上x轴刻度线

   for(int i=0;i<10;i++)//分成10份
   {
       //选取合适的坐标，绘制一段长度为4的直线，用于表示刻度
       painter.drawLine(pointx+(i+1)*width/10,pointy,pointx+(i+1)*width/10,pointy+4);
       painter.drawText(pointx+(i+0.65)*width/10,
                        pointy+10,QString::number((int)((i+1)*((double)n/10))));
   }
   //y轴刻度线
   double _maStep=(double)_ma/10;//y轴刻度间隔需根据最大值来表示
   for(int i=0;i<10;i++)
   {
       //代码较长，但是掌握基本原理即可。
       //主要就是确定一个位置，然后画一条短短的直线表示刻度。

       painter.drawLine(pointx,pointy-(i+1)*height/10,
                        pointx-4,pointy-(i+1)*height/10);
       painter.drawText(pointx-20,pointy-(i+0.85)*height/10,
                        QString::number((int)(_maStep*(i+1))));
   }

}











void MainWindow::on_pushButton_3_clicked()
{

    QMessageBox::information(this, QString::fromLocal8Bit("error"),QString::fromLocal8Bit("bad data!!!!!!!"));


}
