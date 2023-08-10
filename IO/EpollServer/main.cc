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

    return resp;
}

void Calculate(Connection *conn, const Request &req)
{
    Response resp = CalculateHelper(req);
    std::string sendStr;
    resp.Serialize(&sendStr);
    sendStr = protocol_ns::AddHeader(sendStr);
    // 该怎么发送呢？？
    // epoll中，关于fd的读取，一般要常设置(一直要让epoll关心的)
    //  关于fd的写入，一般是按需设置(不能常设置)，只有需要发送的时候，才设置！！
    // version1版本 -- debug
    conn->outbuffer_ += sendStr;
    // 开启对write 事件的关心
    conn->R_->EnableReadWrite(conn, true, true); // 初次设置对写事件的关心，对应的fd会立马触发一次就绪(发送buffer一定是空的)
}

int main()
{
    std::unique_ptr<EpollServer>  es(new EpollServer(Calculate));
    es->InitServer();
    es->Dispatcher();

    return 0;
}