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
//const unsigned char ApplicationProtocol::GET_ROOM=16;
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
const unsigned char ApplicationProtocol::GET_ROOM=16;

ApplicationProtocol::ApplicationProtocol(string load):Database(load)
{
}

ApplicationProtocol::ApplicationProtocol():Database()
{
}
ApplicationProtocol::~ApplicationProtocol()
{
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
        return rtn;
    }else
    {
        ;
    }

    Data_user user;
    Data_room room;
    string str;
    string newadmin,id,oldadmin;
    string msg;
    size_t i=3;
    switch (command[2])//operation command
    {
        case REGISTER:

            user.account=       GetStr(command,i);
            user.passwd=        GetStr(command,i);
            user.nickname=      GetStr(command,i);

            AppRegister(rtn,user);
            break;

        case LOGIN:
            /* ignored, convertor deal with this*/
            break;


        case QUIT:
            /* ignored, convertor deal with this*/
            break;

        case GET_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            AppGetRoom(rtn,user,room);
            break;

        case GET_ROOMS:
            user.account=       GetStr(command,i);
            AppGetRooms(rtn,user);
            break;


        case BUILD_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            room.name=          GetStr(command,i);
            room.description=   GetStr(command,i);
            room.passwd=        GetStr(command,i);
            AppBuildRoom(rtn,user,room);
            break;


        case CLOSE_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            AppCloseRoom(rtn,user,room);

            break;


        //7
        case CHANGE_ROOM_ADMIN:
            oldadmin=    GetStr(command,i);
            id=          GetStr(command,i);
            newadmin=    GetStr(command,i);
            AppChangeRoomAdmin(rtn,oldadmin,id,newadmin);
            break;


        case JOIN_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            room.passwd=        GetStr(command,i);
            AppJoinRoom(rtn,user,room);
            break;


        case QUIT_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            AppQuitRoom(rtn,user,room);

            break;


        case SEND_MESSAGE_IN_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            msg=                GetStr(command,i);
            AppSendMessageInRoom(rtn,user.account,room.id,msg);
            break;


        //11
        case GET_USERS_IN_ROOM:
            user.account=       GetStr(command,i);
            room.id=            GetStr(command,i);
            AppGetUsersInRoom(rtn,user,room);
            break;


        case PRIVATE_MESSAGE:
            oldadmin=           GetStr(command,i);
            newadmin=           GetStr(command,i);
            msg=                GetStr(command,i);
            AppPrivateMessage(rtn,oldadmin,newadmin,msg);
            break;


            //13
        case KICK_USER:
            oldadmin=       GetStr(command,i);
            id=            GetStr(command,i);
            newadmin=        GetStr(command,i);
            AppKickUser(rtn,oldadmin,id,newadmin);
            break;


        case SUCCESS:

            break;


        case FAILURE:

            break;
    };
    return rtn;
}

int ApplicationProtocol::Login(string str, Account&rtn_account, string& rtn)
{

    unsigned int length=0;
    length=(((unsigned int)str[0] )<<8 )+(unsigned int)str[1];
    if(str.size()!=length+2)//message incomplete!!!
    {
        rtn="Imcomplete message.";
        return -1;
    }else
    {
        ;
    }


    size_t i=3;
    Account account=GetStr(str,i);
    Passwd passwd=GetStr(str,i);
    Nickname nickname=GetStr(str,i);
    Data_user user;
    unsigned int status;
    switch(str[2])
    {
        case LOGIN:

            status=GetUser(account,user);
            rtn=ErrorHandle(status);
            if(status==Database::SUCCESS&&user.passwd==passwd)
            {
                rtn_account=account;
                return 0;
            }
            return -1;
            break;

        case REGISTER:
            user.account=account;
            user.passwd=passwd;
            user.nickname=nickname;
            status=AddUser(user);
            rtn=ErrorHandle(status);
            if(status==Database::SUCCESS)
            {
                return 1;
            }
            else {
                return -1;
            }


    }
    return -1;
}


/*
return 0 for success, -1 for failure.
*/
int ApplicationProtocol::AppRegister(vector<pair<string,string>>& rtn,\
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

/*
 * One User get one room's information.
 *
 */
int ApplicationProtocol::AppGetRoom(vector<pair<string,string>>& rtn, Data_user user, Data_room room)
{
    pair<string,string> tmp;
    tmp.first=user.account;

    Data_room room_tmp;
    unsigned int status=GetRoom(room.id,room_tmp);
    if(Database::SUCCESS!=status)
    {
        tmp.second=ErrorHandle(status);
        return -1;
    }else
    {
        tmp.second="";
        Data_user admin;
        GetUser(room_tmp.admin,admin);
        tmp.second+=tmp.second+room_tmp.id+'\n'+room_tmp.name+'\n'+admin.nickname+'\n'+room_tmp.description+'\n';
    }
    rtn.push_back(tmp);


    return 0;
}
/*
 * One User get all rooms' information.
 *
 */
int ApplicationProtocol::AppGetRooms(vector<pair<string,string>>& rtn, Data_user user)
{
    pair<string,string> tmp;
    tmp.first=user.account;

    vector<Data_room> rooms;
    unsigned int status=GetAllRooms(rooms);
    if(Database::SUCCESS!=status)
    {
        tmp.second=ErrorHandle(status);
        return -1;
    }else
    {
        tmp.second="";
        for(auto room:rooms)
        {
            Data_user admin;
            GetUser(room.admin,admin);
            tmp.second=tmp.second+room.id+'\n'+room.name+'\n'+admin.nickname+'\n'+room.description+'\n';
        }

    }
    rtn.push_back(tmp);


    return 0;
}
int ApplicationProtocol::AppBuildRoom(vector<pair<string,string>>&rtn, Data_user user, Data_room room)
{
    pair<string,string> tmp;
    tmp.first=user.account;
    room.admin=user.account;

    tmp.second=ErrorHandle(AddRoom(room));

    rtn.push_back(tmp);

    return 0;
}
/*
 * one user join room, send messages "%nickname% joins in the room." to every other members in the room.
 * send "SUCCESS" to the user
 */
int ApplicationProtocol::AppJoinRoom(vector<pair<string,string>>& rtn,Data_user user,Data_room room)
{
    pair<string,string> tmp;
    unsigned int status;
    status=UserJoinRoom(user.account,room.id,room.passwd);

    tmp.first=user.account;
    tmp.second=ErrorHandle(status);
    rtn.push_back(tmp);

    if(status!=Database::SUCCESS)
    {
        return -1;
    }else
    {
        vector<Data_user> users;

        status=GetUsersInARoom(room.id,users);
        if(status!=Database::SUCCESS)
        {
            return -1;
        }
        status=GetUser(user.account,user);
        if(status!=Database::SUCCESS)
        {
            return -1;
        }

        tmp.second=user.nickname+" joins the room.";
        for(auto mem:users)
        {
            tmp.first=mem.account;
            rtn.push_back(tmp);
        }
        return 0;
    }
}


int ApplicationProtocol::AppGetUsersInRoom(vector<pair<string,string>>& rtn,Data_user user,Data_room room)
{
    pair<string,string> tmp;
    unsigned int status;
    tmp.first=user.account;
    vector<Data_user> users;
    status=GetUsersInARoom(room.id,users);
    if(status!=Database::SUCCESS)
    {
        return -1;
    }
    for(size_t i=0;i<users.size();i++)
    {
        if(i==0)
        {
            tmp.second="ADMIN:\n";
        }
        tmp.second+=users[i].nickname+'\t'+users[i].account+'\n';
    }
    rtn.push_back(tmp);
    return 0;

}
/*
 * change room admin, if failue send %Cause% to the user
 * if SUCCESS, send SUCCESS to the user, messages to everyone in the room.
 *
 */
int ApplicationProtocol::AppChangeRoomAdmin(vector<pair<string,string>>& rtn,Account oldadmin,Id id,Account newadmin)
{
    pair<string,string> tmp;
    unsigned int status;
    status=UserChangeRoomAdmin(oldadmin,id,newadmin);
    tmp.first=oldadmin;
    tmp.second=ErrorHandle(status);
    rtn.push_back(tmp);
    if(status==Database::SUCCESS)
    {
        Data_room room;
        GetRoom(id,room);
        vector<Data_user> users;
        status=GetUsersInARoom(room.id,users);

        if(status!=Database::SUCCESS)
        {
            return -1;
        }
        for(size_t i=0;i<users.size();i++)
        {
            tmp.first=users[i].account;
            tmp.second=users[0].nickname+" becomes the new ADMIN of room named "+room.name+".\n";
            rtn.push_back(tmp);
        }
        return 0;
    }
    return -1;
}


int ApplicationProtocol::AppQuitRoom(vector<pair<string,string>>&rtn,Data_user user,Data_room room)
{
    pair<string,string> tmp;
    unsigned int status;


    tmp.first=user.account;

    status=OneUserLeaveRoom(user.account,room.id);
    tmp.second=ErrorHandle(status);
    rtn.push_back(tmp);

    if(status==Database::SUCCESS)
    {
        Data_room room1;
        GetRoom(room.id,room1);
        Data_user leaver;
        GetUser(user.account,leaver);
        vector<Data_user> users;
        status=GetUsersInARoom(room1.id,users);

        if(status!=Database::SUCCESS)
        {
            return -1;
        }
        for(size_t i=0;i<users.size();i++)
        {
            tmp.first=users[i].account;
            tmp.second=leaver.nickname+" leaves the room named "+room1.name+".\n";
            rtn.push_back(tmp);
        }
        return 0;
    }

    return -1;
}

int ApplicationProtocol::AppCloseRoom(vector<pair<string,string>>& rtn, Data_user user, Data_room room)
{
    pair<string,string> tmp;
    unsigned int status;

    Data_room room1;
    GetRoom(room.id,room1);

    vector<Data_user> users;
    GetUsersInARoom(room1.id,users);

    tmp.first=user.account;
    status=CloseRoom(user.account,room.id);

    tmp.second=ErrorHandle(status);
    rtn.push_back(tmp);
    if(status==Database::SUCCESS)
    {

        Data_user admin_user;
        GetUser(room1.admin,admin_user);

        if(status!=Database::SUCCESS)
        {
            return -1;
        }
        for(size_t i=0;i<users.size();i++)
        {
            tmp.first=users[i].account;
            tmp.second=admin_user.nickname+" closes the room named "+room1.name+".\n";
            rtn.push_back(tmp);
        }
        return 0;
    }
    return -1;
}

int ApplicationProtocol::AppKickUser(vector<pair<string,string>>& rtn, Account admin, Id id, Account kicked)
{
    pair<string,string> tmp;
    tmp.first=admin;

    Data_room room1;
    GetRoom(id,room1);
    vector<Data_user> users;
    GetUsersInARoom(room1.id,users);

    Data_user kicked_user;
    GetUser(kicked,kicked_user);

    unsigned int status = KickUserOut(admin,id,kicked);
    tmp.second=ErrorHandle(status);

    rtn.push_back(tmp);
    if(status==Database::SUCCESS)
    {
        for(auto user:users)
        {
            tmp.first=user.account;
            tmp.second=kicked_user.nickname + " is kicked out of room named "+room1.name+" by ADMIN.\n";
            rtn.push_back(tmp);
        }
        return 0;
    }
    return -1;
}

int ApplicationProtocol::AppSendMessageInRoom(vector<pair<string,string>>& rtn, Account account, Id id, string msg)
{
    pair<string,string> tmp;
    vector<Data_user> users;
    unsigned int status;
    status = GetUsersInARoom(id,users);
    if(status==Database::SUCCESS)
    {
        tmp.second=msg;
        for(auto user:users)
        {
            tmp.first=user.account;
            rtn.push_back(tmp);
        }
        return 0;
    }else
    {
        tmp.first=account;
        tmp.second=ErrorHandle(status);
        return -1;
    }
}

int ApplicationProtocol::AppPrivateMessage(vector<pair<string,string>>& rtn, Account sender, Account recevier, string msg)
{
    pair<string,string> tmp;
    Data_user sender_user,recevier_user;
    unsigned int status;

    status=GetUser(sender,sender_user);
    if(status!=Database::SUCCESS)
    {
        tmp.first=sender;
        tmp.second=ErrorHandle(status);
        rtn.push_back(tmp);
        return -1;
    }

    status=GetUser(recevier,recevier_user);
    if(status!=Database::SUCCESS)
    {
        tmp.first=sender;
        tmp.second=ErrorHandle(status);
        rtn.push_back(tmp);
        return -1;
    }

    tmp.second=sender_user.nickname+" says quietly to "+recevier_user.nickname+": "+msg;
    tmp.first=sender;
    rtn.push_back(tmp);
    tmp.first=recevier;
    rtn.push_back(tmp);

    return 0;

}
