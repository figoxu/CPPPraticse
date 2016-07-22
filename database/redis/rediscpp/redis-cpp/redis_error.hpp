#ifndef REDIS_ERROR_HPP_201505162105
#define REDIS_ERROR_HPP_201505162105

/* {{{
 * =============================================================================
 *      Filename    :   redis_error.hpp
 *      Description :
 *      Created     :   2015-05-16 21:05:01
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <string>

namespace redis
{

class redis_error
{
public:
    redis_error(const std::string &type, const std::string &err);

    operator std::string ();
    operator const std::string () const;
private:
    std::string err_;
};

// Some socket-level I/O or general connection error.

class connection_error : public redis_error
{
public:
    connection_error(const std::string &err);
};

// Redis gave us a reply we were not expecting.
// Possibly an internal error (here or in redis, probably here).

class protocol_error : public redis_error
{
public:
    protocol_error(const std::string &err);
};

// A key that you expected to exist does not in fact exist.

class key_error : public redis_error
{
public:
    key_error(const std::string &err);
};

// A value of an expected type or other semantics was found to be invalid.

class value_error : public redis_error
{
public:
    value_error(const std::string &err);
};

}

#endif  /* REDIS_ERROR_HPP_201505162105 */

