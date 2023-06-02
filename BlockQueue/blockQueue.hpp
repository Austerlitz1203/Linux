#pragma once
#include<iostream>
#include<unistd.h>
#include<queue>
#include<ctime>

using namespace std;

const int _cap=5;

template<class T>
class BlockQueue
{
public:

    BlockQueue(const int cap = _cap)
        :_capacity(cap)
    {
        pthread_mutex_init(&_mutex,nullptr);
        pthread_cond_init(&_productCond,nullptr);
        pthread_cond_init(&_consumerCond,nullptr);
    }

    int isFull()
    {
        return _q.size() == _capacity;
    }

    int isEmpty()
    {
        return _q.size() == 0;
    }

    void push(const T& in)
    {
        pthread_mutex_lock(&_mutex);
        while(isFull())
        {
            pthread_cond_wait(&_productCond,&_mutex);
        }

        _q.push(in);
        pthread_cond_signal(&_consumerCond);
        pthread_mutex_unlock(&_mutex);
    }

    void pop(T* out)
    {
        pthread_mutex_lock(&_mutex);
        while(isEmpty())
        {
            pthread_cond_wait(&_consumerCond,&_mutex);
        }
        *out=_q.front();
        _q.pop();
        pthread_cond_signal(&_productCond);
        pthread_mutex_unlock(&_mutex);
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_productCond);
        pthread_cond_destroy(&_consumerCond);
    }

private:
    queue<T> _q;
    int _capacity; // 阻塞队列的最大容量
    pthread_mutex_t _mutex;
    pthread_cond_t _productCond; // 生产者的条件变量
    pthread_cond_t _consumerCond; // 消费者的条件变量
};
