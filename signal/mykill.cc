#include<iostream>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<cerrno>
#include<cstring>

using namespace std;

void handle1(int signum)
{
    cout<<"I get a signal:"<<signum<<endl;
}


void test1()
{
    signal(SIGINT, handle1);

    while (true)
    {
        cout << "我是一个正在运行的进程，pid:" << getpid() << endl;
        sleep(1);
    }
}


void Manual(char* str)
{
    cout<<"Manual"<<endl;
    cout<<str<<"信号编号 目标进程"<<endl;
}

int count = 0;

void myhandler(int signo)
{
    cout << "get a signal: " << signo <<endl<<"n:"<<count<< endl;
    int n = alarm(10);
    cout << "return: " << n << endl;
    // alarm(0);
    //exit(0);
}




int main(int argc,char* argv[])
{
    // 1.测试 signal() ，自定义产生指定信号后的动作
    //test1();

    // 2.测试 kill 系统调用，根据进程号和信号
    // if(argc != 3)
    // {
    //     Manual(argv[0]);
    // }

    // int signum=atoi(argv[1]);
    // int procpid=atoi(argv[2]);

    // //cout<<procpid<<" "<<signum<<endl;

    // int n=kill(procpid,signum);
    // if(n != 0)
    // {
    //     cout<<errno<<":"<<strerror(errno)<<endl;
    //     exit(-1);
    // }


    // 3.raise 对当前进程发送指定信号
    // signal(SIGINT, myhandler);
    // while (true)
    // {
    //     sleep(1);
    //     raise(2);
    // }


    //  4.测试 abort() ，对当前进程发送6号信号，
    // signal(SIGABRT,myhandler);

    // while(true)
    // {
    //     cout<<"test start"<<endl;
    //     sleep(1);
    //     abort();
    //     cout<<"test end"<<endl;
    // }


    //  5.测试 alarm信号，间隔一定时间发送14号时钟信号
    // cout<<"pid:"<<getpid()<<endl;
    // signal(SIGALRM,myhandler);

    // alarm(1);
    // while(true) count++;
    // // {
    // //     cout<<"count:"<<count++<<endl;
    // // }


    // 6. alarm 的返回值
    cout<<"pid:"<<getpid()<<endl;
    signal(SIGALRM,myhandler);
    alarm(10);
    while(1)
    {
        sleep(1);
    }



    return 0;
}