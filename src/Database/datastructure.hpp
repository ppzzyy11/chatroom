#ifndef _DATASTRUCTURE_HPP_
#define _DATASTRUCTURE_HPP_

#include <vector>
#include <string>

//define data_user
typedef Account std::string;
typedef Passwd std::string;
typedef Nickname std::string;
typedef struct data_user{
    Account account;//identifier
    Passwd passwd;
    Nickname nickname;
}Data_user;


typedef Id std::string;
typedef Name std::string;
typedef Description std::string;
typedef Members std::vector<Account>;
typedef Blacklist std::vector<Account>;
typedef struct data_room{
    Id id;//identifier
    Name name;
    Description description;
    Passwd passwd;
    Account admin;
    Members members;
    Blacklists blacklists;
}Data_room;


#endif //!_DATASTRUCTURE_HPP_
