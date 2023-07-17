#pragma once

#include<iostream>
#include<vector>
#include<pthread.h>
#include<semaphore.h>

using std::vector;

const int N=5;


template<class T>
class RingQueue
{
private:

    void P(sem_t& s)
    {
        sem_wait(&s);
    }

    void V(sem_t& s)
    {
        sem_post(&s);
    }

    void Lock(pthread_mutex_t &m)
    {
        pthread_mutex_lock(&m);
    }

    void UnLock(pthread_mutex_t &m)
    {
        pthread_mutex_unlock(&m);
    }

public:

    RingQueue(int num=N)
        :_ring(num)
        ,_cap(num)
    {
        sem_init(&_space_sem,0,num);
        sem_init(&_data_sem,0,0);
        _c_pos=_p_pos=0;

        pthread_mutex_init(&_p_mutex,nullptr);
        pthread_mutex_init(&_c_mutex,nullptr);
    }

    void push(const T& in)
    {
        P(_space_sem);
        Lock(_p_mutex);
        _ring[_p_pos++]=in;
        _p_pos%=_cap;
        UnLock(_p_mutex);
        V(_data_sem);
    }

    void pop(T* out)
    {
        P(_data_sem);
        Lock(_c_mutex);
        *out=_ring[_c_pos++];
        _c_pos%=_cap;
        UnLock(_c_mutex);
        V(_space_sem);
    }

    ~RingQueue()
    {
        pthread_mutex_destroy(&_p_mutex);
        pthread_mutex_destroy(&_c_mutex);

        sem_destroy(&_space_sem);
        sem_destroy(&_data_sem);
    }

private:
    vector<T> _ring;  // 环形队列
    int _cap;         // 环形队列容器大小
    sem_t _space_sem; // 只有生产者关心
    sem_t _data_sem;  // 只有消费者关心
    int _p_pos;       // 生产者的位置
    int _c_pos;       // 消费者的位置

    pthread_mutex_t _p_mutex;  // 信号量相当于把资源分成了单独的一个个，所以生产者消费者各一把锁
    pthread_mutex_t _c_mutex;
};