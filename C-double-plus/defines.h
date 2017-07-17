#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

using namespace std;


const QString HUOBI_WSURL_BTC = "wss://api.huobi.com/ws";
const QString HUOBI_WSURL_ETH = "wss://be.huobi.com/ws";

//REST Url
const QString HUOBI_RESTURL = "https://api.huobi.com/apiv3";
//REST Header
const QString HUOBI_REST_HEADER_CONTENTTYPE = "application/x-www-form-urlencoded";

extern QString G_Huobi_AccessKey;
extern QString G_Huobi_SecretKey;


#endif // DEFINES_H
