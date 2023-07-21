#include "TcpServer.hpp"
#include <memory>
using namespace tcpserver_ns;

// ./calserver 8888
Response calculate(const Request &req)
{
    // 走到这里，一定保证req是有具体数据的！
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

    return resp;
}

int main()
{
    uint16_t port = 8888;
    std::unique_ptr<TcpServer> tsvr(new TcpServer(calculate, port)); // TODO
    tsvr->InitServer();
    tsvr->Start();

    return 0;
}