#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
  extern char ** environ;
  pid_t id=fork();

  if(id == 0 ) // 子进程
  {
    //execl("/bin/ls","ls","-l",NULL); // 从这里就可以看出，程序替换只影响当前进程
    
    char *const argv[]={
      "ls",
      "-a",
      "-l",
      NULL
    };
    //execv("/bin/ls",argv);
    //execlp("ls","ls","-a",NULL);
    //execvp("ls",argv);

    char* const myenv[]={
      "MYENV=This is an envi created by me!",
      NULL
    };

   //execl("./otherproc/otherproc","otherproc",NULL); 
   // execle("./otherproc/otherproc","otherproc",NULL,myenv);  // 将自己创建的环境变量 myenv 传入otherproc,覆盖式传入，以前的被清空 
   
   execle("./otherproc/otherproc","otherproc",NULL,environ); 

   //putenv("MYENV=This is an envi created by me!"); //向自己的环境变量列表里面新增一个环境变量
   //execle("./otherproc/otherproc","otherproc",NULL,environ); 

   exit(0);
  }

  sleep(1);
  // 父进程
  printf("我是父进程，我的pid是：%d \n",getpid());
  int status=0;
  waitpid(id,&status,0);  // 阻塞等待
  printf("child exit code:%d \n",WEXITSTATUS(status));

  return 0;
}
