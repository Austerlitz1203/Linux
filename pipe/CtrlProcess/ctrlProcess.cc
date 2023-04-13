#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include"Task.hpp"

using namespace std;

#define NUM 5

TASK t;

class EndPoint
{
private:
    static int num; // 第几个进程
public:
    pid_t _chid_pid;
    int _pipe_write_fd;
    string _process_name;
public:
    EndPoint(pid_t pid,int pipefd)
        :_chid_pid(pid)
        ,_pipe_write_fd(pipefd)
        {
            char buf[128];
            snprintf(buf,sizeof(buf),"process->%d[%d:%d]",num++,_chid_pid,_pipe_write_fd);
            _process_name=buf;
        }

    string name()const{
        return _process_name;
    }

    ~EndPoint()
    {}
};

int EndPoint::num=0;

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

int ShowChoose()
{
    std::cout << "##########################################" << std::endl;
    std::cout << "|   0. 执行日志任务   1. 执行数据库任务    |" << std::endl;
    std::cout << "|   2. 执行请求任务   3. 退出             |" << std::endl;
    std::cout << "##########################################" << std::endl;
    std::cout << "请选择# ";
    int command = 0;
    std::cin >> command;
    return command;
}

void CtrlProcess(const vector<EndPoint> &end_point)
{
    int cnt = 0;
    while (true)
    {
        // 任务
        int command = ShowChoose();
        if (command == 3)
            break;
        if (command < 0 || command > 2)
            continue;

        // 选择进程
        int index = cnt++;
        cnt %= end_point.size();
        cout << "选择进程：" << end_point[index].name() << " | 处理的任务为" << command << endl;

        // 下发任务
        write(end_point[index]._pipe_write_fd, &command, sizeof(command)); // 对这个管道进行写

        sleep(1);
    }
}

void WaitProcess(const vector<EndPoint> &end_point)
{
    // for(auto &s:end_point)
    //     close(s._pipe_write_fd);
    // cout<<"关闭写端完成"<<endl;

    // for(auto &s:end_point)
    //     waitpid(s._chid_pid,nullptr,0);
    // cout<<"进程等待完成"<<endl;

    // 从 0  到 最后一个，不可以的，这是因为后面的子进程继承了父进程的写端
    // 对于第一个管道而言，后面的多个子进程都可以写，只关闭父进程的写端，第一个管道的读端不会自动关闭
    // 从而也就无法等待成功子进程，阻塞了

    // 所以要从后往前
    for(int i=end_point.size()-1;i >= 0;i--)
    {
        close(end_point[i]._pipe_write_fd);

        waitpid(end_point[i]._chid_pid,nullptr,0);
    }
    cout << "关闭写端完成" << endl;
    cout << "进程等待完成" << endl;
}

int main()
{
    vector<EndPoint> end_point;

    // 构建控制结构，得到 end_point 子进程的pid，父进程的写端
    CreateProcess(end_point);

    // 执行任务
    CtrlProcess(end_point);

    WaitProcess(end_point);

    return 0;
}