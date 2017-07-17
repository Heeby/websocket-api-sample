#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#ifdef Q_OS_WIN
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCharts/QtCharts>
#else
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtCharts>
#endif

#include <QtWebSockets/QtWebSockets>
#include "defines.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QWebSocket * _wsBTC;
    QWebSocket * _wsETH;
    QString _subID;

private slots:
    void onWsBtcConnected();
    void onWsBtcDisconnected();
    void onWsBtcBinaryMessageReceived(const QByteArray &message);

    void onWsEthConnected();
    void onWsEthDisconnected();
    void onWsEthBinaryMessageReceived(const QByteArray &message);
    
    void on_btnRealtime_clicked();
    void on_btn5Min_clicked();
};

#endif // MAINWINDOW_H
