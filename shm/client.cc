#include"comm.hpp"

int main()
{
    Init init(CLIENT);
    char *start = init.getstart();

    char c = 'A';

    while (c <= 'Z')
    {
        start[c - 'A'] = c;
        c++;
        start[c] = '\0';
        sleep(1);
    }

    // key_t key = getkey();
    // cout << "create client key" << key << endl;

    // int shmid=GetShm(key,SIZE);
    // cout<<"create client shmid"<<shmid<<endl;

    // char *start = AttachShm(shmid);

    // DetachShm(start);

    return 0;
}