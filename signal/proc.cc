#include <iostream>
#include <unistd.h>

int main()
{
    while (true)
    {
        std::cout << "我是一个进程，我正在运行 ..., pid: " << getpid() << std::endl;
        sleep(1);
    }
}