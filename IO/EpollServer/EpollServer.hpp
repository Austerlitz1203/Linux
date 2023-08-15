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
static const int linkTimeout = 30;

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

    time_t lasttime_; // 这个connection最近一次就绪的时间
};

class EpollServer
{
    const static int gnum = 64;
    using func_t = std::function<Response(const Request &)>;

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

            checkLink();
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

    // 连接管理器
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
        conn->events = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        return epoller_.AddModEvent(conn->fd_, conn->events, EPOLL_CTL_MOD);
    }

private:
    bool ConnIsExists(int fd)
    {
        return connections_.find(fd) != connections_.end();
    }

    // 只进行读取数据的工作
    bool RecverHelper(Connection* conn)
    {
        int ret = true;
        do
        {
            char buffer[bsize];
            ssize_t n = recv(conn->fd_, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                conn->inbuffer_ += buffer;
            }
            else if (n == 0) // 读取结束
            {
                conn->excepter_(conn);
                ret = false;
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
                    ret = false;
                    conn->excepter_(conn);
                    break;
                }
            }

        } while (conn->events & EPOLLET); // 如果是ET模式，循环读取直到缓冲区数据被取完

        return ret;
    }

    void HandlerRequest(Connection *conn)
    {
        int quit = false;
        while(!quit)
        {
            std::string RequestStr;
            // 1. 提取完整报文
            int n = protocol_ns::ParsePackage(conn->inbuffer_,&RequestStr);
            if(n>0) // 是完整的报文
            {
                // 2. 提取有效载荷
                RequestStr = protocol_ns::RemoveHeader(RequestStr,n);
                // 3. 进行反序列化
                Request req;
                req.Deserialize(RequestStr);
                // 4. 业务处理
                Response  resp = func_(req);
                // 5. 序列化
                std::string ResponseStr;
                resp.Serialize(&ResponseStr);
                // 6. 添加报头
                ResponseStr = AddHeader(ResponseStr);

                // 7. 进行返回
                conn->outbuffer_ += ResponseStr;
            }
            else // 不是完整的报文
            {
                quit = true;
            }
        }
    }

    void Recver(Connection *conn)
    {
        // 读取不到完整报文，则返回，等待下一次读取到完整报文
        if(!RecverHelper(conn)) return;

        // 读取完整报文，处理请求
        HandlerRequest(conn);

        //一般在写入的时候，直接写入，没写完才交给Epoll
        if(!conn->outbuffer_.empty())
            conn->sender_(conn); // 第一次触发发送

    }

    void Sender(Connection *conn)
    {
        bool safe = true;
        do
        {
            ssize_t n = send(conn->fd_, conn->outbuffer_.c_str(), conn->outbuffer_.size(), 0);
            if (n > 0)
            {
                conn->outbuffer_.erase(0, n);
                if (conn->outbuffer_.empty())
                    break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                    continue;
                else
                {
                    safe = false;
                    conn->excepter_(conn);
                    break;
                }
            }
        } while (conn->events & EPOLLET);

        if (!safe)
            return;
        if (!conn->outbuffer_.empty())
            EnableReadWrite(conn, true, true);
        else
            EnableReadWrite(conn, true, false);
    }

    void Excepter(Connection *conn)
    {
        // 1. 先从 Epoll 移除fd
        epoller_.DelEvent(conn->fd_);
        // 2. 移除 unordered_map 中的KV关系
        connections_.erase(conn->fd_);
        // 3. 关闭 fd
        close(conn->fd_);
        // 4. 释放conn对象
        delete conn;
        logMessage(Debug, "Excepter...done, fd: %d, clientinfo: [%s:%d]", conn->fd_, conn->clientip_.c_str(), conn->clientport_);
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
        c->R_ = this;
        c->lasttime_ = time(nullptr);
        bool r = epoller_.AddModEvent(fd, events,EPOLL_CTL_ADD);
        assert(r);
        (void)r;
    }

    void checkLink()
    {
        time_t curr = time(nullptr);
        for(auto &connection : connections_)
        {
            if(connection.second->lasttime_ + linkTimeout > curr) continue;
            else Excepter(connection.second);
        }
    }

private:
    uint16_t port_;
    Sock listensock_;
    Epoller epoller_;
    struct epoll_event revs_[gnum];
    func_t func_;
    std::unordered_map<int, Connection *> connections_;
};
