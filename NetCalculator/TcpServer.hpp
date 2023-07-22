#pragma once
#include "err.hpp"
#include "Sock.hpp"
#include "Protocol.hpp"
#include "Util.hpp"
#include "Log.hpp"

#include <iostream>
#include <pthread.h>
#include <functional>

using namespace protocol_ns;

namespace tcpserver_ns
{
    using func_t = std::function<Response(const Request &)>;

    class TcpServer;

    class ThreadData
    {
    public:
        ThreadData(int sock, string ip, uint16_t port, TcpServer *tsvrp)
            : _sock(sock), _ip(ip), _port(port), _tsvrp(tsvrp)
        {
        }
        ~ThreadData() {}

    public:
        int _sock;
        string _ip;
        uint16_t _port;
        TcpServer *_tsvrp;
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, uint16_t port)
            : _port(port), _func(func)
        {
        }

        void InitServer()
        {
            // 1. 初始化服务器
            _listensock.Socket();
            _listensock.Bind(_port);
            _listensock.Listen();
            logMessage(Info, "init server done, listensock: %d", _listensock.Fd());
        }

        void Start()
        {
            for (;;)
            {
                string clientip;
                uint16_t clientport;
                int sock = _listensock.Accept(&clientip, &clientport);
                if (sock < 0)
                    continue;
                logMessage(Debug, "get a new client, client info : [%s:%d]", clientip.c_str(), clientport);

                pthread_t tid;
                ThreadData *td = new ThreadData(sock, clientip, clientport, this);
                pthread_create(&tid, nullptr, ThreadRoutine, td);
            }
        }

        static void *ThreadRoutine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadData *td = static_cast<ThreadData *>(args);
            td->_tsvrp->ServiceIO(td->_sock, td->_ip, td->_port);
            logMessage(Debug, "thread running ...");
            delete td;
            return nullptr;
        }

        void ServiceIO(int sock, const std::string &ip, const uint16_t &port)
        {
            string inbuffer;
            while (true)
            {
                string package;
                int n = ReadPackage(sock, inbuffer, &package);
                cout<<"ReadPackageIO:"<< n<<endl;
                if (n == -1)
                    break;
                else if (n == 0)
                    continue;
                else
                {
                    // 1.需要的只是有效载荷
                    package=RemoveHeader(package,n);

                    // 2. 假设已经读到了一个完整的string
                    Request req;
                    req.Deserialize(package); // 对读到的request字符串要进行反序列化

                    // 3. 直接提取用户的请求数据
                    Response resp = _func(req); // 业务逻辑！

                    // 4. 给用户返回响应 - 序列化
                    string send_string;
                    resp.Serialize(&send_string); // 对计算完毕的response结构要进行序列化，形成可发送字符串

                    // 5. 添加报头
                    send_string = AddHeader(send_string);

                    // 6. 发送
                    send(sock, send_string.c_str(), send_string.size(), 0);
                }
            }
        }

        ~TcpServer()
        {
            _listensock.Close();
        }

    private:
        Sock _listensock;
        uint16_t _port;
        func_t _func;
    };
}