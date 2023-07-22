#include "TcpClient.hpp"
#include "Sock.hpp"
#include "Protocol.hpp"

#include <iostream>
#include <string>

using namespace protocol_ns;

static void usage(string proc)
{
    cout << "Usage:\n\t" << proc << " serverip serverport\n"
              << endl;
}

enum
{
    LEFT,
    OPER,
    RIGHT
};

// 10+20
Request ParseLine(const std::string &line)
{
    string left, right;
    char op;
    int status = LEFT;

    int i = 0;
    while (i < line.size())
    {
        // if(isdigit(e)) left.push_back;

        switch (status)
        {
        case LEFT:
            if (isdigit(line[i]))
                left.push_back(line[i++]);
            else
                status = OPER;
            break;
        case OPER:
            op = line[i++];
            status = RIGHT;
            break;
        case RIGHT:
            if (isdigit(line[i]))
                right.push_back(line[i++]);
            break;
        }
    }

    Request req;
    cout << "left: " << left << endl;
    cout << "right: " << right << endl;
    cout << "op: " << op << endl;

    req._x = std::stoi(left);
    req._y = std::stoi(right);
    req._op = op;

    return req;
}

// ./tcpclient serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    Sock sock;
    sock.Socket();

    int n = sock.Connect(serverip, serverport);
    if (n != 0)
        return 1;

    string buffer;
    while (true)
    {
        cout << "Enter# "; // 1+1,2*9
        string line;
        getline(cin, line);

        Request req = ParseLine(line);

        cout << "test: " << req._x << req._op << req._y << endl;

        // 1. 序列化
        string sendString;
        req.Serialize(&sendString);
        // 2. 添加报头
        sendString = AddHeader(sendString);
        // 3. send
        send(sock.Fd(), sendString.c_str(), sendString.size(), 0);
        // 4. 获取响应
        string package;
        int n = 0;
    START:
        n = ReadPackage(sock.Fd(), buffer, &package);
        if (n == 0)
            goto START;
        else if (n < 0)
            break;
        else
        {
        }

        // 5. 去掉报头
        package = RemoveHeader(package,n);
        // 6. 反序列化
        Response resp;
        resp.Deserialize(package);

        cout << "result: " << resp._result << "[code: " << resp._code << "]" << endl;
    }

    sock.Close();
    return 0;
}