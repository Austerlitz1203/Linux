#include "Task.hpp"
#include "ThreadPool.hpp"
#include <memory>
#include <time.h>

const char *op = "+-*/%";

int main()
{
    std::unique_ptr<ThreadPool<Task>> up(new ThreadPool<Task>(20));
    up->start();

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

        up->pushTask(t);
    }
    return 0;
}