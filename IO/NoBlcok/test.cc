#include <iostream>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <functional>

void PrintLog()
{
    std::cout << "这个是一个日志例程" << std::endl;
}
void OperMysql()
{
    std::cout << "这个是一个操作数据库的例程" << std::endl;
}
void CheckNet()
{
    std::cout << "这个是一个检测网络状态的例程" << std::endl;
}

using func_t = std::function<void(void)>;
std::vector<func_t> funcs;

// 一个线程想想另一个线程派发任务
void LoadTask()
{
    funcs.push_back(PrintLog);
    funcs.push_back(OperMysql);
    funcs.push_back(CheckNet);
}

void HandlerALLTask()
{
    for (const auto &func : funcs)
        func();
}

void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cerr << "error string: " << strerror(errno) << " error code: " << errno << std::endl;
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

int main()
{
    char buffer[64];
    SetNonBlock(0);
    LoadTask();
    while (true)
    {
        printf(">>> ");
        fflush(stdout);
        // 1. 成功  2. 结束  3. 出错(一旦底层没有数据就绪，以出错的形式返回，但是不算真正的出错)
        ssize_t n = read(0, buffer, sizeof(buffer) - 1); // 检测条件是否就绪 + 拷贝
        if (n > 0)
        {
            buffer[n - 1] = 0;
            std::cout << "echo# " << buffer << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "end file" << std::endl;
            break;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 底层数据没有准备好，希望下次继续来检测
                HandlerALLTask();
                sleep(1);

                std::cout << "data not ready" << std::endl;
                continue;
            }
            else if (errno == EINTR)
            {
                // 这次IO被信号中断，也需要重新读取
                continue;
            }
            else
            {
                std::cerr << "read error? "
                          << "error string: " << strerror(errno) << " error code: " << errno << std::endl;
                break;
            }
            // break;
        }
    }
}