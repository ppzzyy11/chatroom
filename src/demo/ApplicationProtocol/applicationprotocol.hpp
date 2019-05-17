/*
 * applicationprotocol.hpp
 * Copyright (C) 2019 pzy <pzy@pzy-ThinkPad-Edge-E440>
 *
 * Distributed under terms of the MIT license.
 *
 * Description:
 * convert steam bytes into customed message
 */

#ifndef APPLICATIONPROTOCOL_HPP
#define APPLICATIONPROTOCOL_HPP

#include <vector>
#include <utility>

#include "database.hpp"
#include "datastructure.hpp"

using std::vector;
using std::string;
using std::pair;
class ApplicationProtocol:public Database
{
    public:
        ApplicationProtocol(string);
        ApplicationProtocol();
        ~ApplicationProtocol();

        //execute the command, return messages in format than send .second(string) to .first(account).
        std::vector<std::pair<std::string,std::string>> Exec(string command);
    private:
        string GetStr(const string&,size_t&);
        int AppRegister(std::vector<std::pair<std::string,std::string>>&,\
                Data_user user);
        int Login(std::vector<std::pair<std::string,std::string>>&,\
                Data_user user);
        int AppGetRoom(vector<pair<string,string>>&,Data_user,Data_room);
        int AppGetRooms(vector<pair<string,string>>&,Data_user);
        int AppBuildRoom(vector<pair<string,string>>&,Data_user,Data_room);


        static const unsigned char REGISTER;
        static const unsigned char LOGIN;
        static const unsigned char QUIT;
        static const unsigned char GET_ROOM;
        static const unsigned char GET_ROOMS;
        static const unsigned char BUILD_ROOM;
        static const unsigned char CLOSE_ROOM;
        static const unsigned char CHANGE_ROOM_ADMIN;
        static const unsigned char JOIN_ROOM;
        static const unsigned char QUIT_ROOM;
        static const unsigned char SEND_MESSAGE_IN_ROOM;
        static const unsigned char GET_USERS_IN_ROOM;
        static const unsigned char PRIVATE_MESSAGE;
        static const unsigned char KICK_USER;
        static const unsigned char SUCCESS;
        static const unsigned char FAILURE;
    protected:


};


#endif /* !APPLICATIONPROTOCOL_HPP */
