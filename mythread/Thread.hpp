#pragma once
#include<pthread.h>
#include<string>
#include<iostream>
using namespace std;

class Thread
{

public:
    typedef void* (*func_t)(void*);

    typedef enum
    {
        NEW=0,
        RUNNING,
        EXITED
    }ThreadStatus;

public:
    Thread(int num,func_t func,void* args)
        :_tid(0)
        ,_status(NEW)
        ,_func(func)
        ,_args(args)
    {
        char name[64];
        snprintf(name,64,"pthread-%d",num);
        _name=name;
    }

    ~Thread()
    {}

    void run()
    {
        int n=pthread_create(&_tid,nullptr,_func,_args);
        if(n!=0)
        {
            cout<<"create pthread error"<<endl;
            exit(1);
        }
        _status=RUNNING;
    }

    void join()
    {
        int n=pthread_join(_tid,nullptr);
        if(n!=0)
        {
            cerr<<_name<<" error"<<endl;
            return;
        }
        _status=EXITED;
    }

private:
    pthread_t _tid;
    string _name;
    func_t _func;
    void* _args;
    ThreadStatus _status;
};