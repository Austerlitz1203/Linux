#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

class Util
{
public:
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