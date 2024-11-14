#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H


class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    inline int getfd() { return fd; };
    bool setListen(unsigned short &port);
    TcpSocket *acceptConnect();
private:
    int fd;
};


#endif
