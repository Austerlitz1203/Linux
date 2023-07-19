#pragma once
#include <iostream>
#include <string>
#include<unistd.h>
#include<functional>
using namespace std;

using func_t = function<void(int,string,uint16_t)>;

class Task
{
public:
    Task()
    {}

    Task(int sock,const string ip,const uint16_t port,func_t func)
        :sock_(sock)
        ,ip_(ip)
        ,port_(port)
        ,func_(func)
    {
    }

    void operator()()
    {
        func_(sock_,ip_,port_);
    }

    ~Task()
    {
    }

private:
    int sock_;
    string ip_;
    uint16_t port_;
    func_t func_;
};