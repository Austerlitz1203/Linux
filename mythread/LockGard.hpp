#pragma once
#include<pthread.h>


class LockGard
{
public:
    LockGard(pthread_mutex_t *mutex)
        :_mutex(mutex)
    {
        pthread_mutex_lock(_mutex);
    }

    ~LockGard()
    {
        pthread_mutex_unlock(_mutex);
    }

private:
    pthread_mutex_t *_mutex;
};