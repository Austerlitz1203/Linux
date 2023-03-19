#include<stdio.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>

#define MAX 666
#define NUM 64
#define STEP " "

int splitc(char* str,char* argv[])
{
  assert(str);
  assert(argv);
  argv[0]=strtok(str,STEP);
  if(argv[0] == NULL) return -1;
  int i=1;
  while(argv[i++] = strtok(NULL,STEP));

  return 0;
}

void Print(char ** argv)
{
  int i=0;
  while(argv[i]) printf("%s\n",argv[i++]);
}

void Print_Env()
{
  extern char** environ;
  int i;
  for(i=0;environ[i];++i)
  {
    printf("%d:%s \n",i,environ[i]);
  }
  printf("\n");

}


int main()
{
  int last_process_retval=0;
  char myenv[32][256];
  int index=0;
  char commandstr[MAX]={0};
  char* argv[NUM]={NULL};
  while(1)
  {
    printf("[leo@mymachine shell]#");
    char* s=fgets(commandstr,sizeof(commandstr),stdin);
    assert(s);
    (void)s; //保证在release方式发布的时候，因为去掉assert了，所以s就没有被使用，而带来的编译告警, 什么都没做，但是充当一次使用
    commandstr[strlen(commandstr)-1]='\0';
   // printf("%s\n",commandstr);  
   int ret=splitc(commandstr,argv);
   if(ret!=0) continue;
   // Print(argv);
   
   // 一些内键指令，这些指令是 bash 这个进程自己执行的，不是创建子进程
   if(strcmp(argv[0],"env") == 0)
   {
     Print_Env();
     continue;
   }
   else if(strcmp(argv[0],"export") == 0)
   {
     if(argv[1] != NULL)
     {
       strcpy(myenv[index],argv[1]);
       putenv(myenv[index++]);
     }
     continue;

   }
   else if(strcmp(argv[0],"cd") == 0)
   {
     if(argv[1]) chdir(argv[1]);
     continue;
   }
   else if(strcmp(argv[0],"echo") == 0)
   {
     const char* tmp_env=NULL;
     if(argv[1][0] == '$')
     {
       if(argv[1][1] == '?') // 上一个进程的返回值
       {
        // printf("%d\n",last_process_retval);
         printf("exit code:%d,error information:%s \n",last_process_retval,strerror(last_process_retval));
         continue;
       }
       else tmp_env=getenv(argv[1]+1);  
     }

     if(tmp_env) printf("%s=%s \n",argv[1]+1,tmp_env);
     
     continue;
   }
   
   // ls指令文件夹颜色问题
   if(strcmp(argv[0],"ls") == 0)
   {
     int pos=0;
     while(argv[pos]) pos++;
     argv[pos++]=(char*)"--color=auto";
     argv[pos]=NULL;
   }

   
   pid_t id=fork();
   assert(id >= 0);
   (void)id;
   if(id == 0)
   {
     execvp(argv[0],argv);
     exit(1);
   }
   int status=0;
   pid_t wait=waitpid(id,&status,0);
   //printf("%d\n",status);
   if(wait > 0)
   {
     last_process_retval=WEXITSTATUS(status);
   }

  }

  return 0;
}
