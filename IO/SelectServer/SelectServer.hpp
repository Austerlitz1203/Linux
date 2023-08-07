#pragma once

#include<iostream>
#include"Sock.hpp"
#include"err.hpp"


typedef int type_t;

const static int N = (sizeof(fd_set)*8);
const static int defaultfd = -1;
const static int defaultport = 8888;

class SelectServer
{
public:
    SelectServer(uint16_t port = defaultport)
        :port_(port)
    {}

    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();

        for(int i=0;i<N;i++)
        {
            fdarray_[i] = defaultfd;
        }

        std::cout<<"InitServer done"<<std::endl;
    }


    void Start()
    {
        // 1. 这里我们能够直接获取新的链接吗？
        // 2. 最开始的时候，服务器是没有太多的sock的，甚至只有一个sock！listensock
        // 3. 在网络中， 新连接到来被当做 读事件就绪！
        // listensock_.Accept(); 不能！

        fdarray_[0] = listensock_.Fd();
        while (true)
        {
            // struct timeval timeout = {0, 0};
            // 因为rfds是一个输入输出型参数，注定了每次都要对rfds进行重置，所以要知道我历史上都有哪些fd?fdarray_[]
            // 因为服务器在运行中，sockfd的值一直在动态变化，所以maxfd也一定在变化， maxfd也要进行动态更新， fdarray_[]
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = fdarray_[0];
            // 重置 rfds
            for (int i = 0; i < N; i++)
            {
                if (fdarray_[i] == defaultfd)
                    continue;
                // 合法fd
                FD_SET(fdarray_[i], &rfds);
                if (maxfd < fdarray_[i])
                    maxfd = fdarray_[i];
            }
            // select 并根据结果来进行不同的输出
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
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
                HandlerEvent(rfds);
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
            if (fdarray_[i] == defaultfd)
                continue;
            std::cout << fdarray_[i] << " ";
        }
        std::cout << "\n";
    }

    ~SelectServer()
    {
        listensock_.Close();
    }
private:
    void Accepter()
    {
        // std::cout << "有一个新连接到来了" << std::endl;
        // 这里在进行Accept不会被阻塞
        std::string clientip;
        uint16_t clientport;
        int sock = listensock_.Accept(&clientip, &clientport);
        if (sock < 0)
            return;
        // 得到了对应的sock, 不可以进行read/recv，读取sock
        // 因为sock上不一定有数据就绪。所以需要将sock交给select，让select进行管理！
        logMessage(Debug, "[%s:%d], sock: %d", clientip.c_str(), clientport, sock);
        // 要让select 帮我们进行关心，只要把sock添加到fdarray_[]里面即可！
        int pos = 1;
        for (; pos < N; pos++)
        {
            if (fdarray_[pos] == defaultfd)
                break;
        }
        if (pos >= N)
        {
            close(sock);
            logMessage(Warning, "sockfd array[] full");
        }
        else
        {
            fdarray_[pos] = sock;
        }
    }

    void ServiceIO(const int i)
    {
        int fd = fdarray_[i];
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
            close(fdarray_[i]);
            fdarray_[i] = defaultfd;
        }
    }

    void HandlerEvent(fd_set &rfds)
    {
        for (int i = 0; i < N; i++)
        {
            if (fdarray_[i] == defaultfd)
                continue;

            // _listensock 就绪 并且在rfds中已经被设置为1
            if ((fdarray_[i] == listensock_.Fd()) && FD_ISSET(listensock_.Fd(), &rfds))
            {
                Accepter();
            }
            else if ((fdarray_[i] != listensock_.Fd()) && FD_ISSET(fdarray_[i], &rfds))
            { // 非_listensock 就绪，那就是其他的 fd 就绪，有数据，需要读取
                ServiceIO(i);
            }
        }
    }

private:
    uint16_t port_;
    Sock listensock_;
    type_t fdarray_[N]; // 管理所有的 fd
};
