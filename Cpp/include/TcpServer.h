#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H


class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    bool setListen(unsigned short &port);
    TcpSocket *acceptConnect(sockaddr_in *addr);
private:
    int fd;
};


#endif
