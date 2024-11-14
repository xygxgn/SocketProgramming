#include "TcpSocket.h"


TcpSocket::TcpSocket()
{
    cfd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int _cfd) : cfd(_cfd) 
{
    // pass
};


TcpSocket::~TcpSocket()
{
    if (cfd != -1)
        close(cfd);
}

bool TcpSocket::connectToHost(const std::string &ip, const unsigned short port)
{
    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &saddr.sin_addr.s_addr);
    if (connect(cfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        return false;
    }
    return true;
}

bool TcpSocket::sendMsg(const std::string &msg)
{
    char *data = new char[msg.size() + 4];
    int bigLen = htonl(msg.size());
    memcpy(data, &bigLen, 4);
    memcpy(data + 4, msg.c_str(), msg.size());
    if (writen(data, msg.size() + 4) != msg.size() + 4)
    {
        delete[] data;
        return false;
    }
    delete[] data;
    return true;
}

bool TcpSocket::recvMsg(std::string &msg)
{
    int len = 0;
    readn((char*)&len, 4);
    len = ntohl(len);
    char *buff = new char[len + 1];
    if (readn(buff, len) != len || len == 0)
    {
        delete[] buff;
        return false;
    }
    buff[len] = '\0';
    msg = std::string(buff);
    return true;
}


int TcpSocket::writen(const char *data, int size)
{
    int cnt = 0, left = size;
    while (left > 0)
    {
        if ((cnt = send(cfd, data, left, 0)) > 0)
        {
            data += cnt;
            left -= cnt;
        }
        else if (cnt == -1)
        {
            perror("send");
            return -1;
        }
        else
            return -1;
    }
    return size;
}


int TcpSocket::readn(char *buff, int size)
{
    int cnt = 0, left = size;
    while (left > 0)
    {
        if ((cnt = recv(cfd, buff, left, 0)) > 0)
        {
            buff += cnt;
            left -= cnt;
        }
        else if (cnt == -1)
        {
            perror("recv");
            return -1;
        }
        else
            return -1;
    }
    return size;
}
