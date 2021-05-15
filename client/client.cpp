#include "client.h"

client::client()
{
    clnt_name = "Imclnt";
    p_connect = new QTcpSocket(this);
}

client::~client() {}
