#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>

#include <QUrl>
#include "qcompressor.h"
#include "huobitick.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //连接两个websocket行情源
    _wsBTC = new QWebSocket();
    connect(_wsBTC, &QWebSocket::connected, this, &MainWindow::onWsBtcConnected);
    _wsBTC->open(QString(HUOBI_WSURL_BTC));

    _wsETH = new QWebSocket();
    connect(_wsETH, &QWebSocket::connected, this, &MainWindow::onWsEthConnected);
    _wsETH->open(QString(HUOBI_WSURL_ETH));
}

MainWindow::~MainWindow()
{
    delete _wsBTC;
    delete _wsETH;

    delete ui;
}


void MainWindow::onWsBtcConnected()
{
    qDebug() << "onWsBtcConnected";
    connect(_wsBTC, &QWebSocket::disconnected, this, &MainWindow::onWsBtcDisconnected);
    connect(_wsBTC, &QWebSocket::binaryMessageReceived, this, &MainWindow::onWsBtcBinaryMessageReceived);
    QTime t;
    if (_subID.isEmpty()) {
        _subID = "brucezhao" + t.currentTime().toString("hhmmsszzz");
    }

    //订阅BTC的kline
    QJsonObject json;
    json.insert("sub", "market.btccny.kline.1min");
    json.insert("id", _subID);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(json);

    QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
    QString jsonStr(ba);
    //qDebug() << jsonStr;

    _wsBTC->sendTextMessage(jsonStr);

    //订阅LTC的kline
    json.insert("sub", "market.ltccny.kline.1min");
    jsonDoc.setObject(json);

    ba = jsonDoc.toJson(QJsonDocument::Compact);
    jsonStr = QString(ba);
    //qDebug() << jsonStr;

    _wsBTC->sendTextMessage(jsonStr);
}


void MainWindow::onWsBtcDisconnected()
{
    qDebug() << "onWsBtcDisconnected";
}


void MainWindow::onWsBtcBinaryMessageReceived(const QByteArray &message)
{
    QByteArray baUnpack;
    //取回来的内容要解压
    bool bResult = QCompressor::gzipDecompress(message, baUnpack);
    if (bResult) {
        //解析Json
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(baUnpack, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("ping")) {
                    //是心跳包
                    QJsonObject jsonPong;
                    jsonPong.insert("pong", jsonObj.value("ping"));
                    QJsonDocument docPong(jsonPong);
                    _wsBTC->sendTextMessage(QString(docPong.toJson(QJsonDocument::Compact)));
                }
                else if (jsonObj.contains("ch")) {
                    //订阅的数据包
                    QString symbol = jsonObj.value("ch").toString();
                    if (symbol == "market.btccny.kline.1min") {
                        //自己的处理代码
                    } else if (symbol == "market.ltccny.kline.1min"){
                        //自己的处理代码
                    }
                }
                else if (jsonObj.contains("rep")) {
                    //请求返回的数据包
                    QString sSymbol = jsonObj.value("rep").toString();
                    if (sSymbol == "market.btccny.kline.1min") {
                        //自己的处理代码
                    }
                    else if (sSymbol == "market.btccny.kline.5min") {
                        //自己的处理代码
                    }
                }
            }
        }
    } else {
        qDebug() << "Failure";
    }
}

void MainWindow::onWsEthConnected()
{
    qDebug() << "onWsEthConnected";
    connect(_wsETH, &QWebSocket::disconnected, this, &MainWindow::onWsEthDisconnected);
    connect(_wsETH, &QWebSocket::binaryMessageReceived, this, &MainWindow::onWsEthBinaryMessageReceived);

    //订阅eth的MarketDetail
    QJsonObject json;
    json.insert("sub", "market.ethcny.detail");
    QTime t;
    if (_subID.isEmpty()) {
        _subID = "brucezhao" + t.currentTime().toString("hhmmsszzz");
    }
    json.insert("id", _subID);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(json);

    QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
    QString jsonStr(ba);

    //发送订阅信息
    _wsETH->sendTextMessage(jsonStr);
}

void MainWindow::onWsEthDisconnected()
{
    qDebug() << "onWsEthDisconnected";
}

void MainWindow::onWsEthBinaryMessageReceived(const QByteArray &message)
{
    QByteArray baUnpack;
    //取回来的内容要解压
    bool bResult = QCompressor::gzipDecompress(message, baUnpack);
    if (bResult) {
        //解析Json
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(baUnpack, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("ping")) {
                    //是心跳包
                    QJsonObject jsonPong;
                    jsonPong.insert("pong", jsonObj.value("ping"));
                    QJsonDocument docPong(jsonPong);
                    //回应心跳包
                    _wsETH->sendTextMessage(QString(docPong.toJson(QJsonDocument::Compact)));
                }
                else if (jsonObj.contains("ch")) {
                    //是数据包
                    QString symbol = jsonObj.value("ch").toString();
                    QJsonObject jsonTick = jsonObj.value("tick").toObject();
                    
                    //取最新价格
                    double fClose = jsonTick.value("close").toDouble();
                    QString sClose = QString::number(fClose, 'f', 2);

                    if (symbol == "market.ethcny.detail") {
                        //自己的处理代码
                    }
                }
            }
        }
    } else {
        qDebug() << "Failure";
    }

}

//请求btc的1分钟K线图
void MainWindow::on_btnRealtime_clicked()
{
    //请求数据
    QJsonObject json;
    json.insert("req", "market.btccny.kline.1min");
    json.insert("id", _subID);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(json);

    QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
    QString jsonStr(ba);
    qDebug() << jsonStr;

    _wsBTC->sendTextMessage(jsonStr);
}

//请求btc的5分钟K线图
void MainWindow::on_btn5Min_clicked()
{
    //请求数据
    QJsonObject json;
    json.insert("req", "market.btccny.kline.5min");
    json.insert("id", _subID);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(json);

    QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
    QString jsonStr(ba);
    qDebug() << jsonStr;

    _wsBTC->sendTextMessage(jsonStr);
}

