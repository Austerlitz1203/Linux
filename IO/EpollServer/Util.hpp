#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

class Util
{
public:
    static bool SetNonBlock(int fd)
    {
        int fl = fcntl(fd, F_GETFL);
        if (fl < 0)
            return false;
        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
        return true;
    }


    // 输入: const &
    // 输出: *
    // 输入输出: &
    static bool StringSplit(const string &str, const string &sep, vector<std::string> *result)
    {
        size_t start = 0;
        // + 20
        // "abcd efg" -> for(int i = 0; i < 10; i++) !=  for(int i = 0; i <= 9; i++)
        while (start < str.size())
        {
            auto pos = str.find(sep, start);
            if (pos == string::npos)
                break;
            result->push_back(str.substr(start, pos - start));
            // 位置的重新reload
            start = pos + sep.size();
        }
        if (start < str.size())
            result->push_back(str.substr(start));
        return true;
    }

    static int toInt(const std::string &s)
    {
        return atoi(s.c_str());
    }
};