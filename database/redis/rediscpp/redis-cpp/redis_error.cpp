/* {{{
 * =============================================================================
 *      Filename    :   redis_error.cpp
 *      Description :
 *      Created     :   2015-05-16 21:05:20
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <iostream>
#include "redis_error.hpp"

using namespace std;

namespace redis
{

redis_error::redis_error(const string &type, const string &err)
    :err_(err)
{
    cerr << "[ERR][REDIS][" << type << "]" << err << endl;
}

redis_error::operator std::string ()
{
    return err_;
}

redis_error::operator const std::string () const
{
    return err_;
}

connection_error::connection_error(const string &err)
    :redis_error("connection", err)
{
}

protocol_error::protocol_error(const string &err)
    :redis_error("protocol", err)
{
}

key_error::key_error(const string &err)
    :redis_error("key", err)
{
}

value_error::value_error(const string &err)
    :redis_error("value", err)
{
}

}
