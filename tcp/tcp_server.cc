#include"tcp_server.hpp"
#include"daemon.hpp"

#include<memory>

using namespace std;
using namespace ns_server;

void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " port\n"
         << endl;
}

string echo(const string &message)
{
    return message;
}

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port=atoi(argv[1]);
    std::unique_ptr<TcpServer> ts(new TcpServer(echo,port));

    ts->InitServer();
    Daemon(); // 服务器守护进程化
    ts->start();

    return 0;
}