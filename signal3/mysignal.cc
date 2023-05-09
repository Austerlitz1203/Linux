#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>


using namespace std;


// 多线程
void *thread1_run(void *args)
{
    while (1)
    {
        printf("我是线程1， 我正在运行\n");
        sleep(1);
    }
}

void *thread2_run(void *args)
{
    while (1)
    {
        printf("我是线程2， 我正在运行\n");
        sleep(1);
    }
}

void *thread3_run(void *args)
{
    while (1)
    {
        printf("我是线程3， 我正在运行\n");
        sleep(1);
    }
}

int main()
{
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, thread1_run, NULL);
    pthread_create(&t2, NULL, thread2_run, NULL);
    pthread_create(&t3, NULL, thread3_run, NULL);

    while (1)
    {
        printf("我是主线程， 我正在运行\n");
        sleep(1);
    }
}







volatile int quit = 0; // 编译加 -O1 选项，优化

void handler(int signo)
{
    cout << "change quit from 0 to 1" << endl;
    quit = 1;
    cout << "quit:" << quit;
}

// int main()
// {
//     signal(2,handler);

//     while(!quit);

//     cout<<"main quit is normal"<<endl;
//     return 0;
// }




pid_t id;

void waitProcess(int signo)
{
    printf("捕捉到一个信号:%d,who get it:%d\n", signo, getpid());
    sleep(5);

    // 去掉这一层的 while，则可以看到，父进程只会回收部分子进程
    while (1)
    {
        pid_t res = waitpid(-1, NULL, WNOHANG);
        if (res > 0)
        {
            printf("wait success,res:%d,id:%d,\n", res, id);
        }
        else
            break;
    }

    printf("handler done...\n");
}



// int main()
// {
//     signal(SIGCHLD, waitProcess);

//     int i = 1;
//     for (; i < 10; i++)
//     {
//         id = fork();
//         if (id == 0)
//         {
//             int cnt = 5;
//             while (cnt)
//             {
//                 printf("我是子进程，我的pid:%d,ppid:%d\n", getpid(), getppid());
//                 sleep(1);
//                 cnt--;
//             }
//             exit(1);
//         }
//     }
//     while (1)
//     {
//         sleep(1);
//     }

//     return 0;
// }
