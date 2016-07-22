/* {{{
 * =============================================================================
 *      Filename    :   redis_pool.cpp
 *      Description :
 *      Created     :   2015-05-16 23:05:29
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <iostream>
#include "redis_pool.hpp"

using namespace redis;
using namespace std;

connection_pool::connection_pool(const std::string &host, int port, int max_connections)
    :_host(host), _port(port),
     _max_connctions(max_connections), _max_idle(max_connections)
{

}

connection_pool::~connection_pool(void)
{

}

connection* connection_pool::new_connection(void)
{
    return NULL;
}

void connection_pool::disconnect(connection *conn)
{
    delete conn;
}

connection* connection_pool::acquire(void)
{
    connection *conn = NULL;

    if(_connections_idle.size() > 0) {
        conn = _connections_idle.back();
        _connections_idle.pop_back();
    } else {
        conn = new connection(_host, _port);
    }

    _connections_in_use.insert(conn);

    cout << "Acquire conn:" << conn->id() << endl;

    return conn;
}

connection* connection_pool::try_acquire(int timeout)
{
    return NULL;
}

void connection_pool::release(connection *conn)
{
    conns_in_use_type::iterator iter = _connections_in_use.find(conn);

    _connections_in_use.erase(iter);
    _connections_idle.push_back(conn);
}

void connection_pool::_lock(void)
{

}

void connection_pool::_unlock(void)
{

}

void connection_pool::set_max_active(int num)
{
    _lock();
    _max_connctions = num;
    _unlock();
}

void connection_pool::set_max_idle(int num)
{
    _lock();
    _max_idle = num;
    _unlock();
}

int  connection_pool::get_max_connections(void) const
{
    return _max_connctions;
}

int  connection_pool::get_max_idle(void) const
{
    return _max_idle;
}

int  connection_pool::get_connections_count(void) const
{
    return get_idle_count() + get_in_use_count();
}

int  connection_pool::get_idle_count(void) const
{
    return _connections_idle.size();
}

int  connection_pool::get_in_use_count(void) const
{
    return _connections_in_use.size();
}
