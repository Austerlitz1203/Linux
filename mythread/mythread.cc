#include "Thread.hpp"
#include "LockGard.hpp"

#include <unistd.h>
using namespace std;

int tickets = 5000;
pthread_mutex_t mutex;

void *thread_run(void *args)
{
    string message = static_cast<const char *>(args);
    while (true)
    {
        {
            LockGard lockgard(&mutex); // RAII 风格的锁
            if (tickets > 0)
            {
                usleep(2000);
                cout << message << " get a ticket：" << tickets-- << endl;
            }
            else
            {
                break;
            }
        }
        usleep(100); // 抢完票的后续动作
    }
    return nullptr;
};

int main()
{
    pthread_mutex_init(&mutex,nullptr);

    Thread t1(1, thread_run, (void *)"hello 1");
    Thread t2(2, thread_run, (void *)"hello 2");
    Thread t3(3, thread_run, (void *)"hello 3");
    Thread t4(4, thread_run, (void *)"hello 4");
    t1.run();
    t2.run();
    t3.run();
    t4.run();

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}