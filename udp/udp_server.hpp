#pragma once
#include <iostream>
#include<functional>
#include<unordered_map>
#include<cstring>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "err.hpp"
#include"LockGuard.hpp"
#include"RingQueue.hpp"
#include"Thread.hpp"

namespace us_server
{

    const static u_int16_t defaultport = 8080;
    using func_t = std::function<string(string)>;

    class UdpServer
    {
    public:
        UdpServer(const uint16_t port = defaultport)
            : port_(port)
        {
            pthread_mutex_init(&lock_, nullptr);

            recv_ = new Thread(1,std::bind(&UdpServer::Recv,this));
            broad_ = new Thread(2,std::bind(&UdpServer::BroadCast,this));
        }

        void Start()
        {
            // 1. 创建 socket 接口，打开网络文件
            sock_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_ < 0)
            {
                cerr << "create socket error:" << strerror(errno) << endl;
                exit(SOCKET_ERR);
            }
            cout << "create socket success!" << endl;

            // 2. 给服务器指明 ip 地址（？） 和 port
            struct sockaddr_in local;
            bzero(&local, sizeof(local));

            local.sin_family = AF_INET;
            // 1. 字符串风格的IP地址，转换成为4字节int, "1.1.1.1" -> uint32_t -> 不能强制类型转换
            local.sin_addr.s_addr = INADDR_ANY;
            // 2. 主机序列转化为网络序列
            local.sin_port = htons(port_);

            if (bind(sock_, (const sockaddr *)&local, sizeof(local)) < 0)
            {
                cerr << "bind error:" << strerror(errno) << endl;
                exit(BIND_ERR);
            }
            cout << "bind success:" << sock_ << endl;

            recv_->run();
            broad_->run();
        }

        void addUser(const string &name, const struct sockaddr_in &peer)
        {
            //?
            // Users[name] = peer;
            LockGuard lockguard(&lock_);
            auto iter = Users_.find(name);
            if (iter != Users_.end())
                return;
            Users_.insert(make_pair(name, peer));
        }

        void Recv()
        {
            char buffer[1024];
            while (true)
            {
                // 收
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer); // 这里一定要写清楚，未来传入的缓冲区大小
                int n = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
                if (n > 0)
                    buffer[n] = '\0';
                else
                    continue;


                // 提取client信息 -- debug
                string clientip = inet_ntoa(peer.sin_addr);
                uint16_t clientport = ntohs(peer.sin_port);
                cout << clientip << "-" << clientport << "# " <<buffer << std::endl;

                // 构建一个用户，并检查
                string name = clientip;
                name += "-";
                name += std::to_string(clientport);

                // 如果不存在，就插入，如果存在，什么都不做
                addUser(name, peer);
                rq_.push(buffer);
            }
        }


        void BroadCast()
        {
            while (true)
            {
                // 拿到要广播的消息
                string sendstring;
                rq_.pop(&sendstring);

                // 要广播给哪些 user
                std::vector<struct sockaddr_in> v;
                {
                    LockGuard lockguard(&lock_);
                    for (auto user : Users_)
                    {
                        v.push_back(user.second);
                    }
                }

                 // 广播
                for (auto user : v)
                {
                    // cout << "Broadcast message to " << user.first << sendstring << endl;
                    sendto(sock_, sendstring.c_str(), sendstring.size(), 0, (struct sockaddr *)&(user), sizeof(user));
                    cout << "send done ..." << sendstring << endl;
                }
            }
        }

        ~UdpServer()
        {
            pthread_mutex_destroy(&lock_);
            recv_->join();
            broad_->join();
            delete recv_;
            delete broad_;
        }

    private:
        int sock_;
        uint16_t port_;
        //func_t service_;
        std::unordered_map<string,struct sockaddr_in> Users_;
        pthread_mutex_t lock_;
        RingQueue<string> rq_;
        Thread * recv_;  // 接收线程
        Thread * broad_; // 广播线程
        //string ip_;
    };
}
