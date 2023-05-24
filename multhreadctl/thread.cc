#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <ctime>

using namespace std;

#define NUM 10


void* thread_run(void* args)
{
    char* name=(char*)args;

    int cnt=3;
    while(cnt)
    {
        cout<<"I am a thread,my name："<<name<<",my id："<<pthread_self()<<endl;
        sleep(1);
        cnt--;
    }

    delete name;
    pthread_exit((void*)222); // 只退出当前线程
    //exit(111);  // 主线程也退出
}


void test1()
{
    pthread_t tids[NUM];
    for (int i = 0; i < NUM; i++)
    {
        // char tname[64];  // 这样会出错，所有传到线程里的 name 都是10。所有线程看到的都是同一块空间。
        char *tname = new char[64];
        snprintf(tname, 64, "thread-%d", i);
        pthread_create(tids + i, nullptr, thread_run, tname); // 所有线程创建的时候都穿的一个地址
    }

    void *ret = nullptr;

    for (int i = 0; i < NUM; i++)
    {
        // 线程等待
        int n = pthread_join(tids[i], &ret);
        if (!n)
        {
            cerr << "pthread_join error,the tid is：" << tids[i] << endl;
        }
        cout << "ret：" << (uint64_t)ret << endl; // 云服务器是 64 位的
    }

    cout << "all thread quit" << endl;
}



enum{OK=0,ERROR};

class ThreadData
{
public:

    ThreadData(const string &name,int id,uint64_t createtime,int top)
        :_name(name)
        ,_id(id)
        ,_createTime(createtime)
        ,_top(top)
    {}

    ~ThreadData()
    {}

    string _name;
    int _id;
    uint64_t _createTime;
    int _top;

    int _status;
    int _result;

};

void *thread_run2(void *args)
{
    ThreadData* tmp=(ThreadData*)args;
    tmp->_result=0;
    for(int i=1;i<=tmp->_top;++i)
    {
        tmp->_result+=i;
    }

    cout<<tmp->_name<<"，calculate done！"<<endl;

    pthread_exit(tmp);
}

void test2()
{
    pthread_t tids[NUM];
    for (int i = 0; i < NUM; i++)
    {
        char tname[64];
        snprintf(tname, 64, "thread-%d", i);
        ThreadData* td=new ThreadData(tname,i+1,time(nullptr),100+i*10);
        pthread_create(tids + i, nullptr, thread_run2, td); // 最后一个参数可以是calss的指针
    }

    void *ret = nullptr;

    for (int i = 0; i < NUM; i++)
    {
        int n = pthread_join(tids[i], &ret);
        if (n != 0)
        {
            cerr << "pthread_join error,the tid is：" << tids[i] <<"，error number："<< n << endl;
        }
        ThreadData* td=static_cast<ThreadData*>(ret);
        if(td->_status ==OK)
        {
            cout<<"计算结果是："<<td->_result<<"，这是[1,"<<td->_top<<"] 的结果！"<<endl;
        }

    }

    cout << "all thread quit" << endl;
}


void* thread_run3(void* args)
{
    char* name=static_cast<char*>(args);

    int cnt=5;
    while(cnt)
    {
        cout<<name<<" is running："<<cnt--<<" thread id："<<pthread_self()<<endl;
    }

    pthread_exit((void*)55);

}

void test3()
{
    pthread_t tid;
    pthread_create(&tid,nullptr,thread_run3,(void*)"thread 1");

    pthread_cancel(tid);


    void* ret=nullptr;
    pthread_join(tid,&ret);
    cout<<"new thread exit："<<(int64_t)ret<<" quit thread："<<tid<<endl;
}


int tickets=10000;//票数
pthread_mutex_t mutex;

void* thread_run4(void* args)
{
    string name=static_cast<const char*>(args);
    while(true)
    {
        pthread_mutex_lock(&mutex);
        if(tickets>0)
        {
            usleep(2000);
            cout<<name<<" get a ticket："<<tickets--<<endl;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        usleep(1000); // 抢完一张票的后续动作
    }
    return nullptr;
}


void test4()
{
    pthread_mutex_init(&mutex,nullptr);
    pthread_t t[4];
    for(int i=0;i<4;i++)
    {
        char* data=new char[64];
        snprintf(data,64,"thread-%d",i+1);
        pthread_create(t+i,nullptr,thread_run4,data);
    }


    for(int i=0;i<4;i++)
    {
        pthread_join(t[i],nullptr);
    }

    return;
}

int main()
{
    //test1(); // 线程控制
    //test2(); // 传入类的指针
    //test3();
    test4();// 加锁
    return 0;
}