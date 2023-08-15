#pragma once

#include"Log.hpp"
#include"err.hpp"
#include<sys/epoll.h>

static const int defaultepfd = -1;
static const int gsize = 128;

class Epoller
{

public:
    Epoller(int epfd = defaultepfd)
        :epfd_(epfd)
    {}

    void Create()
    {
        epfd_ = epoll_create(gsize);
        if(epfd_ < 0)
        {
            logMessage(Fatal, "epoll_create error, code: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }

    bool AddModEvent(int fd,uint32_t events,int op)
    {
        struct epoll_event ev;
        ev.events  = events;
        ev.data.fd=fd;
        int n = epoll_ctl(epfd_,op,fd,&ev);
        if( n < 0)
        {
            logMessage(Fatal, "epoll_ctl error, code: %d, errstring: %s", errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool DelEvent(int fd)
    {
        return epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,nullptr);
    }

    int wait(struct epoll_event* revs , int num,int timeout)
    {
        return epoll_wait(epfd_,revs,num,timeout);
    }

    int Fd()
    {
        return epfd_;
    }

    void Close()
    {
        if(epfd_ != defaultepfd) close(epfd_);
    }

    ~Epoller()
    {}

private:
    int epfd_;
};