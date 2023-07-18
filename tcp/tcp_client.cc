#include"err.hpp"
#include<iostream>
#include<cstring>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<unistd.h>

using namespace std;

static void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " serverip serverport\n"<< endl;
}


int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    string serverip=argv[1];
    uint16_t serverport=atoi(argv[2]);

    // 1. create socket
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        cerr<<"client sock error "<<strerror(errno)<<endl;
        exit(SOCKET_ERR);
    }

    // 2. connect   不用自己 bind，不用 lisetn、accept
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    inet_aton(serverip.c_str(), &(server.sin_addr));
    server.sin_port=htons(serverport);

    int cnt=5;
    while(connect(sock,(const struct sockaddr*)&server,sizeof(server)) != 0)
    {
        cerr<<"connect error "<<strerror(errno)<<endl;
        cout<<"尝试重连，还有 "<<cnt--<<"次机会"<<endl;

        if(cnt == 0) break;
    }

    if(cnt<=0)
    {
        cout<<"连接失败！"<<endl;
        exit(CONNECT_ERR);
    }

    char buffer[1024];
    // 3. 连接成功
    while (true)
    {
        string line;
        cout << "Enter>>> ";
        getline(cin, line);

        write(sock, line.c_str(), line.size());

        ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s] = 0;
            cout << "server echo >>>" << buffer << endl;
        }
        else if (s == 0)
        {
            cerr << "server quit" << endl;
            break;
        }
        else
        {
            cerr << "read error: " << strerror(errno) << endl;
            break;
        }
    }
    close(sock);

    return 0;
}