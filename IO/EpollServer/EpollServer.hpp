#pragma once
#include "Sock.hpp"
#include "Log.hpp"
#include "err.hpp"
#include "Epoller.hpp"
#include "Util.hpp"
#include "Protocol.hpp"
#include <sys/epoll.h>
#include <cassert>
#include <functional>
#include <unordered_map>

static const uint16_t defaultport = 8888;
static const int bsize = 1024;
class Connection;
class EpollServer;

using callback_t = std::function<void(Connection *)>;
using namespace protocol_ns;

class Connection
{
public:
    Connection(EpollServer* es,int fd)
        : fd_(fd)
        , R_(es)
    {
    }

    void Register(callback_t recver, callback_t sender, callback_t excepter)
    {
        recver_ = recver;
        sender_ = sender;
        excepter_ = excepter;
    }

    ~Connection()
    {
    }

public:
    int fd_;
    std::string inbuffer_;
    std::string outbuffer_;
    std::string clientip_;
    uint16_t clientport_;

    // IO处理函数
    callback_t recver_;
    callback_t sender_;
    callback_t excepter_;

    uint32_t events; // 要关心的事件

    // 回指指针
    EpollServer *R_;
};

class EpollServer
{
    const static int gnum = 64;
    using func_t = std::function<void(Connection *, const Request &)>;

public:
    EpollServer(func_t func, uint16_t port = defaultport)
        : port_(port), func_(func)
    {
    }

    void InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
        epoller_.Create();
        // listensock_ 添加到 epoll中
        AddConnection(listensock_.Fd(), EPOLLIN | EPOLLET);
        logMessage(Debug, "init server success!");
    }

    // 事件派发器
    void Dispatcher()
    {
        int timeout = -1;
        while (true)
        {
            LoopOnce(timeout);
        }
    }

    void LoopOnce(int timeout)
    {
        int n = epoller_.wait(revs_, gnum, timeout);
        for (int i = 0; i < n; i++)
        {
            int fd = revs_[i].data.fd;
            uint32_t events = revs_[i].events;

            logMessage(Debug, "当前正在处理%d上的%s", fd, (events & EPOLLIN) ? "EPOLLIN" : "OTHER");

            // 我们将所有的异常情况，最后全部转化成为recv，send的异常！
            if ((events & EPOLLERR) || (events & EPOLLHUP))
                events |= (EPOLLIN | EPOLLOUT);

            if (events & EPOLLIN && ConnIsExists(fd))
                connections_[fd]->recver_(connections_[fd]);
            if (events & EPOLLOUT && ConnIsExists(fd))
                connections_[fd]->sender_(connections_[fd]);
        }
    }

    void Accepter(Connection *c)
    {
        do
        {
            int err = 0;
            uint16_t clientport;
            std::string clientip;
            cout << "开始 Accepter中的 Accept" << endl;
            int sock = listensock_.Accept(&clientip, &clientport, &err);
            if (sock > 0)
            {
                logMessage(Debug, "%s:%d 已经连上了服务器了", clientip.c_str(), clientport);
                AddConnection(sock, EPOLLIN | EPOLLET, clientip, clientport);
            }
            else
            {
                if (err == EAGAIN | err == EWOULDBLOCK) // 没有数据了
                    break;
                else if (err == EINTR) // 收到信号
                    continue;
                else // 本次读取真的出错了
                {
                    logMessage(Warning, "errstring : %s, errcode: %d", strerror(err), err);
                    continue;
                }
            }
        } while (c->events & EPOLLET);
    }

    ~EpollServer()
    {
        listensock_.Close();
        epoller_.Close();
    }

    bool EnableReadWrite(Connection *conn, bool readable, bool writeable)
    {
    }

private:
    bool ConnIsExists(int fd)
    {
        return connections_.find(fd) != connections_.end();
    }

    void Recver(Connection *conn)
    {
        do
        {
            char buffer[bsize];
            ssize_t n = recv(conn->fd_, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                conn->inbuffer_ += buffer;
                // 根据基本协议，进行数据分析
                std::string requestStr;
                int n = protocol_ns::ParsePackage(conn->inbuffer_, &requestStr);
                if (n > 0)
                {
                    requestStr = protocol_ns::RemoveHeader(requestStr, n);
                    Request req;
                    req.Deserialize(requestStr);
                    func_(conn, req); // request 保证是一个完整的请求报文！
                }
            }
            else if (n == 0) // 读取结束
            {
                conn->excepter_(conn);
                break;
            }
            else
            {
                if (errno == EAGAIN | errno == EWOULDBLOCK) // 没有数据了
                    break;
                else if (errno == EINTR) // 收到信号
                    continue;
                else
                {
                    conn->excepter_(conn);
                    break;
                }
            }

        } while (conn->events & EPOLLET); // 如果是ET模式，循环读取直到缓冲区数据被取完
    }

    void Sender(Connection *conn)
    {
        logMessage(Debug, "Sender..., fd: %d, clientinfo: [%s:%d]", conn->fd_, conn->clientip_.c_str(), conn->clientport_);
    }

    void Excepter(Connection *conn)
    {
        logMessage(Debug, "Excepter..., fd: %d, clientinfo: [%s:%d]", conn->fd_, conn->clientip_.c_str(), conn->clientport_);
    }

    void AddConnection(int fd, uint32_t events, std::string clientip = "127.0.0.1", uint16_t clientport = 8888)
    {
        // 设置 fd 非阻塞
        Util::SetNonBlock(fd);
        Connection *c(new Connection(this,fd));
        if (fd == listensock_.Fd())
        {
            c->Register(std::bind(&EpollServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
        }
        else
        {
            c->Register(std::bind(&EpollServer::Recver, this, std::placeholders::_1),
                        std::bind(&EpollServer::Sender, this, std::placeholders::_1),
                        std::bind(&EpollServer::Excepter, this, std::placeholders::_1));
        }
        connections_.insert(std::make_pair(fd, c));
        c->clientip_ = clientip;
        c->clientport_ = clientport;
        c->events = events;
        bool r = epoller_.AddEvent(fd, events);
        assert(r);
        (void)r;
    }

private:
    uint16_t port_;
    Sock listensock_;
    Epoller epoller_;
    struct epoll_event revs_[gnum];
    func_t func_;
    std::unordered_map<int, Connection *> connections_;
};
