#ifndef REDIS_CPP_HPP_201505161505
#define REDIS_CPP_HPP_201505161505

/* {{{
 * =============================================================================
 *      Filename    :   redis_cpp.hpp
 *      Description :
 *      Created     :   2015-05-16 15:05:32
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unistd.h>

#include "redis_error.hpp"
#include "redis_zset_opt.hpp"

#define REDIS_CPP_VERSION_MAJ 0
#define REDIS_CPP_VERSION_MIN 0

namespace redis
{

using namespace std;

struct key_value
{
    string key;
    string val;
};

struct key_score
{
    string key;
    double score;
};

typedef vector<string>      string_vector;
typedef vector<key_score>   key_score_vector;
typedef set<string>         string_set;
typedef map<string, string> string_map;

class connection
{
public:
    explicit connection(void);
    explicit connection(const string &host);
    explicit connection(const string &host, int port);
    ~connection(void);

    bool opened(void) const;
    int  id(void) const { return _socket; }

    /*
     * String
     */
    void set(const string &key, const string &value);
    bool setnx(const string &key, const string &value);
    void setex(const string &key, int expire_sec, const string &value);
    long setrange(const string &key, int pos, const string &value);
    void mset(const string_vector &keys, const string_vector &values);
    bool msetnx(const string_vector &keys, const string_vector &values);
    string get(const string &key);
    string getset(const string &key, const string &value);
    string getrange(const string &key, int pos_start, int pos_end = -1);
    void mget(const string_vector &keys, string_vector &out);
    long incr(const string &key);
    long incrby(const string &key, long by);
    long decr(const string &key);
    long decrby(const string &key, long by);
    long append(const string &key, const string &value);
    long strlen(const string &key);

    /*
     * Hash
     */
    bool hset(const string &key, const string &field, const string &value);
    bool hsetnx(const string &key, const string &field, const string &value);
    void hmset(const string &key, const string_vector &fields, const string_vector &values);
    string hget(const string &key, const string &field);
    void hmget(const string &key, const string_vector &fields, string_vector &values);
    long hincrby(const string &key, const string &field, long by);
    bool hexists(const string &key, const string &field);
    long hlen(const string &key);
    void hdel(const string &key, const string &field);
    void hkeys(const string &key, string_vector &fields);
    void hvals(const string &key, string_vector &values);
    void hgetall(const string &key, string_vector &fields, string_vector &values);

    /*
     * List
     */
    /**
     * @Brief
     *
     * @Param key
     * @Param value
     *
     * @Returns the elements count of list 'key'
     */
    long rpush(const string &key, const string &value);
    long lpush(const string &key, const string &value);
    long rpushx(const string &key, const string &value);
    long lpushx(const string &key, const string &value);
    long llen(const string &key);
    long lrange(const string &key, long start, long end, string_vector &out);
    long lrange(const string &key, long start, string_vector &out) {
        return lrange(key, start, -1, out);
    }
    long lrange(const string &key, string_vector &out){
        return lrange(key, 0, -1, out);
    }
    void ltrim(const string &key, long start, long end);
    string lindex(const string &key, long index);
    void lset(const string &key, long index, const string &);
    long lrem(const string &key, long count, const string &value);
    string lpop(const string &key);
    string blpop(const string &key, long timeout);
    key_value blpop(const string_vector &keys, long timeout);
    string rpop(const string &key);
    string brpop(const string &key, long timeout);
    key_value brpop(const string_vector &keys, long timeout);
    string rpoplpush(const string &key_src, const string &key_dst);
    string brpoplpush(const string &key_src, const string &key_dst, long timeout);

    /*
     * Set
     */
    void sadd(const string &key, const string &member);
    void srem(const string &key, const string &member);
    string spop(const string &key);
    long sdiff(const string_vector &keys, string_set &out);
    long sdiffstore(const string &key_dst, const string_vector &keys);
    long sinter(const string_vector &keys, string_set &out);
    long sinterstore(const string &key_dst, const string_vector &keys);
    long sunion(const string_vector &keys, string_set &out);
    long sunionstore(const string &key_dst, const string_vector &keys);
    void smove(const string &key_src, const string &key_dst, const string &member);
    long scard(const string &key);
    bool sismember(const string &key, const string &member);
    long smembers(const string &key, string_set &out);
    string srandmember(const string &key);

    /*
     * Sorted_set
     */
    /**
     * @Brief
     *
     * @Param key
     * @Param score
     * @Param member
     *
     * @Returns  TRUE: member not exist in key
     *          FALSE: member already exist in key
     */
    bool zadd(const string &key, double score, const string &member);
    long zcard(const string &key);
    long zcount(const string &key, double min, double max);
    long zincrby(const string &key, double increment, const string &member);
    void zrange(const string &key, long start, long stop, string_vector &out);
    void zrange(const string &key, long start, long stop,
                bool withscores, key_score_vector &out);
    void zrangebyscore(const string &key, double min, double max, string_vector &out);
    void zrangebyscore(const string &key, double min, double max,
                       long offset, long count, string_vector &out);
    void zrangebyscore(const string &key, double min, double max,
                       const zset_opt &opt, key_score_vector &out);
    void zrem(const string &key, const string &member);
    long zremrangebyrank(const string &key, long start, long stop);
    long zremrangebyscore(const string &key, double min, double max);
    void zrevrange(const string &key, long start, long stop, string_vector &out);
    void zrevrange(const string &key, long start, long stop,
                   bool withscores, key_score_vector &out);
    void zrevrangebyscore(const string &key, long start, long stop,
                          string_vector &out);
    void zrevrangebyscore(const string &key, long start, long stop,
                          bool withscores, key_score_vector &out);
    double zscore(const string &key, const string &member);
    // zunionstore


    enum datatype
    {
        datatype_none = 0,      // key doesn't exist
        datatype_string,
        datatype_list,
        datatype_set,
        datatype_zset,
        datatype_hash,
    };

    /*
     * Keys
     */
    datatype type(const string &key);
    long keys(const string &pattern, string_vector &out);
    bool exists(const string &key);
    void del(const string &key);
    string randomkey();
    void rename(const string &old_name, const string &new_name);
    bool renamenx(const string &old_name, const string &new_name);
    string debug_object(const string &key);
    void expire(const string &key, unsigned int secs);
    void expireat(const string &key, time_t timestamp);
    long ttl(const string &key);
    void persist(const string &key);
    void move(const string &key, long dbindex);

    /*
     * Database
     */
    void save();
    void bgsave();
    void select(long dbindex);
    void flushdb();
    void flushall();
    long dbsize();
    time_t lastsave();

    /*
     * Server
     */
    void info(string_map &out);
    void info(const string &section, string_map &out);
    void quit(void);
    void publish(long channel, const string &message);
    void auth(const string &pass);
    void shutdown();
    void slaveof(const string &host, int port);

    void watch(const string_vector &keys);
    void watch(const string &key);
    void unwatch(void);
    void multi(void);
    void exec(void);
    void discard(void);

    /*
     * Configure
     */
    string config_get(const string &parameter);
    void config_set(const string &parameter, const string &value);
    void config_set(const string &parameter, long value);

    //
    // Sorting
    // Just go read http://code.google.com/p/redis/wiki/SortCommand
    //

    enum sort_order
    {
      sort_order_ascending,
      sort_order_descending
    };

    long sort(const string &key,
              string_vector &out,
              sort_order order = sort_order_ascending,
              bool lexicographically = false);

    long sort(const string &key,
              string_vector &out,
              long limit_start,
              long limit_end,
              sort_order order = sort_order_ascending,
              bool lexicographically = false);

    long sort(const string &key,
              string_vector &out,
              const string &by_pattern,
              long limit_start,
              long limit_end,
              const string_vector &get_patterns,
              sort_order order = sort_order_ascending,
              bool lexicographically = false);

private:
    void _init(const string &host, int port);
    connection(const connection &);
    connection& operator=(const connection &);

    void _send(const string &);
    string _read_n(ssize_t n);
    string _read_line(ssize_t max_size = 2048);

    void _recv_ok_reply();
    void _recv_int_ok_reply();
    long _recv_bulk_reply(char prefix);
    key_value _recv_kv_reply(void);
    // void _recv_kvs_reply(void);
    long _recv_multi_bulk_reply(string_vector &out);
    long _recv_multi_bulk_reply(string_set &out);
    long _recv_int_reply(void);
    bool _recv_bool_reply(void);
    double _recv_double_reply(void);
    string _recv_bulk_reply(void);
    string _recv_single_line_reply(void);
private:
    int _socket;

    static string missing_value;
};

}

#endif  /* REDIS_CPP_HPP_201505161505 */

