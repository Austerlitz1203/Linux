#include"blockQueue.hpp"
#include"Task.hpp"

void* consumer(void* args)
{
    BlockQueue<Task>* bq=static_cast<BlockQueue<Task>*>(args);
    while(true)
    {
        //sleep(1);
        Task a;
        bq->pop(&a);
        a();
        cout<<pthread_self()<<"consumer data:"<<a.formatArg()<<a.formatRes()<<endl;
    }
}

void* productor(void* args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    string opers="+-*/%";
    while(true)
    {
        sleep(1);
        char op=opers[rand()%opers.size()];
        int x=rand()%20+1;
        int y=rand()%10+1;
        Task a(x,y,op);
        bq->push(a);
        cout<<pthread_self()<<"productor data:"<<a.formatArg()<<"=?"<<endl;
    }
}

int main()
{
    srand((uint64_t)time(nullptr)^getpid());
    pthread_t c[3],p[4];
    BlockQueue<Task> *bq=new BlockQueue<Task>();
    pthread_create(c,nullptr,consumer,bq);
    pthread_create(c+1, nullptr, consumer, bq);
    pthread_create(c+2, nullptr, consumer, bq);
    pthread_create(p,nullptr,productor,bq);
    pthread_create(p+1, nullptr, productor, bq);
    pthread_create(p+2, nullptr, productor, bq);
    pthread_create(p+3, nullptr, productor, bq);

    pthread_join(c[0],nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(c[2], nullptr);
    pthread_join(p[0],nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);
    pthread_join(p[3], nullptr);
    delete bq;
    return 0;
}