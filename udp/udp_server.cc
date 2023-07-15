#include"udp_server.hpp"

int main()
{
    us_server::UdpServer us("1.15.119.209");
    us.InitServer();


    return 0;
}