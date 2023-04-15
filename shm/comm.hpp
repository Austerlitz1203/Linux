#ifndef __COMM_HPP__
#define __COMM_HPP

#include<iostream>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<cerrno>
#include<cstring>
#include<sys/stat.h>
#include<cassert>
#include<unistd.h>

using namespace std;

#define PATH "."
#define PROJID 0x1111
#define SIZE 4096
#define CLIENT 0
#define SERVER 1


key_t getkey()
{
    key_t k=ftok(PATH,PROJID);
    if(k == -1)
    {
        cout<<"ftok error:"<<errno<<strerror(errno)<<endl;
        exit(1);
    }
    return k;
}

static int tocreateshm(key_t k,int size,int flag)
{
    int shmid=shmget(k,size,flag);
    if(shmid == -1)
    {
        cout<<"shmget error:"<<errno<<strerror(errno)<<endl;
        exit(2);
    }
    return shmid;
}

int CreateShm(key_t k,int size)
{
    umask(0);
    return tocreateshm(k,size,IPC_CREAT | IPC_EXCL | 0666);
}

int GetShm(key_t k,int size)
{
    return tocreateshm(k,size,IPC_CREAT);
}

void DelShm(int shmid)
{
    int n=shmctl(shmid,IPC_RMID,nullptr);
    assert(n != -1);
}

char * AttachShm(int shmid)
{
    char* start=(char*)shmat(shmid,nullptr,0);
    return start;
}

void DetachShm(char* start)
{
    int n=shmdt(start);
    assert(n != -1);
    (void)n;
}

class Init
{
public:
    Init(int t)
        :type(t)
    {
        key_t key=getkey();
        if(type == SERVER)
              shmid=CreateShm(key,SIZE);
        else  shmid=GetShm(key,SIZE);
        start=AttachShm(shmid);
    }

    char* getstart()
    {
        return start;
    }

    ~Init()
    {
        DetachShm(start);
        if(type == SERVER) DelShm(shmid);
    }

private:
    int type;
    char* start;
    int shmid;
};



#endif // !
