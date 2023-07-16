#include"udp_server.hpp"
#include<memory>


static void usage(string proc)
{
    cout << "Usage:\n\t" << proc << " port\n"
         << endl;
}

string transactionString(string request) // request 就是一个string
{
    string result;
    char c;
    for (auto &r : request)
    {
        if (islower(r))
        {
            c = toupper(r);
            result.push_back(c);
        }
        else
        {
            result.push_back(r);
        }
    }

    return result;
}

static bool isPass(const std::string &command)
{
    bool pass = true;
    auto pos = command.find("rm");
    if (pos != std::string::npos)
        pass = false;
    pos = command.find("mv");
    if (pos != std::string::npos)
        pass = false;
    pos = command.find("while");
    if (pos != std::string::npos)
        pass = false;
    pos = command.find("kill");
    if (pos != std::string::npos)
        pass = false;
    return pass;
}

// 在本地把命令给server，server再把结果给你！
// ls -a -l
string excuteCommand(string command) // command就是一个命名
{
    // 1. 安全检查
    if (!isPass(command))
        return "you are bad man!";

    // 2. 业务逻辑处理
    FILE *fp = popen(command.c_str(), "r");
    if (fp == nullptr)
        return "None";

    // 3. 获取结果了
    char line[1024];
    string result;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        result += line;
    }
    pclose(fp);

    return result;
}


using namespace std;

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    u_int16_t port=atoi(argv[1]);
    // us_server::UdpServer us(transactionString,port);
    // us.InitServer();
    // us.StartServer();

    unique_ptr<us_server::UdpServer> us(new us_server::UdpServer(excuteCommand,port));
    us->InitServer();
    us->StartServer();

    return 0;
}
