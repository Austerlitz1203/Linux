#include<iostream>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

using namespace std;

void handler(int signo)
{
  cout<<"我的进程收到了："<<signo<<"信号导致崩溃"<<endl;
  exit(100);
}



void test1()
{
  signal(SIGFPE,handler);
  int a=10;
  a/=0;
  cout<<"10 div 0 here"<<endl;
}


void test2()
{
  pid_t id=fork();
  if(id == 0)
  {
    signal(SIGSEGV,handler);
    cout<<"野指针问题....here"<<endl;
    cout<<"野指针问题....here"<<endl;
    cout<<"野指针问题....here"<<endl;

    int* p=nullptr;
    //p=100;
    *p=100;

    cout<<"野指针问题....here"<<endl;
    cout<<"野指针问题....here"<<endl;
    cout<<"野指针问题....here"<<endl;
    cout<<"野指针问题....here"<<endl;
    exit(10086);
  } 

  int status=0;
  waitpid(id,&status,0);
  cout<<"exit code:"<<((status>>8)&0xff)<<endl;
  cout<<"exit signal:"<<(status&0x7F)<<endl;
  cout<<"code drump flag:"<<((status>>7)&0x1)<<endl;
  
  while(true)
  {
    cout<<"我是一个正常的进程，正在模拟某种异常:"<<getpid()<<endl;
    sleep(1);
  }
}


void showBlock(sigset_t* oset)
{
  int signo=1;
  for(;signo<=31;signo++)
  {
    if(sigismember(oset,signo)) cout<<"1";
    else cout<<"0";
  }
  cout<<endl;
}

void test3()
{ 
 // 只是在用户层面进行设置
 sigset_t set,oset;
 sigemptyset(&set);
 sigemptyset(&oset);
 sigaddset(&set,2);

 // 设置进入进程，谁调用，设置shei
 sigprocmask(SIG_SETMASK,&set,&oset);
 int cnt=0;
 while(true)
 {
    showBlock(&oset);
    sleep(1);
    cnt++;
    if(cnt == 10)
    {
      cout<<"remove block"<<endl;
      sigprocmask(SIG_SETMASK,&oset,&set);
      showBlock(&set);
      exit(1);
    }
 }

}

int main()
{
  //test1(); // 测试硬件中断
  //test2(); // 测试野指针问题
  test3(); // 测试信号集函数

  return 0;
}
