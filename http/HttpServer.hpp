#pragma once

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include "Sock.hpp"

static const uint16_t defaultport = 8888;
class HttpServer;

using func_t = std::function<string(string &)>;

class ThreadData
{
public:
    ThreadData(int sock, const string &ip, const uint16_t &port, HttpServer *tsvrp)
        : _sock(sock), _ip(ip), _port(port), _tsvrp(tsvrp)
    {
    }
    ~ThreadData() {}

public:
    int _sock;
    string _ip;
    uint16_t _port;
    HttpServer *_tsvrp;
};

class HttpServer
{
public:
    HttpServer(func_t f, int port = defaultport) : func(f), port_(port)
    {
    }
    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
    }
    void HandlerHttpRequest(int sock)
    {
        char buffer[4096];
        string request;
        ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0); // 假设一次读完
        if (s > 0)
        {
            buffer[s] = 0;
            request = buffer;
            string response = func(request);
            send(sock, response.c_str(), response.size(), 0);
        }
        else
        {
            logMessage(Info, "client quit...");
        }
    }

    static void *threadRoutine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);
        td->_tsvrp->HandlerHttpRequest(td->_sock);
        close(td->_sock);
        delete td;
        return nullptr;
    }

    void Start()
    {
        for (;;)
        {
            std::string clientip;
            uint16_t clientport;
            int sock = listensock_.Accept(&clientip, &clientport);
            if (sock < 0)
                continue;

            pthread_t tid;
            ThreadData *td = new ThreadData(sock, clientip, clientport, this);
            pthread_create(&tid, nullptr, threadRoutine, td);
        }
    }
    ~HttpServer()
    {
    }

private:
    int port_;
    Sock listensock_;
    func_t func;
};