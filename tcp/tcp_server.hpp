#pragma once

#include "err.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include "Log.hpp"

#include <iostream>
#include <string>
#include <sys/types.h> /* See NOTES */
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <functional>

namespace ns_server
{
    static const int backlog = 32;
    using func_t = std::function<std::string(const std::string &)>;

    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(int sock, string ip, uint16_t port, TcpServer *ts)
            : sock_(sock), ip_(ip), port_(port), current(ts)
        {
        }

    public:
        int sock_;
        string ip_;
        uint16_t port_;
        TcpServer *current;
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, const uint16_t port)
            : port_(port), func_(func)
        {
        }

        void InitServer()
        {
            // 1.创建 socket
            listensock_ = socket(AF_INET, SOCK_STREAM, 0);
            if (listensock_ < 0)
            {
                // cerr << "create socket error" << strerror(errno) << endl;
                logMessage(Fatal, "create socket error, code: %d, error string: %s", errno, strerror(errno));
                exit(SOCKET_ERR);
            }
            //cout << "create socket success:" << listensock_ << endl;
            logMessage(Info, "create socket success, code: %d, error string: %s", errno, strerror(errno));

            // 2. bind
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port_);
            local.sin_addr.s_addr = htonl(INADDR_ANY);

            if (bind(listensock_, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                //cerr << "bind error " << strerror(errno) << endl;
                logMessage(Fatal, "bind socket error, code: %d, error string: %s", errno, strerror(errno));
                exit(BIND_ERR);
            }
            //cout << "bind success" << endl;
            logMessage(Info, "bind socket success, code: %d, error string: %s", errno, strerror(errno));

            // 3. lsiten
            if (listen(listensock_, backlog) < 0)
            {
                logMessage(Fatal, "listen socket error, code: %d, error string: %s", errno, strerror(errno));
                exit(LISTEN_ERR);
            }
            logMessage(Fatal, "listen socket error, code: %d, error string: %s", errno, strerror(errno));
        }

        void start()
        {
            // signal(SIGCHLD,SIG_IGN);

            quit_ = false;
            while (!quit_)
            {
                // 4. accept
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int sock = accept(listensock_, (struct sockaddr *)&client, &len);
                if (sock < 0)
                {
                    //cerr << "accept error " << strerror(errno) << endl;
                    logMessage(Warning, "accept error, code: %d, error string: %s", errno, strerror(errno));
                    exit(ACCEPT_ERR);
                }

                string clientip = inet_ntoa(client.sin_addr);
                uint16_t clientport = ntohs(client.sin_port);

                // 5. 开始业务处理
                logMessage(Info, "获取新连接成功: %d from %d, who: %s - %d",
                           sock, listensock_, clientip.c_str(), clientport);
                // Version 1
                // service(sock,clientip,clientport);

                // version2 多进程  父进程只负责建立好连接，子进程来处理
                // pid_t id=fork();
                // if(id < 0)
                // {
                //     close(sock);
                //     continue;
                // }
                // else if(id == 0) // 子进程
                // {
                //     // 子进程只需要处理，所以可以关闭不需要的 fd
                //     close(listensock_);
                //     if(fork() > 0) exit(0);  // 这一行代码解决阻塞等待的问题，子进程退出，孙子进程执行，所以父进程很快就可以等待到

                //     // 孙子进程在执行后面的代码（孤儿进程，被系统领养）
                //     service(sock,clientip,clientport);
                //     exit(0);
                // }
                // // 父进程一定要关闭不需要的 fd
                // close(sock);
                // pid_t ret = waitpid(id, nullptr, 0);  // 阻塞等待，需要进行优化，WAINOHANG并不推荐，没有新连接就无法走到 waitpid 这里
                // if (ret == id)
                //     std::cout << "wait child " << id << " success" << std::endl;

                // Version3 多线程

                // pthread_t tid;
                // ThreadData *td = new ThreadData(sock, clientip, clientport, this);
                // pthread_create(&tid, nullptr, threadRoutine, td);

                // Version4 线程池
                Task t(sock, clientip, clientport, std::bind(&TcpServer::service, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                ThreadPool<Task>::GetInstance()->pushTask(t);
            }
        }

        static void *threadRoutine(void *args)
        {
            pthread_detach(pthread_self());

            ThreadData *td = static_cast<ThreadData *>(args);
            td->current->service(td->sock_, td->ip_, td->port_);
            delete td;
            return nullptr;
        }

        void service(int sock, string ip, uint16_t port)
        {
            string name = ip + "-" + std::to_string(port);
            char buffer[1024];
            // 线程池版本 只需要提供一次服务，不然线程会持续为某个用户服务，降低了效率
            ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                string response = func_(buffer);
                cout << name << ":" << response << endl;

                write(sock, response.c_str(), response.size());
            }
            else if (n == 0) // 对方关闭连接
            {
                close(sock);
                logMessage(Info, "%s quit,me too", name.c_str());
            }
            else if (n < 0)
            {
                close(sock);
                logMessage(Error, "read error, %d:%s", errno, strerror(errno));
            }

            close(sock);

            // while (true)
            // {
            //     ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
            //     if (n > 0)
            //     {
            //         buffer[n] = 0;
            //         string response = func_(buffer);
            //         cout << name << ":" << response << endl;

            //         write(sock, response.c_str(), response.size());
            //     }
            //     else if (n == 0) // 对方关闭连接
            //     {
            //         close(sock);
            //         cout << name << " quit,me too" << endl;
            //         break;
            //     }
            //     else if (n < 0)
            //     {
            //         close(sock);
            //         cerr << "read error" << strerror(errno) << endl;
            //         break;
            //     }
            // }
        }

    private:
        uint16_t port_;
        int listensock_;
        bool quit_;
        func_t func_;
    };
}