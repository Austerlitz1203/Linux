#include<stdio.h>


int add(int x)
{
  int ret=0;
  printf("start\n");

  for(int i=0;i<x;i++)
  {
    ret+=i;
  }
  printf("end\n");
  return ret;
}

int main()
{
  int x=100;
  int ret=add(x);
  printf("result:%d",ret);
  return 0;
}
