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

void my_weather::ProvinceSlot(const QString &ProvinceName)
{
   // qDebug()<<ProvinceName<<"22222222222";
    choose=3;
    caonima->get(QNetworkRequest(QUrl("http://www.webxml.com.cn/WebServices/WeatherWebService.asmx/getSupportCity?byProvinceName=" + ProvinceName)));
    evenLoop.exec();
}
void my_weather::replyFinished(QNetworkReply *reply)
{
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString all = codec->toUnicode(reply->readAll());
        QXmlStreamReader reader(all);
        //提取城市名字
        while (!reader.atEnd())
        {
            reader.readNext();
            if (reader.isStartElement())
            {
                if (reader.name() == "string")
                {
                    List.append(reader.readElementText());
                }
            }
        }

        //刷新省份下拉框
        switch(choose)
        {
        case 1:
                for (QStringList::const_iterator citer = List.constBegin(); citer != List.constEnd(); citer++)
                        {
                            ui->comboBoxS->addItem(*citer);
                        }
                        ui->comboBoxS->setCurrentIndex(0);//默认显示第一个

                        break;
         //刷新城市下拉框
        case 2:
            for (QStringList::const_iterator citer = List.constBegin(); citer != List.constEnd(); citer++)
                        {
                            QString str = citer->left(citer->indexOf(QChar(' ')));
                            ui->comboBoxCity->addItem(str);
                        }
                        ui->comboBoxCity->setCurrentIndex(0);//默认第一个
                        List.clear();
                        break;
         //刷新城市下拉框
        case 3:
            ui->comboBoxCity->clear();
                        for (QStringList::const_iterator citer = List.constBegin(); citer != List.constEnd(); citer++)
                        {
                            QString str = citer->left(citer->indexOf(QChar(' ')));
                            ui->comboBoxCity->addItem(str);
                        }
                        ui->comboBoxCity->setCurrentIndex(0);//默认显示第一个
                        List.clear();
                        break;

        }
}

void my_weather::onReplyFinished(QNetworkReply *reply)
{
   // qDebug()<<QTime::currentTime().toString();
    QByteArray weather=reply->readAll();
    if(!weather.isEmpty())
    {
        analyWeatherXML(weather);
    }
    reply->deleteLater();

}
void my_weather::analyWeatherXML(QByteArray xml)
{
    //如果数据为空 返回
    if(xml.isEmpty())
    {
        return;
    }
    //qDebug()<<xml.data();
    //json 解析数据 并显示在 textedit上
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(xml,&err);
    QJsonObject jsonObj= jsonDoc.object().value("data").toObject();

    QJsonObject yesterday = jsonObj.value("yesterday").toObject(); //昨天
    ui->textEdit->clear();
    ui->textEdit->append(JsonObj2String(yesterday));

    QJsonArray forecast = jsonObj.value("forecast").toArray();
    QJsonObject day0 = forecast[0].toObject(); //今天
    ui->textEdit_2->clear();
    ui->textEdit_2->append(JsonObj2String(day0));
    ui->textEdit_today->clear();
    ui->textEdit_today->append(JsonObj2String(day0));

    QJsonObject day1 = forecast[1].toObject(); //明天
    ui->textEdit_3->clear();
    ui->textEdit_3->append(JsonObj2String(day1));

    QJsonObject day2 = forecast[2].toObject();
    ui->textEdit_4->clear();
    ui->textEdit_4->append(JsonObj2String(day2));

    QJsonObject day3 = forecast[3].toObject();
    ui->textEdit_5->clear();
    ui->textEdit_5->append(JsonObj2String(day3));

    QJsonObject day4 = forecast[4].toObject();
    ui->textEdit_6->clear();
    ui->textEdit_6->append(JsonObj2String(day4));
}

QString my_weather::JsonObj2String(const QJsonObject jsonObj)
{
    QString weather;
    if(!jsonObj.isEmpty())
    {
        weather += jsonObj.value("date").toString()          + "\n";
        if(!jsonObj.value("fx").toString().isEmpty())
            weather += jsonObj.value("fx").toString()        + "\n";
        else
            weather += jsonObj.value("fengxiang").toString() + "\n";
        weather += jsonObj.value("high").toString()          + "\n";
        weather += jsonObj.value("low").toString()           + "\n";
        weather += jsonObj.value("type").toString();
    }
    return weather;

}
void my_weather::Init()
{
    choose=1;
    //获取所有省份
    caonima->get(QNetworkRequest(QUrl("http://www.webxml.com.cn/WebServices/WeatherWebService.asmx/getSupportProvince?")));

    connect(caonima,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    connect(caonima, &QNetworkAccessManager::finished, &evenLoop, &QEventLoop::quit);
    evenLoop.exec();

       // choose =2;//初始化城市列表（代號：2）
        //QString City = List.at(0);//默認省份列表第一項（直轄市）爲默認省份
        List.clear();
       // caonima->get(QNetworkRequest(QUrl("http://www.webxml.com.cn/WebServices/WeatherWebService.asmx/getSupportCity?byProvinceName=" + City)));//發送請求

        //---局部事件循環
        //connect(caonima, &QNetworkAccessManager::finished, &evenLoop, &QEventLoop::quit);
        //evenLoop.exec();
}
//QString my_weather::getIPAddress()
//{
//    QString ipAddress;
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
//    for (int i = 0; i < ipAddressesList.size(); ++i)
//    {
//        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&  ipAddressesList.at(i).toIPv4Address())
//        {
//            ipAddress = ipAddressesList.at(i).toString();
//            break;
//        }
//    }
//    if (ipAddress.isEmpty())
//        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
//    return ipAddress;
//}
QString my_weather::getIpInfo()
{
    //mac地址
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    QString info = QString::fromLocal8Bit("IP:")+QString::fromLocal8Bit(";MAC:");;
    if(list.size() != 0)//内网ip
    {
        info= "9999,9999,9999,9999";
       QString netIp = GetNetIP(GetHtml("http://whois.pconline.com.cn/"));//公网ip
        //QHostInfo类作用，获取主机名，也可以通过主机名来查找IP地址，或者通过IP地址来反向查找主机名。
        char* ip = getIP();
        info = "IP:"+QString::fromLatin1(ip)+";MAC:"+list[0].hardwareAddress();
    }
    qDebug()<<"IP="<<IPAddress;
    return info;
}

//外网的获取方法，通过爬网页来获取外网IP
QString my_weather::GetHtml(QString url)//网页源代码
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QString all = codec->toUnicode(reply->readAll());
//    QXmlStreamReader reader(all);
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    qDebug()<<"__________________\n"<<responseData<<"_____________________\n";
    return QString(responseData);

}
QString my_weather::GetNetIP(QString webCode)//公网ip
{
//    qDebug()<<"________"<<webCode;
//    QString queryStr;
//    QXmlQuery query;
//    QString sXmlPath="F:\\qtfile\\my_weather\\xpath.xml";
//    QFile db(sXmlPath);

//    QString sfist = "工作目录";
//    QString ssecond = "工作目录";
//    if ( ! db.exists()) {
//            //errMsg = "Xml文件未找到！";
//           // return ;
//        qDebug()<<"文件错误1";
//        }

//        if (!db.open(QIODevice::ReadOnly | QIODevice::Text)){
//            //errMsg = "文件打不开！";
//            qDebug()<<"文件错误2";
//            //return ;
//        }
////    QByteArray byte=webCode.toUtf8();
////    const char *utf8=byte.data();
//    query.setFocus(&db);
//     qDebug()<<"查询shuju有误";
//    query.setQuery("tree[@id='0']/item[@text='"+sfist+"']/item[@text='"+ssecond+"']/userdata[@name='value']");
//    if(!query.isValid())
//    {
//        qDebug()<<"查询参数有误";
//        //return;
//    }
//    query.evaluateTo(&queryStr);
//    qDebug()<<"字符串是："<<queryStr;
    QString web = webCode.replace(" ", "");
    web = web.replace("\r", "");
    web = web.replace("\n", "");
    QStringList list = web.split("<br/>");
    if(list.size() < 4)
        return "0.0.0.0";
    QString tar = list[3];
    QStringList ip = tar.split("=");
    return ip[1];
}
//内网获取方法
char* my_weather::getIP()
{
    WSAData wsaData;
    if(WSAStartup(MAKEWORD(1,1),&wsaData)!=0)//需要添加库WS2_32.lib，如果不明白什么意思，参见http://blog.csdn.net/bolike/article/details/7584727
    {
        return NULL;
    }

    char *ipAddress = fetchIPAddress();
    WSACleanup();
    return ipAddress;
}
char* my_weather::fetchIPAddress()
{
    char host_name[225];
    if(gethostname(host_name,sizeof(host_name)) == SOCKET_ERROR)
    {
        qDebug()<<"Error "<<WSAGetLastError()<<" when getting local host name."<<endl;
    }
    struct hostent *phe = gethostbyname(host_name);
    if(phe == 0)
    {
        qDebug()<<"get host entry error";
    }
    struct in_addr addr;
    memcpy(&addr,phe->h_addr_list[0],sizeof(struct in_addr));
    return inet_ntoa(addr);
}
