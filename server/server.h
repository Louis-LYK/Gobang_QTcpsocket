#ifndef SERVER_H
#define SERVER_H

#include "stdafx.h"

class server: public QTcpServer
{
    Q_OBJECT
public:
    server(QObject* parent = nullptr);
    ~server();

public:
    QTcpServer* p_server;
    QTcpSocket* p_socket;
    QString hostname;
    bool isconnected;
};

#endif // SERVER_H
