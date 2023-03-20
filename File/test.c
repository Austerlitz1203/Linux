#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>



#define LOG "log.txt"
void Test1()
{
  umask(0);
  int fd=open(LOG,O_RDONLY | O_CREAT,0666);
  if(fd == -1)
  {
    printf("fd:%d,errno:%d ,errstring:%s \n",fd,errno,strerror(errno));
  }
  else printf("fd:%d errno:%d ,errstring:%s \n",fd,errno,strerror(errno)); 
  
  char buffer[1024];
  
  size_t num=read(fd,buffer,sizeof(buffer)-1);
  if(num > 0)
  {
    buffer[num]='\0';
    printf("%s\n",buffer);
  }
  
  close(fd);

}



void Test2()
{
  //int fd=open(LOG,O_WRONLY | O_CREAT | O_APPEND);

  int fd=open(LOG,O_WRONLY | O_CREAT);
  const char* str="abcdefghi";
  int cnt=5;

  while(cnt--)
  {
    char line[128];
    snprintf(line,sizeof(line),"%d,%s\n",cnt,str);

    write(fd,line,strlen(line));  // 这里不用 strlen(line) + 1 ，因为 \0 是C语言规定的，但是对于系统没有这个规定
    //write(fd,line,sizeof(line));// 注意和上一行的区别，这里会有乱码。
  }
  close(fd);


}


void Test3()
{
  int fd1 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd2 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd3 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd4 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd5 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd6 = open(LOG, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  printf("%d\n",fd1);
  printf("%d\n",fd2);
  printf("%d\n",fd3);
  printf("%d\n",fd4);
  printf("%d\n",fd5);
  printf("%d\n",fd6);

  close(fd1);
  close(fd2);
  close(fd3);
  close(fd4);
  close(fd5);
  close(fd6);

}


void Test4()
{
  printf("%d\n",stdin->_fileno);
  printf("%d\n",stdout->_fileno);
  printf("%d\n",stderr->_fileno);

  FILE* fp=fopen(LOG,"w");
  printf("%d\n",fp->_fileno);


}


#define LOG_NORMAL "log-normal.txt"
#define LOG_ERROR "log-error.txt"


void Test5()
{
  umask(0);

  close(1);
  open(LOG_NORMAL,O_WRONLY | O_CREAT | O_APPEND,0666);

  close(2);
  
  open(LOG_ERROR,O_WRONLY | O_CREAT | O_APPEND,0666);

  printf("test printf->stdout\n");
  printf("test printf->stdout\n");
  printf("test printf->stdout\n");
  printf("test printf->stdout\n");
  printf("test printf->stdout\n");
  printf("test printf->stdout\n");
  

  fprintf(stdout,"test fprintf->stdout\n");
  fprintf(stdout,"test fprintf->stdout\n");
  fprintf(stdout,"test fprintf->stdout\n");
  fprintf(stdout,"test fprintf->stdout\n");
  fprintf(stdout,"test fprintf->stdout\n");
  fprintf(stdout,"test fprintf->stdout\n");


  fprintf(stderr,"test fprintf->stderr\n");
  fprintf(stderr,"test fprintf->stderr\n");
  fprintf(stderr,"test fprintf->stderr\n");
  fprintf(stderr,"test fprintf->stderr\n");
  fprintf(stderr,"test fprintf->stderr\n");
  fprintf(stderr,"test fprintf->stderr\n");

}

void Test6()
{
  int fd=open(LOG_NORMAL,O_WRONLY | O_CREAT |O_APPEND,0666);
  if(fd < 0)
  {
    perror("open");
    return;
  }
  dup2(fd,1); // 1就是标准输出流的文件描述符,将1原本指向的标准输出文件，改成和文件描述符fd指向的一样
  printf("This is a test for dup2()\n");
  
  close(fd);

}


int main()
{
  //Test1();// 测试 read
  //Test2(); // 测试 write
  //Test3(); // 文件描述符
  //Test4();
  //Test5(); // 关闭标准流，将 printf \fprintf 输出到标准流的内容，输出到指定文件
  Test6(); // Test5() 的方法不好，可以利用 dup 函数，将文件描述符指向的文件改变（比如1原本指向标准输出，改成指向另一个文件）
  return 0;
}
