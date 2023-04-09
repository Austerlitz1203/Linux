#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <assert.h>
#include"Task.hpp"

using namespace std;

#define NUM 5

TASK t;

class EndPoint
{
public:
    pid_t _chid_pid;
    int _pipe_write_fd;

public:
    EndPoint(pid_t pid,int pipefd)
        :_chid_pid(pid)
        ,_pipe_write_fd(pipefd)
        {}

    ~EndPoint()
    {}
};

void WaitCommand()
{
    while(true)
    {
        int command=0;
        int n=read(0,&command,sizeof(int)); // 从读端 读取父进程输入的指令
        if(n == sizeof(int))
        {
            t.Excute(command);
        }
        else if(n == 0)
        {
            break;
        }
        else break;

    }
}

void CreateProcess(vector<EndPoint> &endpoint)
{
    for (int i = 0; i < NUM; i++)
    {
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        assert(n != -1);

        pid_t id = fork();
        assert(id != -1);
        if (id == 0) // 子进程
        {
            close(pipefd[1]);
            dup2(pipefd[0], 0); // 输入重定向
            WaitCommand();
            close(pipefd[0]);
            exit(0);
        }

        // 父进程 , 通过多个管道和多个子进程通信
        close(pipefd[0]);

        endpoint.push_back(EndPoint(id,pipefd[1]));
    }
}

int main()
{
    vector<EndPoint> end_point;
    CreateProcess(end_point);
    while(true)
    {
        int command=PRINT_LOG;
        int index=rand()%end_point.size(); // 随机选一个管道
        write(end_point[index]._pipe_write_fd,&command,sizeof(command));  // 对这个管道进行写

        sleep(1);
    }

    return 0;
}