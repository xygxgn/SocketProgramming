#include "TcpSocket.h"


int main()
{
    TcpSocket client_sock;
    client_sock.connectToHost("127.0.0.1", 9999);

    int count = 0;
    while (1)
    {
        std::string msg = std::string("hello, world! ") + std::to_string(count++) + "...\n";
        client_sock.sendMsg(msg);
        if (client_sock.recvMsg(msg))
        {
            std::cout << msg << std::endl;
        }
        else
        {
            std::cout << "disconnect with the server..." << std::endl;
            break;
        }
        sleep(1);
    }

    return 0;
}
