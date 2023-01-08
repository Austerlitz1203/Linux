#include"proc.h"
#include<unistd.h>
#include<string.h>

#define SIZE 101
#define STYLE '#' 

void process()
{
  char tmp[SIZE];
  char wait[5]="|/-\\";
  memset(tmp,'\0',sizeof(tmp));
  int i=0;
  for(;i<=100;i++)
  {
    printf("[\033[0;32;31m%-100s\033[m][%d%%][%c]\r",tmp,i,wait[i%4]);
    tmp[i]=STYLE;
    if(i!=99) tmp[i+1]='>';
    fflush(stdout);
    usleep(100000);
  }
  printf("\n");

}
