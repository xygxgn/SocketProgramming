#ifndef __TCP_SOCKET_H
#define __TCP_SOCKET_H

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <memory.h>


class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int _cfd);
    ~TcpSocket();
    bool connectToHost(const std::string &ip, const unsigned short port);
    bool sendMsg(const std::string &msg);
    bool recvMsg(std::string &msg);
private:
    int writen(const char * buff, int size);
    int readn(char *data, int size);
    int cfd;
};

#endif
