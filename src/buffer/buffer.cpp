/*
 * Buffer.cpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 */

#include "buffer.hpp"
#include <iostream>

using std::string;
using std::vector;

Buffer::Buffer()
{
    pool="";
    Log("Buffer initilizing...",1);
}

Buffer::~Buffer()
{
    pool="";
    Log("Buffer quit.",1);
}

vector<string> Buffer::Pour(string str)
{
    Log("Buffer Pour str with size of "+std::to_string(str.size())+".",1);
    pool=pool+str;
    vector<string> rtn;
    while(pool.size()>3)
    {
        unsigned int len=( ((unsigned int)pool[0])<<8 )+((unsigned int)pool[1]);
        if(pool.size()-2<len)
        {
            break;
        }
        else
        {
            rtn.push_back(pool.substr(0,2+len));
            pool=pool.substr(2+len);
        }
    }
    Log("Buffer Pour generates messages with size of "+std::to_string(rtn.size())+".",1);
    return rtn;
}


