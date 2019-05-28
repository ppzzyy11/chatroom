/*
 * Buffer.hpp
 * Copyright (C) 2019 puzeyu <puzeyu@bupt.edu.cn>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <string>

#include "datastructure.hpp"

using std::string;
using std::vector;

class Buffer{
    public:
        Buffer();
        ~Buffer();
        vector<string> Pour(string);
    private:
        string pool;
    protected:

};



#endif /* !BUFFER_HPP */
