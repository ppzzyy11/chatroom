/*
 * Copyright (C)  puzeyu puzeyu@bupt.edu.cn
 *
 * 2019年 05月 07日 星期二 14:40:57 CST
 *
 * database.hpp
 *
 * To Implement:
 *      Load()
 *      Save()
 *
 * To optimize:
 *      Store index of data_users in hashtable
 *      Store data_users and data_rooms in Redis or something else
 */

#include "database.hpp"

/*
 * @para filenaem to load
 */

Database::Database(string load)
{
    //load file
    data_users.clear();
    data_rooms.clear();
    save="";

    int status = Load(load);
    if( status!=0 )
    {
        ErrorHandle(status);
        return ;
    }
    save=load;//remember the file name, save to it when destruct.
}

Database::~Database()
{
    int status = Save(save);
    if( status !=0 )
    {
        ErrorHandle(status);
    }
    data_users.clear();
    data_rooms.clear();
}


/*
 * Add user to database, call by Register
 * @parm include all information of newuser
 * return 0 for SUCCESS, others for failure
 */
int Database::AddUser(Data_user newuser)
{
    //check account length
    if(newuser.account.length()>ACCOUNT_UPPER_LENGTH)
    {
        return ACCOUNT_TOO_LONG;
    }else if(newuser.account.length()<ACCOUNT_LOWER_LENGTH)
    {
        return ACCOUNT_TOO_SHORT;
    }

    //check nickname length
    if(newuser.nickname.length()>NICKNAME_UPPER_LENGTH)
    {
        return NICKNAME_TOO_LONG;
    }else if(newuser.nickname.length()<NICKNAME_LOWER_LENGTH)
    {
        return NICKNAME_TOO_SHORT;
    }

    //check password length
    if(newuser.passwd.length()>PASSWD_UPPER_LENGTH)
    {
        return PASSWD_TOO_LONG;
    }else if(newuser.passwd.length()<PASSWD_LOWER_LENGTH)
    {
        return PASSWD_TOO_SHORT;
    }

    //check for duplication with the existed users
    for(auto user:data_users)
    {
        if(user.account == newuser.account)
        {
            return ACCOUNT_EXIST;
        }

        if(user.nickname == newuser.nickname)
        {
            return NICKNAME_EXIST;
        }
    }

    //push
    data_users.push_back(newuser);

    return SUCCESS;
}

/*
 * Get the specified user information
 *
 * @para account of the user
 * @para recipient of the user
 * return 0 for SUCCESS
 */
int Database::GetUser(Account account, Data_user& rtn)
{
    for(auto user:data_users)
    {
        if(user.account == account)
        {
            rtn=user;
            return SUCCESS;
        }
    }
    return USER_NOT_FOUND;
}

/*
 * Get a vector of all users
 * @para recipient of users
 * return 0 for SUCCESS
 */
int Database::GetAllUsers(vector<Data_user>& rtn)
{
    rtn=data_users;
    return SUCCESS;
}

/*
 * Build a new room
 *
 * @para all information of the new room
 * return 0 for SUCCESS
 */
int Database::AddRoom(Data_room newroom)
{
    //check for length
    if(newroom.passwd.length()>PASSWD_UPPER_LENGTH)
    {
        return PASSWD_TOO_LONG;
    }else if(newroom.passwd.length()<PASSWD_LOWER_LENGTH)
    {
        return PASSWD_TOO_SHORT;
    }

    if(newroom.name.length()>ROOM_NAME_UPPER_LENGTH)
    {
        return ROOM_NAME_TOO_LONG;
    }else if(newroom.name.length()<ROOM_NAME_LOWER_LENGTH)
    {
        return ROOM_NAME_TOO_SHORT;
    }

    //check for duplication
    for(auto room:data_rooms)
    {
        if(room.id == newroom.id)
        {
            return ROOM_ID_EXIST;
        }
        if(room.name== newroom.name)
        {
            return ROOM_NAME_EXIST;
        }
    }

    data_rooms.push_back(newroom);

    return SUCCESS;
}

/*
 * Get information of a specified room
 *
 * @para id of room
 * @para recipient of room
 * return 0 for SUCCESS
 */
int Database::GetRoom(Id id, Data_room& rtn)
{
    for(auto room:data_rooms)
    {
        if(room.id == id)
        {
            rtn=room;
            return SUCCESS;
        }
    }
    return ROOM_NOT_FOUND;
}

/*
 * Get a vector of all rooms
 *
 * @para recipient
 * return 0 for SUCCESS
 */
int Database::GetAllRooms(vector<Data_room>& rtn)
{
    rtn=data_rooms;
    return SUCCESS;
}

/*
 * one user try to Change the administor of a room
 *
 * @para Account of requestor
 * @para room id
 * @para new administor account
 * return 0 for SUCCESS
 */
int Database::UserChangeRoomAdmin(Account oldadmin, Id roomid, Account newadmin)
{
    Data_user user;
    //check for users' existing
    if(SUCCESS!=GetUser(oldadmin,user))
    {
        return USER_NOT_FOUND;
    }
    if(SUCCESS!=GetUser(newadmin,user))
    {
        return USER_NOT_FOUND;
    }

    //find the room
    for(auto& room:data_rooms)
    {
        if(room.id==roomid)
        {
            //only administor can operate
            if(room.admin==oldadmin)
            {
                room.admin=newadmin;
                return SUCCESS;
            }else
            {
                return USER_NOT_ADMIN;
            }
        }
    }
    return ROOM_NOT_FOUND;
}

/*
 * user join a room
 *
 * @para account of user
 * @para id of room
 * return 0 for SUCCESS
 */
int Database::UserJoinRoom(Account account, Id id)
{
    Data_user user;
    if(SUCCESS==GetUser(account,user))
    {
        for(auto& room:data_rooms)
        {
            if(room.id==id)
            {
                for(auto black:room.blacklist)
                {
                    if(black==account)
                    {
                        return  BLACKLISTS;
                    }
                }
                room.members.push_back(account);
                return SUCCESS;
            }
        }
        return ROOM_NOT_FOUND;
    }
    return USER_NOT_FOUND;
}

/*
 * get a vector of users in a room
 *
 * @para id of a room
 * @para recipient
 * return 0 for SUCCESS
 */
int Database::GetUsersInARoom(Id id,vector<Data_user>& rtn)
{
    for(auto room:data_rooms)
    {
        if(room.id==id)
        {
            rtn.clear();
            Data_user user;
            for(auto account:room.members)
            {

                GetUser(account,user);
                rtn.push_back(user);
            }
            return SUCCESS;
        }
    }
    return ROOM_NOT_FOUND;
}

/*
 * one user leave one room
 *
 * @para account of a user
 * @para id of a room
 * return 0 for SUCCESS
 */
int Database::OneUserLeaveRoom(Account account, Id id)
{
    for(auto& room:data_rooms)
    {
        if(room.id==id)
        {
            if(room.admin==account)
            {
                return ADMIN_NOT_ALLOWED_LEAVE;
            }
            for(auto& member:room.members)
            {
                if(member==account)
                {
                    swap(member, room.members.back());
                    room.members.pop_back();
                    return SUCCESS;
                }
            }
            return USER_NOT_FOUND;
        }
    }
    return ROOM_NOT_FOUND;
}

/*
 * ERROR Handling
 *
 * @para ERROR Number
 * return an empty string
 */
string Database::ErrorHandle(unsigned int erron)
{
    switch(erron)
    {
        case SUCCESS:
            Log("SUCCESS",2);
            break;

        case ACCOUNT_EXIST:
            Log("ACCOUNT_EXIST.",2);
            break;

        case ACCOUNT_TOO_LONG:
            Log("ACCOUNT_TOO_LONG",2);
            break;

        case ACCOUNT_TOO_SHORT:
            Log("ACCOUNT_TOO_SHORT",2);
            break;

        case PASSWD_TOO_LONG:
            Log("PASSWD_TOO_LONG",2);
            break;

        case PASSWD_TOO_SHORT:
            Log("PASSWD_TOO_SHORT",2);
            break;

        case NICKNAME_EXIST:
            Log("NICKNAME_EXIST",2);
            break;

        case NICKNAME_TOO_SHORT:
            Log("NICKNAME_EXIST",2);
            break;

        case NICKNAME_TOO_LONG:
            Log("NICKNAME_TOO_LONG",2);
            break;

        case NOT_ADMIN:
            Log("NOT_ADMIN",2);
            break;

        case USER_NOT_FOUND:
            Log("USER_NOT_FOUND",2);
            break;

        case ROOM_NOT_FOUND:
            Log("ROOM_NOT_FOUND",2);
            break;

        case BLACKLISTS:
            Log("BLACKLISTS",2);
            break;

        case ADMIN_NOT_ALLOWED_LEAVE:
            Log("ADMIN_NOT_ALLOWED_LEAVE",2);
            break;

        case SAVE_NOT_FOUND:
            Log("SAVE_NOT_FOUND",2);
            break;

        case FORMAT_ERROR:
            Log("FORMAT_ERROR",2);
            break;

        case ROOM_ID_EXIST:
            Log("ROOM_ID_EXIST",2);
            break;

        case ROOM_ID_TOO_SHORT:
            Log("ROOM_ID_TOO_SHORT",2);
            break;

        case ROOM_ID_TOO_LONG:
            Log("ROOM_ID_TOO_LONG",2);
            break;

        case ROOM_NAME_EXIST:
            Log("ROOM_NAME_EXIST",2);
            break;

        case ROOM_NAME_TOO_SHORT:
            Log("ROOM_NAME_TOO_SHORT",2);
            break;

        case ROOM_NAME_TOO_LONG:
            Log("ROOM_NAME_TOO_LONG",2);
            break;
    };
    return "";
}

/*
 * Load the database from a file
 *
 * @para file name
 * return 0 for SUCCESS
 */
int Database::Load(string str)
{
    return SUCCESS;
}

/*
 * Save Database to a file
 *
 * @para file name
 * return 0 for SUCCESS
 */
int Database::Save(string str)
{
    return SUCCESS;
}

/*
 * Log system
 *
 * @para Log content
 * @para 1 for stdout, 2 for stderr
 * return 0 for SUCCESS
 */
int Database::Log(string str, int i=1)
{
    FILE* fp;
    switch(i)
    {
        case 1:
            fp=stdout;
            break;

        case 2:
            fp=stderr;
            break;
    }
    fprintf(fp,"%s",str.c_str());
    return 0;
}
