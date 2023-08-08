#pragma once

#include <iostream>
#include<sys/poll.h>
#include "Sock.hpp"
#include "err.hpp"

const static int N = 4096;
const static int defaultfd = -1;
const static int defaultport = 8888;
const static short defaultevent = 0;

typedef struct pollfd type_t;

class PollServer
{
public:
    PollServer(uint16_t port = defaultport)
        : port_(port)
        ,fdarray_(nullptr)
    {
        fdarray_ = new type_t[N];
    }

    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();

        for (int i = 0; i < N; i++)
        {
            fdarray_[i].fd = defaultfd;
            fdarray_[i].events=defaultevent;
            fdarray_[i].revents=defaultevent;
        }

        std::cout << "InitServer done" << std::endl;
    }

    void Start()
    {
        // 1. 这里我们能够直接获取新的链接吗？
        // 2. 最开始的时候，服务器是没有太多的sock的，甚至只有一个sock！listensock
        // 3. 在网络中， 新连接到来被当做 读事件就绪！
        // listensock_.Accept(); 不能！

        fdarray_[0].fd = listensock_.Fd();
        fdarray_[0].events=POLLIN;
        while (true)
        {
            // select 并根据结果来进行不同的输出
            int timeout = -1;
            int n = poll(fdarray_,N,timeout);
            switch (n)
            {
            case 0:
                logMessage(Debug, "timeout, %d: %s", errno, strerror(errno));
                break;
            case -1:
                logMessage(Warning, "%d: %s", errno, strerror(errno));
                break;
            default:
                // 成功了
                logMessage(Debug, "有一个就绪事件发生了: %d", n);
                HandlerEvent();
                DebugPrint();
                break;
            }

            // sleep(1);
        }
    }
    void DebugPrint()
    {
        std::cout << "fdarray[]: ";
        for (int i = 0; i < N; i++)
        {
            if (fdarray_[i].fd == defaultfd)
                continue;
            std::cout << fdarray_[i].fd << " ";
        }
        std::cout << "\n";
    }

    ~PollServer()
    {
        listensock_.Close();
        if(fdarray_) delete[] fdarray_;
    }

private:
    void Accepter()
    {
        std::string clientip;
        uint16_t clientport;
        int sock = listensock_.Accept(&clientip, &clientport);
        if (sock < 0)
            return;
        // 得到了对应的sock, 不可以进行read/recv，读取sock
        // 因为sock上不一定有数据就绪。所以需要将sock交给select，让select进行管理！
        logMessage(Debug, "[%s:%d], sock: %d", clientip.c_str(), clientport, sock);

        int pos = 1;
        for (; pos < N; pos++)
        {
            if (fdarray_[pos].fd == defaultfd)
                break;
        }
        if (pos >= N)
        {
            close(sock);
            logMessage(Warning, "sockfd array[] full");
        }
        else
        {
            fdarray_[pos].fd = sock;
            fdarray_[pos].events = POLLIN; // 关心读事件
            fdarray_[pos].revents = defaultevent;
        }
    }

    void ServiceIO(const int i)
    {
        int fd = fdarray_[i].fd;
        char buffer[1024];
        ssize_t s = recv(fd, buffer, sizeof(buffer) - 1, 0); // 读取会被阻塞吗？不会
        if (s > 0)
        {
            buffer[s - 1] = 0;
            std::cout << "client# " << buffer << std::endl;

            // 发送回去也要被select管理的,TODO
            std::string echo = buffer;
            echo += " [select server echo]";
            send(fd, echo.c_str(), echo.size(), 0);
        }
        else
        {
            if (s == 0)
                logMessage(Info, "client quit ..., fdarray_[i] -> defaultfd: %d->%d", fd, defaultfd);
            else
                logMessage(Warning, "recv error, client quit ..., fdarray_[i] -> defaultfd: %d->%d", fd, defaultfd);
            close(fd);
            fdarray_[i].fd = defaultfd;
            fdarray_[i].events=defaultevent;
            fdarray_[i].revents=defaultevent;
        }
    }

    void HandlerEvent()
    {
        for (int i = 0; i < N; i++)
        {
            int fd = fdarray_[i].fd;
            if (fd == defaultfd)
                continue;
            if (fdarray_[i].revents & POLLIN) // 读事件就绪
            {
                // _listensock 就绪 并且在rfds中已经被设置为1
                if (fd == listensock_.Fd())
                {
                    Accepter();
                }
                else if (fd != listensock_.Fd())
                { // 非_listensock 就绪，那就是其他的 fd 就绪，有数据，需要读取
                    ServiceIO(i);
                }
            }
            else if(fdarray_[i].revents & POLLOUT) // 写事件就绪
            {

            }
        }
    }

private:
    uint16_t port_;
    Sock listensock_;
    type_t* fdarray_; // 管理所有的 fd
};
