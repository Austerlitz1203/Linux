#include "Task.hpp"
#include "Thread.hpp"
#include "LockGuard.hpp"
#include <vector>
#include <queue>

const static int N = 5;

template <class T>
class ThreadPool
{
public:
    ThreadPool(int num = N)
        : _num(num), _threads(num)
    {
        pthread_mutex_init(&_lock, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }

    pthread_mutex_t* GetMutex()
    {
        return &_lock;
    }

    void threadWait()
    {
        pthread_cond_wait(&_cond, &_lock);
    }

    void threadWakeup()
    {
        pthread_cond_signal(&_cond);
    }

    bool isEmpty()
    {
        return _tasks.empty();
    }

    void pushTask(const T &t)
    {
        LockGuard lockguard(&_lock);
        _tasks.push(t);
        threadWakeup(); // 唤醒等待的线程
    }

    T popTask()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }

    static void threadRountine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);
        while (true)
        {
            T t;
            // 有无任务，有，处理，无，等待
            {
                LockGuard lockguard(tp->GetMutex());
                while (tp->isEmpty())
                {
                    tp->threadWait();
                }
                t = tp->popTask(); // 相当于把任务从 _tasks 这个公共区，拿到自己的私有区域
            }

            t();
            cout << "result is : " << t.formatRes() << endl;
        }
    }

    void init()
    {
        for (int i = 0; i < _num; i++)
        {
            _threads.push_back(Thread(i, threadRountine, this));
        }
    }

    void run()
    {
        for (auto &a : _threads)
        {
            a.run();
        }
    }

    void check()
    {
        for (auto &t : _threads)
        {
            std::cout << t.threadname() << " running..." << std::endl;
        }
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    }

private:
    std::vector<Thread> _threads;
    int _num;

    std::queue<T> _tasks;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
};