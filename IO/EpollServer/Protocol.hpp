#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <jsoncpp/json/json.h>

#include "Util.hpp"

// #define MYSELF 1

// 给网络版本计算机定制协议
namespace protocol_ns
{
#define SEP " "
#define SEP_LEN strlen(SEP) // 不能写成sizeof
#define HEADER_SEP "\r\n"
#define HEADER_SEP_LEN strlen("\r\n")

    // "长度"\r\n""_x _op _y"\r\n
    // "10 + 20" => "7"\r\n""10 + 20"\r\n => 报头 + 有效载荷
    // 请求/响应 = 报头\r\n有效载荷\r\n

    // "10 + 20" => "7"\r\n""10 + 20"\r\n
    string AddHeader(const string &str)
    {
        cout << "AddHeader 之前:\n"
             << str << std::endl;
        string s = std::to_string(str.size());
        s += HEADER_SEP;
        s += str;
        s += HEADER_SEP;

        cout << "AddHeader 之后:\n"
             << s << std::endl;

        return s;
    }
    // "7"\r\n""10 + 20"\r\n => "10 + 20"
    string RemoveHeader(const string &str, int len)
    {
        cout << "RemoveHeader 之前:\n"
             << str << endl;

        string res = str.substr(str.size() - HEADER_SEP_LEN - len, len);

        cout << "RemoveHeader 之后:\n"
             << res << endl;

        return res;
    }

    int ParsePackage(string &inbuffer, string *package)
    {

        // 分析， "7"\r\n""10 + 20"\r\n
        auto pos = inbuffer.find(HEADER_SEP);
        if (pos == string::npos)
            return 0;                                                    // inbuffer什么都没有动
        string lenStr = inbuffer.substr(0, pos);                         // 获取了头部字符串, inbuffer什么都没有动
        int len = Util::toInt(lenStr);                                   // "123" -> 123 inbuffer什么都没有动
        int targetPackageLen = lenStr.size() + len + 2 * HEADER_SEP_LEN; // inbuffer什么都没有动
        if (inbuffer.size() < targetPackageLen)
            return 0;                                    // inbuffer什么都没有动
        *package = inbuffer.substr(0, targetPackageLen); // 提取到了报文有效载荷，inbuffer可什么都没有动
        inbuffer.erase(0, targetPackageLen);             // 从inbuffer中直接移除整个报文

        return len;
    }

    // Request && Response都要提供序列化和反序列化功能
    // 1. 自己手写
    // 2. 用别人写的
    class Request
    {
    public:
        Request() {}
        Request(int x, int y, char op) : _x(x), _y(y), _op(op)
        {
        }

        // struct->string
        // 目前: "_x _op _y"
        bool Serialize(string *outStr)
        {
            *outStr = "";
#ifdef MYSELF

            string x_string = std::to_string(_x);
            string y_string = std::to_string(_y);

            // 手动序列化
            *outStr = x_string + SEP + _op + SEP + y_string;
#else
            Json::Value root; // Value: 一种万能对象, 接受任意的kv类型

            root["x"] = _x;
            root["y"] = _y;
            root["op"] = _op;
            // Json::FastWriter writer; // Writer：是用来进行序列化的 struct -> string
            Json::StyledWriter writer;
            *outStr = writer.write(root);
#endif
            return true;
        }

        // string->struct
        bool Deserialize(const string &inStr)
        {
            // inStr : 10 + 20 => [0]=>10, [1]=>+, [2]=>20
            // string -> vector
#ifdef MYSELF

            std::vector<string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 3)
                return false;
            if (result[1].size() != 1)
                return false;
            _x = Util::toInt(result[0]);
            _y = Util::toInt(result[2]);
            _op = result[1][0];
#else
            Json::Value root;
            Json::Reader reader; // Reader: 用来进行反序列化的
            reader.parse(inStr, root);

            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _op = root["op"].asInt();
#endif
            return true;
        }
        ~Request() {}

    public:
        // _x op _y ==> 10 / 0 ? ==> 10 * 9 ?
        int _x;
        int _y;
        char _op;
    };

    class Response
    {
    public:
        Response() {}
        Response(int result, int code) : _result(result), _code(code)
        {
        }
        // struct->string
        bool Serialize(string *outStr)
        {
            //_result _code
            *outStr = "";
#ifdef MYSELF
            string res_string = std::to_string(_result);
            string code_string = std::to_string(_code);

            *outStr = res_string + SEP + code_string;
#else
            Json::Value root;
            root["result"] = _result;
            root["code"] = _code;

            // Json::FastWriter writer;
            Json::StyledWriter writer;

            *outStr = writer.write(root);
#endif
            return true;
        }
        // string->struct
        bool Deserialize(const string &inStr)
        {
            // 10 0, 10 1
#ifdef MYSELF
            std::vector<string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 2)
                return false;

            _result = Util::toInt(result[0]);
            _code = Util::toInt(result[1]);
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(inStr, root);
            _result = root["result"].asInt();
            _code = root["code"].asInt();
#endif
            return true;
        }
        ~Response() {}

    public:
        int _result;
        int _code; // 0 success, 1,2,3,4代表不同的错误码
    };
}