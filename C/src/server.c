#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <memory.h>
#include <stdlib.h>


struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};


int startup(u_short *port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("fd = %d\n", fd);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(*port); // select a port by the system if zero
    saddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("bind");
        close(fd);
        return -1;
    }
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t saddrlen = sizeof(saddr);
        if (getsockname(fd, (struct sockaddr *)&saddr, &saddrlen) == -1)
        {
            perror("getsockname");
            return -1;
        }
        *port = ntohs(saddr.sin_port);
    }
    if (listen(fd, 128) == -1)
    {
        perror("listen");
        close(fd);
        return -1;
    }
    return fd;
}


void* working(void *arg)
{
    struct SockInfo *pinfo = (struct SockInfo*)arg;
    char client_ip[32];
    printf("client IP: %s, port: %d\n", 
        inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
        ntohs(pinfo->addr.sin_port));
    while (1)
    {
        char buff[1024];
        int len = recv(pinfo->fd, buff, sizeof(buff), 0);
        if (len > 0)
        {
            printf("client say: %s\n", buff);
            send(pinfo->fd, buff, len, 0);
        }
        else if (len == 0)
        {
            printf("disconnect with the client...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
    }
    close(pinfo->fd);
    free(pinfo);
    return NULL;
}


int main()
{
    u_short port = 9999; // select a port by the system if zero
    int fd = startup(&port);
    if (fd == -1)
    {
        perror("startup");
        return -1;
    }
    
    while (1)
    {
        struct SockInfo* pinfo = (struct SockInfo*)malloc(sizeof(struct SockInfo));
        socklen_t caddr_len = sizeof(pinfo->addr);
        pinfo->fd = accept(fd, (struct sockaddr*)&pinfo->addr, &caddr_len);
        printf("fd = %d\n", pinfo->fd);

        if (pinfo->fd == -1)
        {
            perror("accept");
            continue;
        }
        pthread_t pid;
        pthread_create(&pid, NULL, working, pinfo);
        pthread_detach(pid);
    }
    close(fd);

    return 0;
}

