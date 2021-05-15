#ifndef CLIENT_H
#define CLIENT_H

#include "stdafx.h"

class client: public QObject
{
    Q_OBJECT
public:
    explicit client();
    ~client();

public:
    QString clnt_name;
    QTcpSocket* p_connect;
    bool isconnected;
};

#endif // CLIENT_H
