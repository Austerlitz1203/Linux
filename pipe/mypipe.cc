#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <string.h>
#include <stdio.h>

using std::cout;
using std::endl;

int main()
{
    int pipefd[2] = {0};
    // 1.创建管道
    int n = pipe(pipefd); // pipefd[0] 写端   pipefd[1] 读端
    if (n == -1)
    {
        cout << "create pipe error," << errno << strerror(errno) << endl;
        exit(1);
    }
    // cout<<pipefd[0]<<endl<<pipefd[1];

    // 2.创建子进程
    pid_t pid = fork();

    // 子进程
    if (pid == 0)
    {
        // 3.子进程写，那么关闭读端
        close(pipefd[0]);

        // 4.通信
        char buf[128];
        int cnt = 1;
        while (true)
        {
            snprintf(buf, sizeof buf, "我是子进程，cnt：%d ,pid: %d\n", cnt++, getpid());
            write(pipefd[1], buf, strlen(buf));
        }

        close(pipefd[1]); // 管道声明周期随进程，所以不手动关闭，进程结束后也会自动关闭
        exit(1);
    }

    // 父进程

    // 3.父进程读，关闭写端
    close(pipefd[1]);

    // 4.通信
    char buf[128];
    while (true)
    {
        sleep(3);
        int n = read(pipefd[0], buf, sizeof(buf) - 1); // 留出一个位置放\0
        if (n > 0)
        {
            buf[n]='\0';
            cout << "我是父进程,子进程给的信息：" << buf << endl;
        }
    }

    close(pipefd[0]);
    return 0;
}