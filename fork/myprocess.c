#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
  int x=100; 
  pid_t ret=fork();

  if(ret == 0)
  {
    while(1)
    {
      printf("我是子进程，我的pid是：%d，我的父进程是：%d，x的值是 %d ,地址是: %p \n",getpid(),getppid(),x,&x);
      sleep(1);
    }
  }
  else if(ret > 0) 
  {
    while(1)
    {
      printf("我是父进程，我的pid是：%d，我的父进程是：%d，x的值是 %d ,地址是: %p \n",getpid(),getppid(),x,&x);
      x=666;
      sleep(1);
    }
  }

  return 0;
}
