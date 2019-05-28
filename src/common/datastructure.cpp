/*
 * datastructure.cpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 */

#include "datastructure.hpp"


using std::string;

int Log(string str, int i=-1)
{
    while(str.back()=='\n')
    {
        str.pop_back();
    }
    FILE* fp;
    string num=std::to_string(Log_Num++);
    while(num.size()<8)
    {
        num="0"+num;
    }
    num="["+num+"]";
    switch(i)
    {
        case -1:
            return 0;

        case 1:
            fp=stdout;

            str=num+"\033[32m"+str+"\n\033[0m";
            break;

        case 2:
            fp=stderr;
            str=num+"\033[31m"+str+"\n\033[0m";
            break;
    }
    fprintf(fp,"%s",str.c_str());
    return 0;
}


