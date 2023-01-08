#include"proc.h"
#include<unistd.h>
#include<string.h>

#define SIZE 101
#define STYLE '#' 

void process()
{
  char tmp[SIZE];
  memset(tmp,'\0',sizeof(tmp));
  int i=0;
  for(;i<=100;i++)
  {
    printf("[%-100s][%d%%]\r",tmp,i);
    tmp[i]=STYLE;
    if(i!=99) tmp[i+1]='>';
    fflush(stdout);
    usleep(100000);
  }
  printf("\n");

}
