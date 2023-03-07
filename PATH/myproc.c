#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define NAME "leo"

void Test1()
{
  
  pid_t p=fork();
  if(p == 0)
  {
    while(1)
    {
    printf("我是子进程，我的pid是:%d ,ppid:%d \n",getpid(),getppid());
    sleep(1);
    }
  }
  else 
  {
    int cnt=10;
    while(1)
    {
    printf("我是父进程，我的pid是%d ,ppid:%d \n",getpid(),getppid());
    sleep(1);
    if (cnt-- == 0) break;
    }
  }

}


void Test2()
{
  char* user=getenv("USER");
  printf("%s\n",user);
  if(strcmp(user,NAME) == 0) 
  {
    printf("这个程序被%s执行！\n",user);
  }
  else 
    printf("当前用户%s权限不足，无法执行该程序!\n",user);
}

void Test3()
{
  char* tmp=getenv("HELLO");
  printf("myenv:%s\n",tmp);
}


void Usage(const char *name)
{
    printf("\nUsage: %s -[a|b|c]\n\n", name);
    exit(0); // 终止进程
}



int main(int argc,char* argv[],char* envp[])
{
  //Test1(); // 测试孤儿进程
  //Test2(); // 测试分配权限
  //Test3(); // 测试自己写环境变量，用export 与 不用export 添加的区别
  

  // 测试 根据不同选项，使用不同功能
  if(argc != 2) Usage(argv[0]);
  if(strcmp(argv[1], "-a") == 0) printf("打印当前目录下的文件名\n");
  else if(strcmp(argv[1], "-b") == 0) printf("打印当前目录下的文件的详细信息\n");
  else if(strcmp(argv[1], "-c") == 0) printf("打印当前目录下的文件名(包含隐藏文件)\n");
  else printf("其他功能，待开发\n");
  
  
   // 测试命令行参数    
   printf("argc: %d\n", argc);
   //for(int i = 0 ; argv[i]; i++)
   int i;
   for(i = 0 ; i < argc; i++)
   {         
     printf("argv[%d]->%s\n", i, argv[i]);
   }     


  //测试环境变量
  
  // 方法一，利用main函数的传参
//  int i;
//  for(i=0;envp[i];i++)
//  {
//    printf("envp[%d]->%s\n",i,envp[i]);
//  }
//  printf("\n\n\n");
//
//  // 方法二，利用 environ
//  extern char** environ;
//  for(i=0;environ[i];i++)
//  {
//    printf("envp[%d]->%s\n",i,environ[i]);
//  }
//  printf("\n\n\n");
//
//  // 方法三，利用 getenv() 函数
//  char* pwd=getenv("PWD");
//  if(pwd == NULL) perror("getrnv");
//  else printf("%s\n",pwd);
//
//  char* user=getenv("USER");
//  if(user == NULL) perror("getenv");
//  else printf("%s\n",user);
//
  return 0;
}
