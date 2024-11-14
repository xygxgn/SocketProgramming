#include "TcpSocket.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include <mutex>

std::mutex rdset_mtx;
std::shared_ptr<ThreadPool> pool(new ThreadPool(2, 8));

void communicate(std::shared_ptr<TcpSocket> tcpsock, fd_set *rdset, int maxfd)
{
    while (!pool->isShotdown())
    {
        int cfd = tcpsock->getfd();
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        
        rdset_mtx.lock();
        fd_set tmpset = *rdset;
        rdset_mtx.unlock();

        int num = select(maxfd + 1, &tmpset, NULL, NULL, &timeout);

        if (FD_ISSET(cfd, &tmpset))
        {
            std::string msg;
            if (!tcpsock->recvMsg(msg))
            {
                std::cout << "disconnect with the client..." << std::endl;
                rdset_mtx.lock();
                FD_CLR(cfd, rdset);
                rdset_mtx.unlock();
                break;
            }
            else
            {
                std::cout << "client say: " << msg << std::endl;
                tcpsock->sendMsg(msg);
            }
        }
    }
}


int main()
{
    std::shared_ptr<TcpServer> server(new TcpServer);
    unsigned short port = 9999; // select a port by the system if zero
    if (!server->setListen(port))
        return -1;

    int lfd = server->getfd();
    int maxfd = lfd;
    fd_set rdset, tmpset;
    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    pool->addTask([&](){
        while (!pool->isShotdown())
        {
            rdset_mtx.lock();
            tmpset = rdset;
            rdset_mtx.unlock();
            int num = select(maxfd + 1, &tmpset, NULL, NULL, &timeout);
            if (!pool->isShotdown() && num > 0 && FD_ISSET(lfd, &tmpset))
            {
                std::shared_ptr<TcpSocket> tcpsock(server->acceptConnect());
                if (tcpsock == nullptr)
                    continue;
                int cfd = tcpsock->getfd();
                std::lock_guard<std::mutex> locker(rdset_mtx);
                FD_SET(cfd, &rdset);
                maxfd = cfd > maxfd ? cfd : maxfd;
                pool->addTask(communicate, tcpsock, &rdset, maxfd);
            }
        }
    });

    getchar();
    
    return 0;
}

