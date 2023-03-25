#include"mystdio.h"
#include<string.h>
#include<unistd.h>

#define LOG "log.txt"

int main()
{
  MY_FILE* mf=my_fopen(LOG,"w");
  if(mf == NULL)
  {
    return 1;
  }

  const char* str="hello world!";
  int cnt=10;
  while(cnt)
  {
    char buff[1024];
    //snprintf(buff,sizeof(buff),"%s:%d\n",str,cnt--);
    snprintf(buff,sizeof(buff),"%s:%d\n",str,cnt--);
    size_t size=my_fwrite(buff,strlen(buff),1,mf);
    sleep(1);
    printf("当前输入了%lu个字符，cnt:%d\n",size,cnt);

    if(cnt%5 == 0){

    }

  }

  my_fclose(mf);
  return 0;
}
