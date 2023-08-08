#include"PollServer.hpp"
#include<iostream>
#include<memory>

using namespace std;

int main()
{
    std::shared_ptr<PollServer> ss(new PollServer());
    ss->InitServer();
    ss->Start();

    return 0;
}