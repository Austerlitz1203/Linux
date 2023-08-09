#pragma once
#include<iostream>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

enum
{
    SOCKET_ERR=1,
    BIND_ERR,
    LISTEN_ERR,
    ACCEPT_ERR,
    USAGE_ERR,
    CONNECT_ERR,
    EPOLL_CREATE_ERR,
};