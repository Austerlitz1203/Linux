#include "HttpServer.hpp"
#include "err.hpp"
#include "Util.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

const std::string SEP = "\r\n";
const std::string page_404 = "./wwwroot/err_404.html";

// 一般一个webserver，不做特殊说明，如果用户之间默认访问‘/’, 不能把把整站给对方
// 需要添加默认首页！而且，不能让用户访问wwwroot里面的任何一个目录本身，也可以给每一个目录都带上一个默认首页！
const string defaultHomePage = "index.html";
const string webRoot = "./wwwroot"; // web根目录

class HttpRequest
{
public:
    HttpRequest() : path_(webRoot)
    {
    }

    ~HttpRequest() {}

    void Print()
    {
        logMessage(Debug, "method: %s, url: %s, version: %s",
                   method_.c_str(), url_.c_str(), httpVersion_.c_str());
        // for(const auto & line : body_)
        //     logMessage(Debug, "-%s", line.c_str());
        logMessage(Debug, "path: %s", path_.c_str());
        logMessage(Debug, "suffix_: %s", suffix_.c_str());
    }

public:
    string method_;
    string url_;
    string httpVersion_;
    std::vector<string> body_;

    string path_;

    string suffix_;
};

HttpRequest Deserialize(string &message)
{
    HttpRequest req;
    string line = Util::ReadOneLine(message, SEP);
    Util::ParseRequestLine(line, &req.method_, &req.url_, &req.httpVersion_);

    while (!message.empty())
    {
        line = Util::ReadOneLine(message, SEP);
        req.body_.push_back(line);
    }
    // 资源路径
    req.path_ += req.url_; // "wwwroot/a/b/c.html", "./wwwroot/"
    if (req.path_[req.path_.size() - 1] == '/')
        req.path_ += defaultHomePage;

    // 资源后缀
    auto pos = req.path_.rfind(".");
    if (pos == std::string::npos)
        req.suffix_ = ".html";
    else
        req.suffix_ = req.path_.substr(pos);
    return req;
}

string GetContentType(const string &suffix)
{
    string content_type = "Content-Type: ";
    if (suffix == ".html" || suffix == ".htm")
        content_type + "text/html";
    else if (suffix == ".css")
        content_type += "text/css";
    else if (suffix == ".js")
        content_type += "application/x-javascript";
    else if (suffix == ".png")
        content_type += "image/png";
    else if (suffix == ".jpg")
        content_type += "image/jpeg";
    else
    {
    }
    return content_type + SEP;
}

class Session // redis -> Session
{
public:
    string name;
    string passwd;
    uint64_t loginTime;
    // 用户的其他信息
    int status;
    int fd;
    int sesisonid;
};

std::unordered_map<int, Session *> sessions;

// bool Login(string &message)
// {
//     // post-> 正文包含账号密码
//     string name;
//     string passwd;

//     if(check(name, passwd))
//     {
//         Session* session = new Session(name, passwd...);
//         int random = rand();
//         sessions.insert(std::pair<int, Session*>(random, session));
//     }

//     http response
//     Set-Cookie: sessionid=random;
// }

string HandlerHttp(string &message)
{
    // 1. 读取请求
    // 确信，request一定是一个完整的http请求报文
    // 给别人返回的是一个http response
    cout << "----------------------------------------" << endl;
    cout << message << endl;
    // 资源，图片(.png, .jpg...)，网页(.html, .htm)，视频(.mp3)，音频(..)->文件！都要有自己的后缀！

    // 2. 反序列化和分析请求
    HttpRequest req = Deserialize(message);
    req.Print();
    // 3. 使用请求
    string body;
    // bool ret = Util::ReadFile(req.path_, &body);
    // /a/b/c.html
    string response;
    if (true == Util::ReadFile(req.path_, &body))
    {
        response = "HTTP/1.0 500 OK" + SEP;
        response += "Content-Length: " + std::to_string(body.size()) + SEP;
        response += GetContentType(req.suffix_);
        response += SEP;
        response += body;
    }
    else
    {
        response = "HTTP/1.0 404 Not Found" + SEP;
        Util::ReadFile(page_404, &body);
        response += "Content-Length: " + std::to_string(body.size()) + SEP;
        response += GetContentType(".html");
        response += SEP;
        response += body;
    }

    return response;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<HttpServer> tsvr(new HttpServer(HandlerHttp, port)); // TODO
    tsvr->InitServer();
    tsvr->Start();

    return 0;
}