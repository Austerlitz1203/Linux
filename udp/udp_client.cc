#include "udp_client.hpp"
#include "err.hpp"

static void usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << std::endl;
}

int main(int argc,char* argv[])
{

    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    u_int16_t serverport=atoi(argv[2]);
    string serverip=argv[1];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        cerr << "create socket error" << endl;
        exit(SOCKET_ERR);
    }

    // 知道 server 是谁
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(serverport);
    server.sin_addr.s_addr=inet_addr(serverip.c_str());


    while(true)
    {
        string message;
        cout << "[leo@VM-4-9-centos leo]$ ";
        //cin>>message;
        getline(cin,message);

        // 发送
        sendto(sock,message.c_str(),message.size(),0,(const sockaddr*)&server,sizeof(server));

        // 接收
        char buffer[2048];
        struct sockaddr_in temp;
        socklen_t len=sizeof(temp);
        int n=recvfrom(sock,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&temp,&len);
        if(n > 0)
        {
            buffer[n]=0;
            cout<<"server echo#"<<buffer<<endl;
        }

    }


    return 0;
}
