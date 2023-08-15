#include<iostream>
#include<memory>
#include"EpollServer.hpp"

using namespace std;

Response CalculateHelper(const Request &req)
{
    // req是有具体数据的！
    // _result(result), _code(code)
    Response resp(0, 0);
    switch (req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;
        break;
    case '*':
        resp._result = req._x * req._y;
        break;
    case '/':
        if (req._y == 0)
            resp._code = 1;
        else
            resp._result = req._x / req._y;
        break;
    case '%':
        if (req._y == 0)
            resp._code = 2;
        else
            resp._result = req._x % req._y;
        break;
    default:
        resp._code = 3;
        break;
    }

    cout<<"CalculateHelper done:"<<resp._result <<endl;
    return resp;
}


int main()
{
    std::unique_ptr<EpollServer>  es(new EpollServer(CalculateHelper));
    es->InitServer();
    es->Dispatcher();

    return 0;
}