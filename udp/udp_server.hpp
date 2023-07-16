#pragma once
#include <iostream>
#include<functional>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "err.hpp"

namespace us_server
{

    const static u_int16_t defaultport = 8080;
    using func_t = std::function<string(string)>;

    class UdpServer
    {
    public:
        UdpServer(func_t f,const uint16_t port = defaultport)
            : port_(port), service_(f)
        {
        }

        void InitServer()
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
        }

        void StartServer()
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

                // 对消息进行处理
                string response = service_(buffer);

                // 提取client信息 -- debug
                std::string clientip = inet_ntoa(peer.sin_addr);
                uint16_t clientport = ntohs(peer.sin_port);
                std::cout << clientip << "-" << clientport << "# " << endl <<response << std::endl;


                //发
                sendto(sock_, response.c_str(), response.size(), 0, (struct sockaddr *)&(peer), sizeof(peer));
            }
        }

        ~UdpServer()
        {
        }

    private:
        int sock_;
        uint16_t port_;
        func_t service_;
        //string ip_;
    };
}
