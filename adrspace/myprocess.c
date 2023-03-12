#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int g_value = 100; //全局变量

int main()
{
    // fork在返回的时候，父子都有了，return两次，id是不是pid_t类型定义的变量呢？返回的本质，就是写入！
    // 谁先返回，谁就让OS发生写时拷贝
    pid_t id = fork();
    assert(id >= 0);
    if(id == 0)
    {
        //child
        while(1)
        {
            printf("我是子进程, 我的id是: %d, 我的父进程是: %d, g_value: %d, &g_value : %p\n",\
                    getpid(), getppid(), g_value, &g_value);
            sleep(1);
            g_value=200; // 只有子进程会进行修改
        }
    }
    else
    {
        //father
        while(1)
        {
            printf("我是父进程, 我的id是: %d, 我的父进程是: %d, g_value: %d, &g_value : %p\n",\
                    getpid(), getppid(), g_value, &g_value);
            sleep(1);
        }
    }
}
