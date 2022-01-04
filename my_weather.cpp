#include "my_weather.h"
#include "ui_my_weather.h"
#include <windows.h>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QTextCodec>
#include <QString>
#include <QXmlStreamReader>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostInfo>
#include <winsock.h>
#include <QtXmlPatterns/QXmlQuery>
#include <QPainter>
//#include <QXmlQuery>
#include <QFile>

my_weather::my_weather(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::my_weather)
{
    ui->setupUi(this);
    //netIp = GetNetIP(GetHtml("http://whois.pconline.com.cn/"));//公网ip
    ////获取本机ip 假如是 局域网 则获取 内部网IP
    //IPAddress=getIpInfo();//getIPAddress();
    // qDebug()<<"IP="<<IPAddress;
    //二级联动下拉框
    connect(ui->comboBoxS,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(ProvinceSlot(const QString &)));
    connect(ui->comboBoxCity,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(str_add()));
    cityStr="平顶山";
    //字符转连接
    str_add();
    mNetManager=new QNetworkAccessManager(this);
    caonima=new QNetworkAccessManager(this);
    //页面的初始化
    Init();
    mNetRequest=new QNetworkRequest;
    //如果mNetManger 变化，则调用对应槽函数
    connect(mNetManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onReplyFinished(QNetworkReply*)));
    //时间间隔为1500，刷新数据情况
    QTimer::singleShot(1500,this,SLOT(onGetWeather()));
}

my_weather::~my_weather()
{
    delete ui;
    //回收
    mNetManager->deleteLater();
    if(mNetRequest!=NULL)
    {
        delete mNetRequest;
        mNetRequest=NULL;
    }
}
void my_weather::str_add()
{
    //城市下拉框赋值
   cityStr=ui->comboBoxCity->currentText();
   //当省份下拉框换的时候，城市下拉框为空，返回
   if(cityStr=="\0")
   {
       return ;
   }
   //qDebug()<<cityStr<<"城市啊"<<comBoxStr;
   //字符串的连接
    weatherStr=QString("http://wthrcdn.etouch.cn/weather_mini?city=%1").arg(cityStr);
    //调用查询天气函数
    onGetWeather();
}

void my_weather::onGetWeather()
{
    //qDebug()<<QTime::currentTime().toString();
    mNetRequest->setUrl(QUrl(weatherStr));
    mNetRequest->setHeader(QNetworkRequest::UserAgentHeader,"RT-Thread ART");
    mNetManager->get(*mNetRequest);
}
