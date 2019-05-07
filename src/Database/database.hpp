/*
 * Copyright (C)  puzeyu puzeyu@bupt.edu.cn
 *
 * 2019年 05月 07日 星期二 14:40:57 CST
 *
 * database.hpp
 *
 *
 *
 *
 *
 *
 *
 *
 */

#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <vector>
#include <string>

#include "datastructure.hpp"


using std::string;
using std::vector;
class Database{
    public:
        Database(string);
        ~Database();

        int AddUser(Data_user);//return 0 on success, others on different error
        int GetUser(Account, Data_user&);
        vector<Data_user> GetAllUsers();

        int AddRoom(Data_room);
        int GetRoom(ID, Data_room&);
        vector<Data_room> GetAllRooms();

        int UserChangeRoomAdmin(Account,Id,Account);//user, room, new admin
        int UserJoinRoom(Account,Id);

        vector<Data_user> GetUsersInARoom(Id);
        int OneUserLeaveRoom(Account,Id);

        //int KickUserOut(Account,Id,Account);

        string ErrorHandle(int);

    private:
        vector<Data_user> data_users;
        vector<Data_room> data_rooms;


        int Save(string);
        int Load(string);

        int Log(string,int);

        string save;

    protected:

        //AddUser Error
        static const int SUCCESS =                      0;
        static const int ACCOUNT_EXIST =               1<<0;
        static const int ACCOUNT_TOO_LONG =            1<<1;
        static const int ACCOUNT_TOO_SHORT =           1<<2;
        static const int PASSWD_TOO_LONG =             1<<3;
        static const int PASSWD_TOO_SHORT =            1<<4;
        static const int NICKNAME_EXIST =              1<<5;
        static const int NICKNAME_TOO_LONG =           1<<6;
        static const int NICKNAME_TOO_SHORT =          1<<7;

        //User Change Room Admin
        static const int NOT_ADMIN =                   1<<8;
        static const int USER_NOT_FOUND =              1<<9;
        static const int ROOM_NOT_FOUND =              1<<10;

        //User Join Room Error
        static const int BLACKLISTS =                  1<<11;

        //User Leave Room
        static const int ADMIN_NOT_ALLOWED_LEAVE =     1<<12;

        //Save Error
        static const int SAVE_NOT_FOUND =              1<<13;
        static const int FORMAT_ERROR =                1<<14;

        //Add room error
        static const int ROOM_ID_EXIST =               1<<15;
        static const int ROOM_ID_TOO_SHORT =           1<<16;
        static const int ROOM_ID_TOO_LONG =            1<<17;

        static const int ROOM_NAME_EXIST =             1<<18;
        static const int ROOM_NAME_TOO_SHORT =         1<<19;
        static const int ROOM_NAME_TOO_LONG =          1<<20;

        static const int USER_NOT_ADMIN =              1<<21;



        static const size_t ACCOUNT_LOWER_LENGTH =    8;
        static const size_t ACCOUNT_UPPER_LENGTH =   20;

        static const size_t NICKNAME_LOWER_LENGTH =   3;
        static const size_t NICKNAME_UPPER_LENGTH =   10;

        static const size_t PASSWD_LOWER_LENGTH =   5;
        static const size_t PASSWD_UPPER_LENGTH =   20;

        static const size_t ROOM_NAME_LOWER_LENGTH =   5;
        static const size_t ROOM_NAME_UPPER_LENGTH =   20;

};


#endif _DATABASE_HPP_
