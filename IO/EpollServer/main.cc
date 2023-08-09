#include<iostream>
#include<memory>
#include"EpollServer.hpp"

using namespace std;

string echoServer(std::string r)
{
    std::string resp = r;
    resp += "[echo]\r\n";

    return resp;
}

int main()
{
    std::unique_ptr<EpollServer>  es(new EpollServer(echoServer));
    es->InitServer();
    es->start();

    return 0;
}