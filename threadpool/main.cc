#include "Task.hpp"
#include "ThreadPool_V4.hpp"
#include <memory>
#include <time.h>

const char *op = "+-*/%";

int main()
{
    //std::unique_ptr<ThreadPool<Task>> up(new ThreadPool<Task>(20));
    //up->init();
    //up->run();


    // srand(time(nullptr));
    //  while(true)
    //  {
    //      int x=rand()%100+1;
    //      int y=rand()%100+1;
    //      char pos=op[(x+y)%5];

    //     Task t(x,y,pos);
    //     up->pushTask(t);
    //     cout<<"pushTask:"<<t.formatArg()<<endl;

    //     //sleep(1);

    // }

    printf("0X%x\n", ThreadPool<Task>::GetInstance());
    printf("0X%x\n", ThreadPool<Task>::GetInstance());
    printf("0X%x\n", ThreadPool<Task>::GetInstance());
    printf("0X%x\n", ThreadPool<Task>::GetInstance());
    printf("0X%x\n", ThreadPool<Task>::GetInstance());
    printf("0X%x\n", ThreadPool<Task>::GetInstance());

    while (true)
    {
        int x, y;
        char op;
        std::cout << "please Enter x> ";
        std::cin >> x;
        std::cout << "please Enter y> ";
        std::cin >> y;
        std::cout << "please Enter op(+-*/%)> ";
        std::cin >> op;

        Task t(x, y, op);

        ThreadPool<Task>::GetInstance()->pushTask(t);
    }
    return 0;
}