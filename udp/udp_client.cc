#include "udp_client.hpp"
#include "err.hpp"

int main(int argc,char* argv[])
{
    u_int16_t serverport=atoi(argv[2]);
    std::string serverip=argv[1];
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(SOCKET_ERR);
    }

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(serverport);
    server.sin_addr.s_addr=inet_addr(serverip.c_str());

    while(true)
    {
        std::string message;
        cout<<"please Enter message:";
        cin>>message;

        sendto(sock,message.c_str(),message.size(),0,(const sockaddr*)&server,sizeof(server));
    }


    return 0;
}