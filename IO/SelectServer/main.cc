#include"SelectServer.hpp"
#include<iostream>
#include<memory>

using namespace std;

int main()
{
    std::shared_ptr<SelectServer> ss(new SelectServer());
    ss->InitServer();
    ss->Start();

    return 0;
}