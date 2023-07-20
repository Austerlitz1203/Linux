#pragma once
// 1. setsid();
// 2. setsid(), 调用进程，不能是组长！
// 3. 守护进程a. 忽略异常信号 b. 0,1,2要做特殊处理 c. 进程的工作路径可能要更改 /

#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Log.hpp"
#include "err.hpp"

// 守护进程的本质：是孤儿进程的一种！
void Daemon()
{
    // 1. 忽略信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    // 2. 让自己不要成为组长
    if (fork() > 0)
        exit(0);
    // 3. 新建会话，自己成为会话的话首进程
    pid_t ret = setsid();
    if ((int)ret == -1)
    {
        logMessage(Fatal, "deamon error, code: %d, string: %s", errno, strerror(errno));
        exit(SETSID_ERR);
    }
    // 4. 可选：可以更改守护进程的工作路径
    // chdir("/")
    // 5. 处理后续的对于0,1,2的问题
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        logMessage(Fatal, "open error, code: %d, string: %s", errno, strerror(errno));
        exit(OPEN_ERR);
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}