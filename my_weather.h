#ifndef MY_WEATHER_H
#define MY_WEATHER_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
namespace Ui {
class my_weather;
}

class my_weather : public QMainWindow
{
    Q_OBJECT

public:
    explicit my_weather(QWidget *parent = 0);
    ~my_weather();
private slots:
    void str_add();
    void onGetWeather();
    void onReplyFinished(QNetworkReply *reply);
    void ProvinceSlot(const QString &);

private:
    void analyWeatherXML(QByteArray xml);
    QString JsonObj2String(const QJsonObject jsonObj);
    void Init();
    QString GetHtml(QString url);

private:
    Ui::my_weather *ui;
    QNetworkAccessManager *mNetManager;
    QNetworkAccessManager *caonima;
    bool bPressFlag;
    QPoint dragPosition;
    bool gIndex;
    QString weatherStr;
    QString cityStr;
    int choose;

};

#endif // MY_WEATHER_H
