#pragma once
#include "Sock.hpp"
#include "Log.hpp"
#include "err.hpp"
#include "Epoller.hpp"
#include <sys/epoll.h>
#include <cassert>
#include<functional>

static const uint16_t defaultport = 8888;


class EpollServer
{
    const static int gnum = 64;
    using func_t = std::function<std::string(std::string)>;
public:
    EpollServer(func_t func,uint16_t port = defaultport)
        : port_(port)
        ,func_(func)
    {
    }

    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
        epoller_.Create();
        logMessage(Debug, "init server success!");

        //
    }

    void start()
    {
        // listensock_ 添加到 epoll中
        bool r = epoller_.AddEvent(listensock_.Fd(), EPOLLIN);
        assert(r);
        (void)r;

        int timeout = -1;
        while (true)
        {
            int n = epoller_.wait(revs_, gnum, timeout);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout...");
                break;
            case -1:
                logMessage(Warning, "epoll_wait failed");
                break;
            default:
                logMessage(Debug, "有%d个事件就绪了", n);
                HandlerEvents(n);
                break;
            }
        }
    }

    void Accepter()
    {
        uint16_t clientport;
        std::string clientip;
        int sock = listensock_.Accept(&clientip, &clientport);
        if (sock < 0)
            return;
        logMessage(Debug, "%s:%d 已经连上了服务器了", clientip.c_str(), clientport);

        bool r = epoller_.AddEvent(sock, EPOLLIN);
        assert(r);
        (void)r;
    }

    void ServiceIO(int fd)
    {
        char request[1024];
        ssize_t s = recv(fd, request, sizeof(request) - 1, 0);
        if (s > 0)
        {
            request[s - 1] = 0; // \r\n
            request[s - 2] = 0; // \r\n

            std::string response = func_(request);

            send(fd, response.c_str(), response.size(), 0);
        }
        else
        {
            if (s == 0)
                logMessage(Info, "client quit ...");
            else
                logMessage(Warning, "recv error, client quit ...");
            // 在处理异常的时候，先从epoll中移除，然后再关闭
            epoller_.DelEvent(fd);
            close(fd);
        }
    }

    void HandlerEvents(int num)
    {
        for (int i = 0; i < num; i++)
        {
            int fd = revs_[i].data.fd;
            uint32_t events = revs_[i].events;

            logMessage(Debug, "当前正在处理%d上的%s", fd, (events & EPOLLIN) ? "EPOLLIN" : "OTHER");

            if (events & EPOLLIN)
            {
                if (fd == listensock_.Fd()) // 新连接
                {
                    Accepter();
                }
                else // 读取数据
                {
                    ServiceIO(fd);
                }
            }
        }
    }

    ~EpollServer()
    {
        listensock_.Close();
        epoller_.Close();
    }

private:
    uint16_t port_;
    Sock listensock_;
    Epoller epoller_;
    struct epoll_event revs_[gnum];
    func_t func_;
};