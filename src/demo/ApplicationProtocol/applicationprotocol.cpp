/*
 * applicationprotocol.cpp
 * Copyright (C) 2019 pzy <pzy@pzy-ThinkPad-Edge-E440>
 *
 * Distributed under terms of the MIT license.
 */

#include "applicationprotocol.hpp"
using std::string;
using std::vector;
using std::pair;

const unsigned char ApplicationProtocol::REGISTER=1;
const unsigned char ApplicationProtocol::LOGIN=2;
const unsigned char ApplicationProtocol::QUIT=3;
const unsigned char ApplicationProtocol::GET_ROOMS=4;
const unsigned char ApplicationProtocol::BUILD_ROOM=5;
const unsigned char ApplicationProtocol::CLOSE_ROOM=6;
const unsigned char ApplicationProtocol::CHANGE_ROOM_ADMIN=7;
const unsigned char ApplicationProtocol::JOIN_ROOM=8;
const unsigned char ApplicationProtocol::QUIT_ROOM=9;
const unsigned char ApplicationProtocol::SEND_MESSAGE_IN_ROOM=10;
const unsigned char ApplicationProtocol::GET_USERS_IN_ROOM=11;
const unsigned char ApplicationProtocol::PRIVATE_MESSAGE=12;
const unsigned char ApplicationProtocol::KICK_USER=13;
const unsigned char ApplicationProtocol::SUCCESS=14;
const unsigned char ApplicationProtocol::FAILURE=15;

ApplicationProtocol::ApplicationProtocol(string str):Database(str)
{
}

ApplicationProtocol::ApplicationProtocol():Database()
{
}
ApplicationProtocol::~ApplicationProtocol()
{
    this->~Database();
}

string ApplicationProtocol::GetStr(const string& command, size_t& i)
{
    string str="";
    while(i<command.size()&&command[i]!='\n')
    {
        str+=command[i];
        i++;
    }
    i++;
    return str;
}

vector<pair<string,string>> ApplicationProtocol::Exec(string command)
{
    vector<pair<string,string>> rtn;
    if(command.size()<=3) return rtn;

    unsigned int length=0;
    length=(((unsigned int)command[0] )<<8 )+(unsigned int)command[1];
    if(command.size()!=length+2)//message incomplete!!!
    {
        ;
    }else
    {
        ;
    }

    Data_user user;
    string str;
    size_t i=3;
    switch (command[2])//operation command
    {
        case REGISTER:

            user.account=GetStr(command,i);
            user.passwd=GetStr(command,i);
            user.nickname=GetStr(command,i);

            Register(rtn,user);
            break;

        case LOGIN:
            user.account=GetStr(command,i);
            user.passwd=GetStr(command,i);

            break;


        case QUIT:

            break;


        case GET_ROOMS:

            break;


        case BUILD_ROOM:

            break;


        case CLOSE_ROOM:

            break;


        case CHANGE_ROOM_ADMIN:

            break;


        case JOIN_ROOM:

            break;


        case QUIT_ROOM:

            break;


        case SEND_MESSAGE_IN_ROOM:

            break;


        case GET_USERS_IN_ROOM:

            break;


        case PRIVATE_MESSAGE:

            break;


        case KICK_USER:

            break;


        case SUCCESS:

            break;


        case FAILURE:

            break;
    };
    return rtn;
}


/*
return 0 for success, -1 for failure.
*/
int ApplicationProtocol::Register(vector<pair<string,string>>& rtn,\
        Data_user user)
{
    pair<string,string> tmp;
    unsigned int status=AddUser(user);

    tmp.first=user.account;
    tmp.second=ErrorHandle(status);
    rtn.push_back(tmp);

    if(status!=Database::SUCCESS)
    {
        return -1;
    }else
    {
        return 0;
    }
    return 0;

}
