/*
 * Copyright (C)  puzeyu puzeyu@bupt.edu.cn
 *
 * 2019年 05月 07日 星期二 14:40:57 CST
 *
 * database.hpp
 *
 * To Implement:
 *
 * To optimize:
 *      Store index of data_users in hashtable
 *      Store data_users and data_rooms in Redis or something else
 *      Didn't deal with the BIG ENDIAN and LiTTEL ENDIAN with the Load and Save
 */

#include "database.hpp"

const unsigned int Database::SUCCESS =                      0;
const unsigned int Database::ACCOUNT_EXIST =               1<<0;
const unsigned int Database::ACCOUNT_TOO_LONG =            1<<1;
const unsigned int Database::ACCOUNT_TOO_SHORT =           1<<2;
const unsigned int Database::PASSWD_TOO_LONG =             1<<3;
const unsigned int Database::PASSWD_TOO_SHORT =            1<<4;
const unsigned int Database::NICKNAME_EXIST =              1<<5;
const unsigned int Database::NICKNAME_TOO_LONG =           1<<6;
const unsigned int Database::NICKNAME_TOO_SHORT =          1<<7;

//User Change Room Admin
const unsigned int Database::NOT_ADMIN =                   1<<8;
const unsigned int Database::USER_NOT_FOUND =              1<<9;
const unsigned int Database::ROOM_NOT_FOUND =              1<<10;

//User Join Room Error
const unsigned int Database::BLACKLISTS =                  1<<11;

//User Leave unsigned RoomDatabase::
const unsigned int Database::ADMIN_NOT_ALLOWED_LEAVE =     1<<12;

//Save Error
const unsigned int Database::SAVE_NOT_FOUND =              1<<13;
const unsigned int Database::FORMAT_ERROR =                1<<14;

//Add room error
const unsigned int Database::ROOM_ID_EXIST =               1<<15;
const unsigned int Database::ROOM_ID_TOO_SHORT =           1<<16;
const unsigned int Database::ROOM_ID_TOO_LONG =            1<<17;

const unsigned int Database::ROOM_NAME_EXIST =             1<<18;
const unsigned int Database::ROOM_NAME_TOO_SHORT =         1<<19;
const unsigned int Database::ROOM_NAME_TOO_LONG =          1<<20;

const unsigned int Database::USER_NOT_ADMIN =              1<<21;
const unsigned int Database::ADMIN_CANNOT_BE_MEMBER =      1<<22;



const size_t Database::ACCOUNT_LOWER_LENGTH =    3;
const size_t Database::ACCOUNT_UPPER_LENGTH =   20;

const size_t Database::NICKNAME_LOWER_LENGTH =   1;
const size_t Database::NICKNAME_UPPER_LENGTH =   10;

const size_t Database::PASSWD_LOWER_LENGTH =   4;
const size_t Database::PASSWD_UPPER_LENGTH =   20;

const size_t Database::ROOM_NAME_LOWER_LENGTH =   2;
const size_t Database::ROOM_NAME_UPPER_LENGTH =   20;

/*
 * constructor without para, use databas.sav as load file name
 */
Database::Database()
{

    data_users.clear();
    data_rooms.clear();
    save="database.sav";
}

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
 * User quit, remove the rooms that User as administor, remove user from room's members if User is a member
 *
 * @para account of a user
 * return 0 for SUCCESS
 */
int Database::UserQuit(Account account)
{
    vector<Data_room> rooms_left;
    vector<Data_room> room_to_close;

    for(auto& room:data_rooms)
    {
        if(room.admin==account)
        {
            room_to_close.push_back(room);
        }else
        {
            for(auto& member:room.members)
            {
                if(member==account)
                {
                    member=room.members.back();
                    room.members.pop_back();
                    break;
                }
            }
            rooms_left.push_back(room);

        }
    }

    data_rooms=rooms_left;

    for(auto& user:data_users)
    {
        if(user.account==account)
        {
            user=data_users.back();
            data_users.pop_back();
            return SUCCESS;
        }
    }

    return USER_NOT_ADMIN;
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

    Data_user user;
    if(USER_NOT_FOUND==GetUser(newroom.admin,user))
    {
        return USER_NOT_FOUND;
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
 * Cloase a Room:remove room from data_rooms
 *
 * @para account
 * @para id
 * return 0 for SUCCESS
 */
int Database::CloseRoom(Account account,Id id)
{
    for(auto& room:data_rooms)
    {
        if(room.id==id)
        {
            if(room.admin==account)
            {
                room=data_rooms.back();
                data_rooms.pop_back();
                return SUCCESS;
            }
            return USER_NOT_ADMIN;
        }
    }
    return ROOM_NOT_FOUND;
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
            for(auto member:room.members)
            {
                if(member==account)
                {
                    return ACCOUNT_EXIST;
                }
            }
            if(room.id==id)
            {
                for(auto black:room.blacklist)
                {
                    if(black==account)
                    {
                        return  BLACKLISTS;
                    }
                }
                if(account==room.admin)
                {
                    return ADMIN_CANNOT_BE_MEMBER;
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
 * get a vector of users including administor in a room
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
            GetUser(room.admin,user);
            rtn.push_back(user);
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
 * one user Kick one user out a room
 * @para account of user kick
 * @para id of room
 * @para account of user kicked
 * return 0 for SUCCESS
 */
int Database::KickUserOut(Account admin,Id id,Account acc)
{
    for(auto& room:data_rooms)
    {
        if(room.id==id)
        {
            if(room.admin!=admin)
            {
                return USER_NOT_ADMIN;
            }else
            {
                for(auto& member:room.members)
                {
                    if(member==acc)
                    {
                        member=room.members.back();
                        room.members.pop_back();
                        room.blacklist.push_back(acc);

                        return SUCCESS;
                    }
                }
                return USER_NOT_FOUND;
            }
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
    string rtn=std::to_string(erron);
    switch(erron)
    {
        case SUCCESS:
            Log(rtn+="SUCCESS",2);
            break;

        case ACCOUNT_EXIST:
            Log(rtn+="ACCOUNT_EXIST.",2);
            break;

        case ACCOUNT_TOO_LONG:
            Log(rtn+="ACCOUNT_TOO_LONG",2);
            break;

        case ACCOUNT_TOO_SHORT:
            Log(rtn+="ACCOUNT_TOO_SHORT",2);
            break;

        case PASSWD_TOO_LONG:
            Log(rtn+="PASSWD_TOO_LONG",2);
            break;

        case PASSWD_TOO_SHORT:
            Log(rtn+="PASSWD_TOO_SHORT",2);
            break;

        case NICKNAME_EXIST:
            Log(rtn+="NICKNAME_EXIST",2);
            break;

        case NICKNAME_TOO_SHORT:
            Log(rtn+="NICKNAME_EXIST",2);
            break;

        case NICKNAME_TOO_LONG:
            Log(rtn+="NICKNAME_TOO_LONG",2);
            break;

        case NOT_ADMIN:
            Log(rtn+="NOT_ADMIN",2);
            break;

        case USER_NOT_FOUND:
            Log(rtn+="USER_NOT_FOUND",2);
            break;

        case ROOM_NOT_FOUND:
            Log(rtn+="ROOM_NOT_FOUND",2);
            break;

        case BLACKLISTS:
            Log(rtn+="BLACKLISTS",2);
            break;

        case ADMIN_NOT_ALLOWED_LEAVE:
            Log(rtn+="ADMIN_NOT_ALLOWED_LEAVE",2);
            break;

        case SAVE_NOT_FOUND:
            Log(rtn+="SAVE_NOT_FOUND",2);
            break;

        case FORMAT_ERROR:
            Log(rtn+="FORMAT_ERROR",2);
            break;

        case ROOM_ID_EXIST:
            Log(rtn+="ROOM_ID_EXIST",2);
            break;

        case ROOM_ID_TOO_SHORT:
            Log(rtn+="ROOM_ID_TOO_SHORT",2);
            break;

        case ROOM_ID_TOO_LONG:
            Log(rtn+="ROOM_ID_TOO_LONG",2);
            break;

        case ROOM_NAME_EXIST:
            Log(rtn+="ROOM_NAME_EXIST",2);
            break;

        case ROOM_NAME_TOO_SHORT:
            Log(rtn+="ROOM_NAME_TOO_SHORT",2);
            break;

        case ROOM_NAME_TOO_LONG:
            Log(rtn+="ROOM_NAME_TOO_LONG",2);
            break;
    };
    return rtn;
}

/*
 * Load the database from a file
 *
 * @para file name
 * return 0 for SUCCESS
 */
int Database::Load(string str)
{

    data_users.clear();
    data_rooms.clear();
    std::ifstream inf(str.c_str(),std::ios::binary);
    if(!inf.is_open())
    {
        return SAVE_NOT_FOUND;
    }
    size_t size;
    inf>>size;
    for(size_t i=0;i<size;i++)
    {
        Data_user user;
        inf>>user.account;
        inf>>user.passwd;
        inf>>user.nickname;
        data_users.push_back(user);
    }

    inf>>size;
    for(size_t i=0;i<size;i++)
    {
        Data_room room;
        inf>>room.id;
        inf>>room.name;
        inf>>room.description;
        inf>>room.passwd;
        inf>>room.admin;

        size_t size2;

        room.members.clear();
        inf>>size2;
        {
            Account account;
            inf>>account;
            room.members.push_back(account);
        }

        room.blacklist.clear();
        inf>>size2;
        for(size_t j=0;j<size2;j++)
        {
            Account account;
            inf>>account;
            room.blacklist.push_back(account);
        }
        data_rooms.push_back(room);
    }
    inf.close();
    return SUCCESS;
}

/*
 * Save Database to a file
 * Write Number of users first
 * Then users
 * Write Number of rooms second
 * Then rooms
 *
 * @para file name
 * return 0 for SUCCESS
 */
int Database::Save(string str)
{
    std::ofstream of(str.c_str(),std::ios::binary);
    if(!of.is_open())
    {
        return SAVE_NOT_FOUND;
    }
    of<<data_users.size()<<'\n';
    for(const auto& user:data_users)
    {
        of<<user.account<<'\n';
        of<<user.passwd<<'\n';
        of<<user.nickname<<'\n';
    }
    of<<data_rooms.size()<<'\n';
    for(const auto& room:data_rooms)
    {
        of<<room.id<<'\n';
        of<<room.name<<'\n';
        of<<room.description<<'\n';
        of<<room.passwd<<'\n';
        of<<room.admin<<'\n';
        of<<room.members.size()<<'\n';
        for(const auto& account:room.members)
        {
            of<<account<<'\n';
        }

        of<<room.blacklist.size()<<'\n';
        for(const auto& account:room.blacklist)
        {
            of<<account<<'\n';
        }
    }

    of.close();
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
