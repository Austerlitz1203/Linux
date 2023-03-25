#include"mystdio.h"
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<malloc.h>
#include<assert.h>
#include<stdlib.h>




int my_fclose(MY_FILE *fp)
{
  assert(fp);
  // 冲刷缓冲区，这是语言层面上的
  if(fp->current > 0) my_fflush(fp);
  // 关闭文件，close
  close(fp->fd); 
  // free 开辟的 fp
  free(fp);
  fp=NULL;

  return 0;
}

int my_fflush(MY_FILE *stream)
{
  // 把缓冲区的数据，冲刷给操作系统，让操作系统决定刷新策略
  write(stream->fd,stream->BUFFER,stream->current-1);
  stream->current=0;

  return 0;
}

MY_FILE *my_fopen(const char *path, const char *mode)
{
  // 1.识别标志位
  int flag=0; // 打开方式，位图存储
  if(strcmp(mode,"r") == 0) flag|= O_RDONLY;
  else if(strcmp(mode,"w") == 0) flag |= O_CREAT | O_WRONLY | O_TRUNC;
  else if(strcmp(mode,"x") == 0) flag |= O_WRONLY | O_CREAT | O_APPEND;
  else{ // r+  w+ x+ ……

  }

  // 2.打开文件
  mode_t m=0666;  // 默认权限
  int fd=0;
  if(flag & O_CREAT) fd=open(path,flag,m);
  else fd=open(path,flag);

  if(fd < 0) return NULL; // 打开失败

  // 3.构建给用户返回的 MY_FILE 对象，并初始化
  MY_FILE *mf=(MY_FILE*)malloc(sizeof(MY_FILE));
  if(mf == NULL){
    return NULL; // 也是打开文件失败
  }
  mf->fd=fd;
  mf->current=0;
  mf->flag=BUF_LINE; // 行缓冲
  memset(mf->BUFFER,'\0',sizeof(mf->BUFFER));

  // 4.返回
  return mf;
}


// 返回写了多少个字节
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb,MY_FILE *stream)
{
  assert(ptr);
  // 缓冲区满了，直接刷新
  if(stream->current == NUM) my_fflush(stream);

  // 缓冲区没有满，根据缓冲区情况拷贝即可
  size_t freesize=NUM-stream->current; // 剩余字节数
  size_t writesize=size*nmemb;         // 写入字节数
  
  size_t ret=0;

  if(freesize >= writesize) {
    memcpy(stream->BUFFER+stream->current,ptr,writesize);
    stream->current+=writesize;
    ret+=writesize;
  }  
  else{ //写入能写的
    memcpy(stream->BUFFER+stream->current,ptr,freesize);
    stream->current=NUM;
    ret+=freesize;

  }

  // 计划刷新
  if(stream->flag & BUF_ALL){
    if(stream->current == NUM) my_fflush(stream);
  }
  else if(stream->flag & BUF_LINE){
    if(stream->BUFFER[stream->current -1 ] == '\0') my_fflush(stream);
  }
  else{

  }

  return ret;
}
