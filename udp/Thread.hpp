#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include<functional>

class Thread
{
public:
    typedef enum
    {
        NEW = 0,
        RUNNING,
        EXITED
    } ThreadStatus;
    //typedef void (*func_t)(void *);
    using func_t=std::function<void()>;


public:
    Thread(int num = 0, func_t func =nullptr) : _tid(0), _status(NEW), _func(func)
    {
        char name[128];
        snprintf(name, sizeof(name), "thread-%d", num);
        _name = name;
    }

    int status() { return _status; }
    std::string threadname() { return _name; }

    pthread_t threadid()
    {
        if (_status == RUNNING)
            return _tid;
        else
        {
            return 0;
        }
    }

    // 是不是类的成员函数，而类的成员函数，具有默认参数this,需要static
    // 但是会有新的问题：static成员函数，无法直接访问类属性和其他成员函数
    static void *runHelper(void *args)
    {
        Thread *ts = (Thread *)args; // 就拿到了当前对象
        // _func(_args);
        (*ts)();
        return nullptr;
    }
    void operator()() // 仿函数
    {
        if (_func != nullptr)
            _func();
    }
    void run()
    {
        int n = pthread_create(&_tid, nullptr, runHelper, this);
        if (n != 0)
            exit(1);
        _status = RUNNING;
    }
    void join()
    {
        int n = pthread_join(_tid, nullptr);
        if (n != 0)
        {
            std::cerr << "main thread join thread " << _name << " error" << std::endl;
            return;
        }
        _status = EXITED;
    }
    ~Thread()
    {
    }

private:
    pthread_t _tid;
    std::string _name;
    func_t _func; // 线程未来要执行的回调
    ThreadStatus _status;
};