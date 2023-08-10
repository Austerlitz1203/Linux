#pragma once
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

#include "err.hpp"
#include"Log.hpp"

static const int gbacklog = 32;
static const int defaultsock = -1;

class Sock
{
public:
    Sock()
        : _sock(defaultsock)
    {
    }

    void Socket()
    {
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {

            exit(SOCKET_ERR);
        }
    }

    void Bind(const uint16_t &port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(_sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(Fatal, "bind error, code: %d, errstring: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
    }

    void Listen()
    {
        if (listen(_sock, gbacklog) < 0)
        {
            logMessage(Fatal, "listen error, code: %d, errstring: %s", errno, strerror(errno));
            exit(LISTEN_ERR);
        }
    }

    int Accept(string *clientip, uint16_t *clientport,int *err)
    {
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        int sock = accept(_sock, (struct sockaddr *)&temp, &len);
        *err = errno;
        if (sock < 0)
        {
            logMessage(Warning, "accept error, code: %d, errstring: %s", errno, strerror(errno));
        }
        else
        {
            *clientip = inet_ntoa(temp.sin_addr);
            *clientport = ntohs(temp.sin_port);
        }
        return sock;
    }

    int Connect(const string &serverip, const uint16_t &serverport)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(serverport);
        server.sin_addr.s_addr = inet_addr(serverip.c_str());

        return connect(_sock, (struct sockaddr *)&server, sizeof(server));
    }

    int Fd()
    {
        return _sock;
    }

    void Close()
    {
        close(_sock);
    }

    ~Sock()
    {
    }

private:
    int _sock;
};