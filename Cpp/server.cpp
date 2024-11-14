#include "TcpSocket.h"
#include "TcpServer.h"
#include "ThreadPool.h"


void working(std::shared_ptr<ThreadPool> pool, std::shared_ptr<TcpSocket> tcpsock, sockaddr_in *addr)
{
    char client_ip[32];
    printf("client IP: %s, port: %d\n", 
        inet_ntop(AF_INET, &addr->sin_addr.s_addr, client_ip, sizeof(client_ip)),
        ntohs(addr->sin_port));
    while (!pool->isShotdown())
    {
        std::string msg;
        if (!tcpsock->recvMsg(msg))
        {
            std::cout << "disconnect with the client..." << std::endl;
            break;
        }
        else
        {
            std::cout << "client say: " << msg << std::endl;
            tcpsock->sendMsg(msg);
        }
    }
    delete addr;
}


int main()
{
    std::shared_ptr<TcpServer> server(new TcpServer);
    unsigned short port = 9999; // select a port by the system if zero
    if (!server->setListen(port))
        return -1;
    
    std::shared_ptr<ThreadPool> pool(new ThreadPool(2, 8));
    pool->addTask([&](){
        while (!pool->isShotdown())
        {
            sockaddr_in *addr = new sockaddr_in;
            std::shared_ptr<TcpSocket> tcpsock(server->acceptConnect(addr));
            if (tcpsock == nullptr)
            {
                delete addr;
                continue;
            }
            pool->addTask(working, pool, tcpsock, addr);
        }
    });

    getchar();
    
    return 0;
}

