#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>
#include <cerrno>
#include <cstring>
#include<unistd.h>

#include"comm.hpp"

using std::cin;
using std::cout;
using std::endl;

int main()
{
    // 1.创建命名管道
    umask(0);
    int n = mkfifo(filename.c_str(),mode);
    if(n < 0)
    {
        cout<<"create fifo error:"<<errno<<":"<<strerror(errno)<<endl;
        return 1;
    }
    cout<<"create fifo success"<<endl;

    // 2.开启管道文件
    int rfd=open(filename.c_str(),O_RDONLY);
    if(rfd < 0)
    {
        cout<<"open fifo file error:"<<errno<<":"<<strerror(errno)<<endl;
        return 1;
    }
    cout<<"open fifo file success"<<endl;

    // 3. 开始通信
    char buf[NUM];

    while(true)
    {
        // 先将缓冲区置0
        buf[0]=0;
        size_t n=read(rfd,buf,sizeof(buf)-1); // 读取是按字节流，所以去掉 \0
        if(n > 0)
        {
            buf[n]='\0';
            cout<<"client#"<<buf<<endl;
            fflush(stdout);
        }
        else if(n==0)
        {
            cout<<"client quit,i will quit either"<<endl;
            break;
        }
        else{
            cout<<errno<<":"<<strerror(errno)<<endl;
        }
    }

    close(rfd);
    unlink(filename.c_str());

     return 0;
}