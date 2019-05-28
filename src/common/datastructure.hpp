#ifndef _DATASTRUCTURE_HPP_
#define _DATASTRUCTURE_HPP_

#include <vector>
#include <string>

//define data_user
typedef std::string Account;
typedef std::string Passwd;
typedef std::string Nickname;
typedef struct data_user{
    Account account;//identifier
    Passwd passwd;
    Nickname nickname;
}Data_user;


typedef std::string Id;
typedef std::string Name;
typedef std::string Description;
typedef std::vector<Account> Members;
typedef std::vector<Account> Blacklist;
typedef struct data_room{
    Id id;//identifier
    Name name;
    Description description;
    Passwd passwd;
    Account admin;
    Members members;
    Blacklist blacklist;
}Data_room;

static unsigned int Log_Num=0;
int Log(std::string,int);

#endif //!_DATASTRUCTURE_HPP_
