#include"blockQueue.hpp"

void* consumer(void* args)
{
    BlockQueue<int>* bq=static_cast<BlockQueue<int>*>(args);
    while(true)
    {
        sleep(1);
        int a;
        bq->pop(&a);
        cout<<"consumer data:"<<a<<endl;
    }
}

void* productor(void* args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while(true)
    {
        int x=rand()%20+1;
        bq->push(x);
        cout<<"productor data:"<<x<<endl;
    }
}

int main()
{
    srand((uint64_t)time(nullptr)^getpid());
    pthread_t c,p;
    BlockQueue<int> *bq=new BlockQueue<int>();
    pthread_create(&c,nullptr,consumer,bq);
    pthread_create(&p,nullptr,productor,bq);


    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
    delete bq;
    return 0;
}