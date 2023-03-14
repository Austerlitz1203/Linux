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




int main()
{
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
   
   pid_t id=fork();
   assert(id >= 0);
   (void)id;
   if(id == 0)
   {
     execvp(argv[0],argv);
     exit(1);
   }
   int status=0;
   waitpid(id,&status,0);

  }

  return 0;
}
