/*
 * converter.cpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 */

#include "converter.hpp"


using std::pair;
using std::make_pair;
using std::vector;
using std::string;
using std::unordered_map;

Converter::Converter(string load):ApplicationProtocol(load)
{
    //load

    bev2acc.clear();
    acc2bev.clear();
    bev2buf.clear();
    off_msgs.clear();
}

Converter::Converter()
{
    //load

    bev2acc.clear();
    acc2bev.clear();
    bev2buf.clear();
    off_msgs.clear();
}

Converter::~Converter()
{
    //save to the database

    bev2acc.clear();
    acc2bev.clear();
    bev2buf.clear();
    off_msgs.clear();
}

vector<pair<void*,string>> Converter::ConRead(void* bev, vector<char> bytes)
{
    vector<pair<void*,string>> rtn;
    vector<pair<Account,string>> rtn_app;
    vector<string> msgs;
    string str="";
    for(auto byte:bytes) str.push_back(byte);

    if(bev2buf.find(bev)!=bev2buf.end())//online account
    {
        msgs=bev2buf[bev].Pour(str);
        for(auto msg:msgs)
        {
            rtn_app=Exec(msg);
            for(auto r:rtn_app)
            {
                if(acc2bev.find(r.first)!=acc2bev.end())//receiver online
                {
                    rtn.push_back(make_pair(acc2bev[r.first],r.second));
                }else//receiver offline, push messages into the offl_msgs
                {
                    off_msgs[r.first].push_back(r.second);
                }
            }
        }
    }else//only login is allowed.
    {
        msgs=bev2buf[bev].Pour(str);
        Account account;
        for(auto msg:msgs)
        {
            account=Login(msg);
            if(account=="")//Login failure
            {
                rtn.push_back(make_pair(bev,"Login FAILURE.\n"));
                break;
            }else//login success, get the account
            {
                bev2acc[bev]=account;
                acc2bev[account]=bev;
                rtn.push_back(make_pair(bev,"Login SUCCESSFULLY.\n"));
                //send off_msgs
                if(off_msgs.find(account)!=off_msgs.end())
                {
                    for(auto msg:off_msgs[account])
                    {
                        rtn.push_back(make_pair(bev,msg));
                    }
                    off_msgs.erase(msg);
                }
                break;
            }
        }
    }
    return rtn;
}

int Converter::ConAccept(void* bev)
{
    if(bev2buf.find(bev)!=bev2buf.end()) return -1;
    bev2buf[bev]=Buffer();
    return 0;
}

int Converter::ConError(void* bev)
{
    if(bev2buf.find(bev)==bev2buf.end()) return -1;
    Account account=bev2acc[bev];

    bev2buf.erase(bev);
    bev2acc.erase(bev);
    acc2bev.erase(account);

    off_msgs[account]=vector<string>(0,"");
    return 0;

}

