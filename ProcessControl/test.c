#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int add_to_top(int top)
{
  printf("enter the function att\n");
  int sum=0;
  int i=0;
  for(i=0;i<top;i++)
  {
    sum+=i;
  }
  exit(123);
  printf("out the function att\n");
  return sum;

}

#define TASK_NUM 10

// 预设一批任务
void sync_disk()
{
    printf("这是一个刷新数据的任务!\n");
}

void sync_log()
{
    printf("这是一个同步日志的任务!\n");
}

void net_send()
{
    printf("这是一个进行网络发送的任务!\n");
}


// 要保存的任务相关的
typedef void (*func_t)(); 
func_t other_task[TASK_NUM] = {NULL};  //函数指针数组

int LoadTask(func_t func)
{
    int i = 0;
    for(; i < TASK_NUM; i++){
        if(other_task[i] == NULL) break;
    }
    if(i == TASK_NUM) return -1;
    else other_task[i] = func;

    return 0;
}

void InitTask()
{
    for(int i = 0; i < TASK_NUM; i++) other_task[i] = NULL;
    LoadTask(sync_disk);
    LoadTask(sync_log);
    LoadTask(net_send);
}

void RunTask()
{
    for(int i = 0; i < TASK_NUM; i++)
    {
        if(other_task[i] == NULL) continue;
        other_task[i]();
    }
}




int main()
{
    pid_t id = fork();
    if(id == 0)
    {
         //子进程
        int cnt = 5;
        while(cnt)
        {
            printf("我是子进程，我还活着，我还有%dS, pid: %d, ppid%d\n", cnt--, getpid(), getppid());
            sleep(1);
            //int *p = NULL;
            //*p = 100;
        }
        if(1==1) exit(0);
        exit(100);
    }
    
    InitTask();
    // 父进程
    //pid_t ret_id = wait(NULL);
    while(1)
    {
        int status = 0;
        pid_t ret_id = waitpid(id, &status, WNOHANG); //添加 WNOHANG 参数之后，就是非阻塞轮询
        if(ret_id < 0)// 出错
        {
            printf("waitpid error!\n");
            exit(1);
        }
        else if(ret_id == 0) // 子进程还在运行 
        {
            RunTask();
            //printf("子进程还在运行，我做自己其他的事情！\n");
            sleep(1);
            continue;
        }
        else{  
           if(WIFEXITED(status)) // 是否收到信号
            {
                printf("wait success, child exit code: %d\n", WEXITSTATUS(status));
            }
            else
            {
                printf("wait success, child exit signal: %d\n", status & 0x7F);
            }
          //.out printf("我是父进程，等待子进程成功, pid: %d, ppid: %d, ret_id: %d, child exit code: %d,child exit signal: %d\n",getpid(), getppid(), ret_id, (status>>8)&0xFF, status & 0x7F);
            break;
        }
    }


//   printf("hello world"); // 输出缓冲区
//    sleep(2);
//    _exit(107);
//
//  for(int i = 0; i<=200; i++)
//  {
//      printf("%d : %s\n", i, strerror(i));
//  }
//  return 0;
//  int result = add_to_top(100);
//  if(result == 5050) return 0;
//  else return 11; //计算结果不正确
}
