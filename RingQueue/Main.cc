#include"RingQueue.hpp"
#include"Task.hpp"
#include<time.h>
#include<unistd.h>
#include<sys/types.h>

const char* ops="+-*/%";

using namespace std;

void* ConsumerRoutine(void* args)
{
    RingQueue<Task>* c=static_cast<RingQueue<Task>*>(args);
    while(true)
    {
        Task x;
        c->pop(&x);
        x();
        cout<<"Consumer done:"<<x.formatRes()<<endl;
        sleep(1);
    }
}

void *ProductorRoutine(void *args)
{
    RingQueue<Task>* p=static_cast<RingQueue<Task>*>(args);
    while(true)
    {
        int x=rand()%100 + 1;
        int y=rand()%100+1;
        char op=ops[(x+y)%5];
        Task t(x, y, op);
        p->push(t);
        cout<<"Productor done:"<<t.formatArg()<<endl;
    }
}

int main()
{
    srand(time(nullptr)^getpid());
    RingQueue<Task> *rq = new RingQueue<Task>();

    // pthread_t c,p;
    // pthread_create(&c,nullptr,ConsumerRoutine,rq);
    // pthread_create(&p, nullptr, ProductorRoutine, rq);

    // pthread_join(c,nullptr);
    // pthread_join(p,nullptr);

    pthread_t c[3], p[2];
    for (int i = 0; i < 3; i++)
        pthread_create(c + i, nullptr, ConsumerRoutine, rq);
    for (int i = 0; i < 2; i++)
        pthread_create(p + i, nullptr, ProductorRoutine, rq);

    for (int i = 0; i < 3; i++)

        pthread_join(c[i], nullptr);
    for (int i = 0; i < 2; i++)

        pthread_join(p[i], nullptr);

    delete rq;

    return 0;
}