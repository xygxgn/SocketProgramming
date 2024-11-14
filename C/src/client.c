#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "string.h"


int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("fd = %d\n", fd);
    if (fd == -1)
    {
        perror("socket");
        close(fd);
        return -1;
    }
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);
    if (connect(fd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        close(fd);
        return -1;
    }

    int count = 0;
    while (1)
    {
        char buff[1024];
        sprintf(buff, "hello, world! %d...\n", count++);
        send(fd, buff, strlen(buff) + 1, 0);
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        if (len > 0)
        {
            printf("server say: %s\n", buff);
        }
        else if (len == 0)
        {
            printf("disconnect with the server...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
        sleep(1);
    }
    close(fd);

    return 0;
}