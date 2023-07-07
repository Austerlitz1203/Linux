#include"RingQueue.hpp"
#include<time.h>
#include<unistd.h>
#include<sys/types.h>



using namespace std;

void* ConsumerRoutine(void* args)
{
    RingQueue<int>* c=static_cast<RingQueue<int>*>(args);
    while(true)
    {
        int x;
        c->pop(&x);
        cout<<"pop:"<<x<<endl;
        sleep(1);
    }
}

void *ProductorRoutine(void *args)
{
    RingQueue<int>* p=static_cast<RingQueue<int>*>(args);
    while(true)
    {
        int x=rand()%100+1;
        p->push(x);
        cout<<"push:"<<x<<endl;
    }
}

int main()
{
    srand(time(nullptr)^getpid());
    RingQueue<int> *rq = new RingQueue<int>();

    pthread_t c,p;
    pthread_create(&c,nullptr,ConsumerRoutine,rq);
    pthread_create(&p, nullptr, ProductorRoutine, rq);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);

    return 0;
}