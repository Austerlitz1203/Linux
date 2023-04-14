#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include<cassert>

#include "comm.hpp"

using std::cin;
using std::cout;
using std::endl;

int main()
{
    // 打开fifo
    int wfd=open(filename.c_str(),O_WRONLY);
    if(wfd < 0)
    {
        cout<<"open fifo error"<<errno<<":"<<strerror(errno)<<endl;
        return 1;
    }
    cout<<"open fifo success"<<endl;

    // 写入
    char buf[NUM];
    while(true)
    {
        cout<<"请输入你的信息#";
        char* msg=fgets(buf,sizeof(buf),stdin); // C库的函数，会默认加上 \0
        assert(msg);
        (void*)msg;

        size_t n=write(wfd,buf,sizeof(buf)-1);
        if(strcasecmp(buf,"quit") == 0) break;

    }
    close(wfd);
    return 0;
}