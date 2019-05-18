/*
 * Copyright (C)  puzeyu puzeyu@bupt.edu.cn
 *
 * 2019年 05月 07日 星期二 14:40:57 CST
 *
 * database.hpp
 *
 */

#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "datastructure.hpp"

//AddUser Error

using std::string;
using std::vector;
class Database{
    private:
        vector<Data_user> data_users;
        vector<Data_room> data_rooms;



        int Log(string,int);

        string save;

    protected:
    public:
        Database();
        Database(string);
        ~Database();

        int Save(string);
        int Load(string);

        int AddUser(Data_user);//return 0 on success, others on different error
        int GetUser(Account, Data_user&);
        int GetAllUsers(vector<Data_user>&);
        int UserQuit(Account);

        int AddRoom(Data_room);
        int GetRoom(Id, Data_room&);
        int GetAllRooms(vector<Data_room>&);
        int CloseRoom(Account,Id);

        int UserChangeRoomAdmin(Account,Id,Account);//user, room, new admin


        int UserJoinRoom(Account,Id,Passwd);
        int GetUsersInARoom(Id,vector<Data_user>&);
        int OneUserLeaveRoom(Account,Id);

        //implement below two
        int KickUserOut(Account,Id,Account);

        string ErrorHandle(unsigned int);

        //AddUser Error
        static const unsigned int SUCCESS;
        static const unsigned int ACCOUNT_EXIST ;
        static const unsigned int ACCOUNT_TOO_LONG ;
        static const unsigned int ACCOUNT_TOO_SHORT;
        static const unsigned int PASSWD_TOO_LONG ;
        static const unsigned int PASSWD_TOO_SHORT;
        static const unsigned int NICKNAME_EXIST;
        static const unsigned int NICKNAME_TOO_LONG;
        static const unsigned int NICKNAME_TOO_SHORT;

        //User Change Room Admin
        static const unsigned int NOT_ADMIN;
        static const unsigned int USER_NOT_FOUND;
        static const unsigned int ROOM_NOT_FOUND;

        //User Join Room Error
        static const unsigned int BLACKLISTS;

        //User Leave unsigned Room
        static const unsigned int ADMIN_NOT_ALLOWED_LEAVE;

        //Save Error
        static const unsigned int SAVE_NOT_FOUND ;
        static const unsigned int FORMAT_ERROR ;

        //Add room error
        static const unsigned int ROOM_ID_EXIST;
        static const unsigned int ROOM_ID_TOO_SHORT;
        static const unsigned int ROOM_ID_TOO_LONG;

        static const unsigned int ROOM_NAME_EXIST;
        static const unsigned int ROOM_NAME_TOO_SHORT;
        static const unsigned int ROOM_NAME_TOO_LONG;

        static const unsigned int USER_NOT_ADMIN;
        static const unsigned int ADMIN_CANNOT_BE_MEMBER;

        static const unsigned int PASSWD_NOT_RIGHT;



        static const size_t ACCOUNT_LOWER_LENGTH;
        static const size_t ACCOUNT_UPPER_LENGTH;

        static const size_t NICKNAME_LOWER_LENGTH;
        static const size_t NICKNAME_UPPER_LENGTH;

        static const size_t PASSWD_LOWER_LENGTH;
        static const size_t PASSWD_UPPER_LENGTH;

        static const size_t ROOM_NAME_LOWER_LENGTH;
        static const size_t ROOM_NAME_UPPER_LENGTH;


};

#endif
