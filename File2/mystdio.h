#include<stdio.h>

#define NUM 1024

// 定义缓冲方式
#define BUF_NONE 0x1
#define BUF_LINE 0x2
#define BUF_ALL  0x4



typedef struct _MY_FILE
{
  int fd;
  char BUFFER[NUM];
  int current; // 写到BUFFER的哪个位置了
  int flag;   // 什么方式缓冲
}MY_FILE;


int my_fclose(MY_FILE *fp);
int my_fflush(MY_FILE *stream);
MY_FILE *my_fopen(const char *path, const char *mode);
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb,MY_FILE *stream);
