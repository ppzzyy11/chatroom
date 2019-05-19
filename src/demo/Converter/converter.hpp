/*
 * converter.hpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 *
 *
 * To optimize:
 *  - store the data in database
 *  - save load function
 *
 */

#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include "applicationprotocol.hpp"
#include "buffer.hpp"

using std::pair;
using std::vector;
using std::string;
using std::unordered_map;

class Converter:public ApplicationProtocol
{
    public:
        Converter();
        ~Converter();

        vector<pair<void*,string>> ConRead(void*,vector<char>);
        int ConAccept(void*);
        int ConError(void*);
    private:
        unordered_map<void*,Account> bev2acc;//for every bev*
        unordered_map<Account,void*> acc2bev;//map every online account to an authenticitd bev*
        unordered_map<void*,Buffer> bev2buf;//a Buffer for each bev*, converter the TCP stream into App message.
        unordered_map<Account,vector<string>> off_msgs;//for every offline account


    protected:

};

#endif /* !CONVERTER_HPP */
