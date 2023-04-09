#pragma once
#include<iostream>
#include<vector>
#include<unistd.h>

typedef void (*func_t)();

using std::cout;
using std::endl;

void Print_Log()
{
    cout<<"pid:"<<getpid()<<",打印日志任务，正在执行！"<<endl;
}

void Instert_Data()
{
    cout<<"pid："<<getpid()<<",插入数据任务，正在执行！"<<endl;
}

void Detete_Data()
{
    cout<<"pid："<<getpid()<<",删除数据任务，正在执行！"<<endl;
}

#define PRINT_LOG 0
#define INSERT_DATA 1
#define DELETE_DATA 2

class TASK
{
    public:
    TASK()
    {
        funcs.push_back(Print_Log);
        funcs.push_back(Instert_Data);
        funcs.push_back(Detete_Data);
    }

    void Excute(int command)
    {
        if(command >=0 && command <=funcs.size()) funcs[command]();
    }

    std::vector<func_t> funcs;
};