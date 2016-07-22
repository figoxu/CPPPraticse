#ifndef REDIS_POOL_HPP_201505162205
#define REDIS_POOL_HPP_201505162205

/* {{{
 * =============================================================================
 *      Filename    :   redis_pool.hpp
 *      Description :
 *      Created     :   2015-05-16 22:05:26
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <set>
#include <vector>
#include "redis_connection.hpp"

namespace redis
{

class connection_pool
{
public:
    typedef std::vector<connection*> conns_idle_type;
    typedef std::set<connection*>    conns_in_use_type;
    connection_pool(const std::string &host = "localhost",
                    int port = 6379, int max_connections = 32);
    ~connection_pool(void);

    connection* acquire(void);
    connection* try_acquire(int timeout = 0);
    void release(connection *conn);

    void set_max_active(int num);
    void set_max_idle(int num);
    int  get_max_connections(void) const;
    int  get_max_idle(void) const;
    int  get_connections_count(void) const;
    int  get_idle_count(void) const;
    int  get_in_use_count(void) const;
private:
    connection_pool(const connection_pool &);
    connection_pool& operator=(const connection_pool &);

    connection* new_connection(void);
    void disconnect(connection *conn);

    void _lock(void);
    void _unlock(void);
    bool _wait(int timeout);
    void _raise(void);
private:
    std::string _host;
    int _port;
    int _max_connctions;
    int _max_idle;

    conns_idle_type     _connections_idle;
    conns_in_use_type   _connections_in_use;
};

}

#endif  /* REDIS_POOL_HPP_201505162205 */

