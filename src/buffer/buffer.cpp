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
}

Buffer::~Buffer()
{
    pool="";
}

vector<string> Buffer::Pour(string str)
{
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
    return rtn;
}


