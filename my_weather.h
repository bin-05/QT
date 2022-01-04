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
private:
    QString cityStr;
};
#endif // MY_WEATHER_H
