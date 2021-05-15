#include "server.h"

server::server(QObject* parent): QTcpServer(parent)
{
    hostname = "Imhost";
    p_server = new QTcpServer(this);
    isconnected = false;
}

server::~server(){}


