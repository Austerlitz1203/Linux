#pragma once

#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdarg>
#include<sys/types.h>
#include<unistd.h>

using std::string;

const string filename = "log/tcpserver.log";

enum
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Uknown
};

static string toLevelString(int level)
{
    switch (level)
    {
    case Debug:
        return "Debug";
    case Info:
        return "Info";
    case Warning:
        return "Warning";
    case Error:
        return "Error";
    case Fatal:
        return "Fatal";
    default:
        return "Uknown";
    }
}

static string getTime()
{
    time_t curr = time(nullptr);
    struct tm *tmp = localtime(&curr);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%d-%d-%d %d:%d:%d", tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
             tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    return buffer;
}

void logMessage(int level,const char* formate,...)
{
    char LogLeft[1024];
    string level_str=toLevelString(level);
    string cur_time=getTime();
    snprintf(LogLeft,sizeof(LogLeft),"[%s] [%s] [%d]",level_str.c_str(),cur_time.c_str(),getpid());

    char LogRight[1024];
    va_list p;
    va_start(p,formate);
    vsnprintf(LogRight,sizeof(LogRight),formate,p);
    va_end(p);

    // 打印
    // printf("%s%s\n", logLeft, logRight);

    // 保存到文件中
    FILE *fp = fopen(filename.c_str(), "a");
    if (fp == nullptr)
        return;
    fprintf(fp, "%s%s\n", LogLeft, LogRight);
    fflush(fp); // 可写也可以不写
    fclose(fp);

    //  va_list p; // char *
    //  int a = va_arg(p, int);  // 根据类型提取参数
    //  va_start(p, format); //p指向可变参数部分的起始地址
    //  va_end(p); // p = NULL;
}