#include"comm.hpp"

int main()
{
    Init init(SERVER);
    char* start=init.getstart();

    int n = 0;
    while (n <= 30)
    {
        cout << "client -> server# " << start << endl;
        sleep(1);
        n++;
    } 

    // key_t key=getkey();
    // cout<<"create server key"<<key<<endl;

    // int shmid=CreateShm(key,SIZE);
    // cout<<"create server shmid"<<shmid<<endl;

    // char* start=AttachShm(shmid);


    // DetachShm(start);

    // DelShm(shmid);

    return 0;
}