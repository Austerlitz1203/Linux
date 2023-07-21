#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "Util.hpp"

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
        string s = std::to_string(str.size());
        s += HEADER_SEP;
        s += str;
        s += HEADER_SEP;
    }
    // "7"\r\n""10 + 20"\r\n => "10 + 20"
    string RemoveHeader(const string &str)
    {
        
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
            string x_string = std::to_string(_x);
            string y_string = std::to_string(_y);

            // 手动序列化
            *outStr = x_string + SEP + _op + SEP + y_string;

            return true;
        }


        // string->struct
        bool Deserialize(const string &inStr)
        {
            // inStr : 10 + 20 => [0]=>10, [1]=>+, [2]=>20
            // string -> vector
            std::vector<string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 3)
                return false;
            if (result[1].size() != 1)
                return false;
            _x = Util::toInt(result[0]);
            _y = Util::toInt(result[2]);
            _op = result[1][0];
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
            string res_string = std::to_string(_result);
            string code_string = std::to_string(_code);

            *outStr = res_string + SEP + code_string;
            return true;
        }
        // string->struct
        bool Deserialize(const string &inStr)
        {
            // 10 0, 10 1
            std::vector<string> result;
            Util::StringSplit(inStr, SEP, &result);
            if (result.size() != 2)
                return false;

            _result = Util::toInt(result[0]);
            _code = Util::toInt(result[1]);
            return true;
        }
        ~Response() {}

    public:
        int _result;
        int _code; // 0 success, 1,2,3,4代表不同的错误码
    };
}