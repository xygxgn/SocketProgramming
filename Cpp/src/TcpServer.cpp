#include "TcpSocket.h"
#include "TcpServer.h"
#include <fcntl.h>


TcpServer::TcpServer()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    // set the socket to non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        close(fd);
    }
}

TcpServer::~TcpServer()
{
    if (fd != -1)
        close(fd);
}


bool TcpServer::setListen(unsigned short &port)
{
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port); // select a port by the system if zero
    saddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("bind");
        return false;
    }
    if (port == 0)  /* if dynamically allocating a port */
    {
        socklen_t saddrlen = sizeof(saddr);
        if (getsockname(fd, (struct sockaddr *)&saddr, &saddrlen) == -1)
        {
            perror("getsockname");
            return -1;
        }
        port = ntohs(saddr.sin_port);
    }
    char server_ip[32];
    std::cout << "socket bind successfully!\n"
        << "ip: " << inet_ntop(AF_INET, &saddr.sin_addr.s_addr, server_ip, sizeof(server_ip))
        << ", port: " << port << std::endl;
    if (listen(fd, 128) == -1)
    {
        perror("listen");
        return false;
    }
    return true;
}


TcpSocket *TcpServer::acceptConnect()
{
    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int cfd = accept(fd, (struct sockaddr*)&addr, &addrlen);

    if (cfd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return nullptr;
        } else {
            perror("accept");
            return nullptr;
        }
    }

    char client_ip[32];
    printf("client IP: %s, port: %d\n", 
        inet_ntop(AF_INET, &addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
        ntohs(addr.sin_port));

    return new TcpSocket(cfd);
}
