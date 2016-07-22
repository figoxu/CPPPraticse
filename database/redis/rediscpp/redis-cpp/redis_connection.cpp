/* {{{
 * =============================================================================
 *      Filename    :   redis_cpp.cpp
 *      Description :
 *      Created     :   2015-05-16 15:05:21
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <iostream>

#include <sstream>
#include <cstring>
#include <cassert>

#include <unistd.h>
#include <sys/errno.h>
#include <sys/socket.h>

#define NDEBUG

#ifndef NDEBUG
#include <algorithm>
#include <ctime>
#endif

#include "redis_connection.hpp"
#include "redis_utils.hpp"
#include "redis_error.hpp"
#include "redis_command.hpp"
#include "anet.h"

using namespace redis;
using namespace std;

#ifndef NDEBUG
void output_proto_msg(const string & data, bool is_received)
{
    string escaped_data(data);
    size_t pos;
    while ((pos = escaped_data.find("\n")) != string::npos)
      escaped_data.replace(pos, 1, "\\n");
    while ((pos = escaped_data.find("\r")) != string::npos)
      escaped_data.replace(pos, 1, "\\r");

    cerr << time(NULL) << ": "
         << (is_received ? "RECV '" : "SEND '")
         << escaped_data
         << "'"
         << endl;
}

#define OUTPUT_PROTO_TX_MSG(msg) output_proto_msg(msg, false)
#define OUTPUT_PROTO_RX_MSG(msg) output_proto_msg(msg, true)

#else
#define OUTPUT_PROTO_TX_MSG(msg)
#define OUTPUT_PROTO_RX_MSG(msg)
#endif

template <typename T>
T value_from_string(const string & data)
{
    T value;

    istringstream iss(data);
    iss >> value;
    if (iss.fail()) {
        throw redis::value_error("invalid number");
    }

    return value;
}

const string status_reply_ok("OK");
const string prefix_status_reply_error("-ERR ");

const char prefix_status_reply_value = '+';
const char prefix_single_bulk_reply  = '$';
const char prefix_multi_bulk_reply   = '*';
const char prefix_int_reply          = ':';

const string server_info_key_version                    = "redis_version";
const string server_info_key_bgsave_in_progress         = "bgsave_in_progress";
const string server_info_key_connected_clients          = "connected_clients";
const string server_info_key_connected_slaves           = "connected_slaves";
const string server_info_key_used_memory                = "used_memory";
const string server_info_key_changes_since_last_save    = "changes_since_last_save";
const string server_info_key_last_save_time             = "last_save_time";
const string server_info_key_total_connections_received = "total_connections_received";
const string server_info_key_total_commands_processed   = "total_commands_processed";
const string server_info_key_uptime_in_seconds          = "uptime_in_seconds";
const string server_info_key_uptime_in_days             = "uptime_in_days";
const string server_info_key_role                       = "role";

const string server_info_value_role_master = "master";
const string server_info_value_role_slave  = "slave";

string connection::missing_value("**nonexistent-key**");

connection::connection(void)
{
    _init("localhost", 6379);
}

connection::connection(const string &host)
{
    _init(host, 6379);
}

connection::connection(const string &host, int port)
{
    _init(host, port);
}

void connection::_init(const string &host, int port)
{
    char err[ANET_ERR_LEN] = {0};

    _socket = anetTcpConnect(err, const_cast<char*>(host.c_str()), port);
    if (_socket == ANET_ERR) { throw connection_error(err); }

    // cout << "Connect to " << host << ":" << port
    //      << " socket fd:" << _socket << endl;

    anetTcpNoDelay(NULL, _socket);
}

connection::~connection(void)
{
    if(_socket != ANET_ERR)
        close(_socket);
}

void connection::set(const string &key, const string &value)
{
    _send(command("SET") << key << value);
    _recv_ok_reply();
}

bool connection::setnx(const string &key, const string &value)
{
    _send(command("SETNX") << key << value);
    return _recv_int_reply() == 1;
}

void connection::setex(const string &key, int expire_sec, const string &value)
{
    _send(command("SETEX") << key << expire_sec << value);
    _recv_ok_reply();
}

long connection::setrange(const string &key, int pos, const string &value)
{
    _send(command("SETRANGE") << key << pos << value);
    return _recv_int_reply();
}

void connection::mset(const string_vector &keys, const string_vector &values)
{
    command cmd("MSET");
    if(keys.size() != values.size()) { return; }

    for(string_vector::size_type i = 0; i < keys.size(); ++i) {
        cmd << keys[i] << values[i];
    }

    _recv_ok_reply();
}

bool connection::msetnx(const string_vector &keys, const string_vector &values)
{
    command cmd("MSETNX");
    if(keys.size() != values.size()) { return false; }

    for(string_vector::size_type i = 0; i < keys.size(); ++i) {
        cmd << keys[i] << values[i];
    }

    return _recv_bool_reply();
}

string connection::get(const string &key)
{
    _send(command("GET") << key);
    return _recv_bulk_reply();
}

string connection::getset(const string &key, const string &value)
{
    _send(command("GETSET") << key << value);
    return _recv_bulk_reply();
}

string connection::getrange(const string &key, int pos_start, int pos_end)
{
    _send(command("GETRANGE") << key << pos_start << pos_end);

    return _recv_bulk_reply();
}

void connection::mget(const string_vector &keys, string_vector &out)
{
    _send(command("MGET") << keys);
    _recv_multi_bulk_reply(out);
}

long connection::incr(const string &key)
{
    _send(command("INCR") << key);
    return _recv_int_reply();
}

long connection::incrby(const string &key, long by)
{
    _send(command("INCRBY") << key << by);
    return _recv_int_reply();
}

long connection::decr(const string &key)
{
    _send(command("DECR") << key);
    return _recv_int_reply();
}

long connection::decrby(const string &key, long by)
{
    _send(command("DECRBY") << key << by);
    return _recv_int_reply();
}

long connection::append(const string &key, const string &value)
{
    _send(command("APPEND") << key << value);

    return _recv_int_reply();
}

long connection::strlen(const string &key)
{
    _send(command("STRLEN") << key);

    return _recv_int_reply();
}

bool connection::hset(const string &key, const string &field, const string &value)
{
    _send(command("HSET") << key << field << value);
    return _recv_bool_reply();
}

bool connection::hsetnx(const string &key, const string &field, const string &value)
{
    _send(command("HSETNX") << key << field << value);

    return _recv_bool_reply();
}

void connection::hmset(const string &key,
                       const string_vector &fields,
                       const string_vector &values)
{
    if(fields.size() != values.size()) { return; }

    command cmd("HMSET");
    cmd << key;
    for(string_vector::size_type i = 0; i < fields.size(); ++i) {
        cmd << fields[i] << values[i];
    }

    _send(cmd);

    _recv_ok_reply();
}

string connection::hget(const string &key, const string &field)
{
    _send(command("HGET") << key << field);
    return _recv_bulk_reply();
}

void connection::hmget(const string &key,
                       const string_vector &fields,
                       string_vector &values)
{
    _send(command("HMGET") << key << fields);

    _recv_multi_bulk_reply(values);
}

long connection::hincrby(const string &key, const string &field, long by)
{
    _send(command("HINCRBY") << key << field << by);
    return _recv_int_reply();
}

bool connection::hexists(const string &key, const string &field)
{
    _send(command("HEXISTS") << key << field);
    return _recv_bool_reply();
}

long connection::hlen(const string &key)
{
    _send(command("HLEN") << key);
    return _recv_int_reply();
}

void connection::hdel(const string &key, const string &field)
{
    _send(command("HDEL") << key << field);
    _recv_int_ok_reply();
}

void connection::hkeys(const string &key, string_vector &fields)
{
    _send(command("HfieldS") << key);
    _recv_multi_bulk_reply(fields);
}

void connection::hvals(const string &key, string_vector &values)
{
    _send(command("HVALS") << key);
    _recv_multi_bulk_reply(values);
}

void connection::hgetall(const string &key,
                         string_vector &fields,
                         string_vector &values)
{
    _send(command("HGETALL") << key);

    string_vector all;
    _recv_multi_bulk_reply(all);

    for(string_vector::size_type i = 0; i < all.size() / 2; ++i) {
        fields.push_back(all[i * 2]);
        values.push_back(all[i * 2 + 1]);
    }
}

/*
 * List
 */
long connection::rpush(const string &key, const string &value)
{
    _send(command("RPUSH") << key << value);
    return _recv_int_reply();
}

long connection::lpush(const string &key, const string &value)
{
    _send(command("LPUSH") << key << value);
    return _recv_int_reply();
}

long connection::rpushx(const string &key, const string &value)
{
    _send(command("RPUSHX") << key << value);
    return _recv_int_reply();
}

long connection::lpushx(const string &key, const string &value)
{
    _send(command("LPUSHX") << key << value);
    return _recv_int_reply();
}

long connection::llen(const string & key)
{
    _send(command("LLEN") << key);
    return _recv_int_reply();
}

long connection::lrange(const string & key, long start, long end, string_vector &out)
{
    _send(command("LRANGE") << key << start << end);
    return _recv_multi_bulk_reply(out);
}

void connection::ltrim(const string &key, long start, long end)
{
    _send(command("LTRIM") << key << start << end);
    _recv_ok_reply();
}

string connection::lindex(const string &key, long index)
{
    _send(command("LINDEX") << key << index);
    return _recv_bulk_reply();
}

void connection::lset(const string &key, long index, const string &value)
{
    _send(command("LSET") << key << index << value);
    _recv_ok_reply();
}

long connection::lrem(const string &key, long count, const string &value)
{
    _send(command("LREM") << key << count << value);
    return _recv_int_reply();
}

string connection::lpop(const string & key)
{
    _send(command("LPOP") << key);
    return _recv_bulk_reply();
}

string connection::blpop(const string &key, long timeout)
{
    _send(command("BLPOP") << key << timeout);

    return _recv_kv_reply().val;
}

key_value connection::blpop(const string_vector &keys, long timeout)
{
    _send(command("BLPOP") << keys << timeout);

    return _recv_kv_reply();
}

string connection::rpop(const string & key)
{
    _send(command("RPOP") << key);
    return _recv_bulk_reply();
}

string connection::brpop(const string &key, long timeout)
{
    _send(command("BRPOP") << key << timeout);
    return _recv_kv_reply().val;
}

key_value connection::brpop(const string_vector &keys, long timeout)
{
    _send(command("BRPOP") << keys << timeout);
    return _recv_kv_reply();
}

string connection::rpoplpush(const string &list_pop, const string &list_push)
{
    _send(command("RPOPLPUSH") << list_pop << list_push);
    return _recv_bulk_reply();
}

string connection::brpoplpush(const string &list_pop,
                              const string &list_push,
                              long timeout)
{
    _send(command("BRPOPLPUSH") << list_pop << list_push << timeout);
    return _recv_bulk_reply();
}

/*
 * Set
 */
void connection::sadd(const string &key, const string &member)
{
    _send(command("SADD") << key << member);
    _recv_int_ok_reply();
}

void connection::srem(const string &key, const string &member)
{
    _send(command("SREM") << key << member);
    _recv_int_ok_reply();
}

string connection::spop(const string &key)
{
    _send(command("SPOP") << key);
    return _recv_bulk_reply();
}

long connection::sdiff(const string_vector &keys, string_set &out)
{
    _send(command("SDIFF") << keys);
    return _recv_multi_bulk_reply(out);
}

long connection::sdiffstore(const string &key_dst, const string_vector &keys)
{
    _send(command("SDIFFSTORE") << key_dst << keys);
    return _recv_int_reply();
}

long connection::sinter(const string_vector &keys, string_set &out)
{
    _send(command("SINTER") << keys);
    return _recv_multi_bulk_reply(out);
}

long connection::sinterstore(const string &key_dst, const string_vector &keys)
{
    _send(command("SINTERSTORE") << key_dst << keys);
    return _recv_int_reply();
}

long connection::sunion(const string_vector &keys, string_set &out)
{
    _send(command("SUNION") << keys);
    return _recv_multi_bulk_reply(out);
}

long connection::sunionstore(const string &key_dst, const string_vector &keys)
{
    _send(command("SUNIONSTORE") << key_dst << keys);
    return _recv_int_reply();
}


void connection::smove(const string &key_src,
                       const string &key_dst,
                       const string &member)
{
    _send(command("SMOVE") << key_src << key_dst << member);
    _recv_int_ok_reply();
}

long connection::scard(const string &key)
{
    _send(command("SCARD") << key);
    return _recv_int_reply();
}

bool connection::sismember(const string &key, const string &member)
{
    _send(command("SISMEMBER") << key << member);
    return _recv_bool_reply();
}

long connection::smembers(const string &key, string_set &out)
{
    _send(command("SMEMBERS") << key);
    return _recv_multi_bulk_reply(out);
}

string connection::srandmember(const string &key)
{
    _send(command("SRANDMEMBER") << key);
    return _recv_bulk_reply();
}

/*
 * Sorted_set
 */
bool connection::zadd(const string &key, double score, const string &member)
{
    _send(command("ZADD") << key << score << member);
    return _recv_bool_reply();
}

long connection::zcard(const string &key)
{
    _send(command("ZCARD") << key);
    return _recv_int_reply();
}

long connection::zcount(const string &key, double min, double max)
{
    _send(command("ZCOUNT") << key << min << max);
    return _recv_int_reply();
}

long connection::zincrby(const string &key, double increment, const string &member)
{
    _send(command("ZINCRBY") << key << increment << member);
    return _recv_int_reply();
}

void connection::zrange(const string &key, long start, long stop, string_vector &out)
{
    _send(command("ZRANGE") << key << start << stop);
    _recv_multi_bulk_reply(out);
}

void connection::zrange(const string &key, long start, long stop,
                        bool withscores, key_score_vector &out)
{
    command cmd("ZRANGE");
    cmd << key << start << stop;

    if(withscores) cmd << "WITHSCORES";

    _send(cmd);
    // _recv_multi_bulk_reply(out);
}

void connection::zrangebyscore(const string &key, double min, double max,
                               string_vector &out)
{
    _send(command("ZRANGEBYSCORE") << key << min << max);
    _recv_multi_bulk_reply(out);
}

void connection::zrangebyscore(const string &key, double min, double max,
                               long offset, long count, string_vector &out)
{
    _send(command("ZRANGEBYSCORE") << key << min << max << "LIMIT" << offset << count);
    _recv_multi_bulk_reply(out);
}

void connection::zrangebyscore(const string &key, double min, double max,
                               const zset_opt &opt, key_score_vector &out)
{
    command cmd("ZRANGEBYSCORE");
    cmd << key << min << max;

    if(opt.is_withscores()) { cmd << "WITHSCORES"; }
    if(opt.is_limit()) {
        cmd << "LIMIT" << opt.get_limit_offset() << opt.get_limit_count();
    }

    _send(cmd);
}

void connection::zrem(const string &key, const string &member)
{
    _send(command("ZREM") << key << member);
    _recv_int_ok_reply();
}

long connection::zremrangebyrank(const string &key, long start, long stop)
{
    _send(command("ZREMRANGEBYRANK") << key << start << stop);
    return _recv_int_reply();
}

long connection::zremrangebyscore(const string &key, double min, double max)
{
    _send(command("ZREMRANGEBYSCORE") << key << min << max);
    return _recv_int_reply();
}

void connection::zrevrange(const string &key, long start, long stop, string_vector &out)
{
    _send(command("ZREVRANGE") << start << stop);
    _recv_multi_bulk_reply(out);
}

void connection::zrevrange(const string &key, long start, long stop,
                           bool withscores, key_score_vector &out)
{
    command cmd("ZREVRANGE");
    cmd << key << start << stop;
    if(withscores) cmd << "WITHSCORES";

}

void connection::zrevrangebyscore(const string &key, long start, long stop,
                                  string_vector &out)
{
    _send(command("ZREVRANGEBYSCORE") << key << start << stop);
    _recv_multi_bulk_reply(out);
}

void connection::zrevrangebyscore(const string &key, long start, long stop,
                                  bool withscores, key_score_vector &out)
{

}

double connection::zscore(const string &key, const string &member)
{
    _send(command("ZSCORE") << key << member);
    return _recv_double_reply();
}

/*
 * Keys
 */
connection::datatype connection::type(const string &key)
{
    _send(command("TYPE") << key);
    string response = _recv_single_line_reply();

    if (response == "none")   return datatype_none;
    if (response == "string") return datatype_string;
    if (response == "list")   return datatype_list;
    if (response == "set")    return datatype_set;
    if (response == "zset")   return datatype_zset;
    if (response == "hash")   return datatype_hash;

    return datatype_none;
}

long connection::keys(const string &pattern, string_vector &out)
{
    _send(command("KEYS") << pattern);
    return _recv_multi_bulk_reply(out);
}

bool connection::exists(const string &key)
{
    _send(command("EXISTS") << key);
    return _recv_int_reply() == 1;
}

void connection::del(const string &key)
{
    _send(command("DEL") << key);
    _recv_int_ok_reply();
}

string connection::randomkey()
{
    _send(command("RANDOMKEY"));
    return _recv_single_line_reply();
}

void connection::rename(const string &old_name, const string &new_name)
{
    _send(command("RENAME") << old_name << new_name);
    _recv_ok_reply();
}

bool connection::renamenx(const string &old_name, const string &new_name)
{
    _send(command("RENAMENX") << old_name << new_name);
    return _recv_int_reply() == 1;
}

string connection::debug_object(const string &key)
{
    _send(command("DEBUG OBJECT") << key);
    return _recv_bulk_reply();
}

void connection::expire(const string &key, unsigned int secs)
{
    _send(command("EXPIRE") << key << secs);
    _recv_int_ok_reply();
}

void connection::expireat(const string &key, time_t timestamp)
{
    _send(command("EXPIREAT") << key << timestamp);
    _recv_int_ok_reply();
}

long connection::ttl(const string &key)
{
    _send(command("TTL") << key);
    return _recv_int_reply();
}

void connection::persist(const string &key)
{
    _send(command("PERSIST") << key);
    _recv_ok_reply();
}

void connection::move(const string & key,
                    long dbindex)
{
    _send(command("MOVE") << key << dbindex);
    _recv_int_ok_reply();
}

/*
 * Database
 */
void connection::save()
{
    _send(command("SAVE"));
    _recv_ok_reply();
}

void connection::bgsave()
{
    _send(command("BGSAVE"));
    _recv_ok_reply();
}

void connection::select(long dbindex)
{
    _send(command("SELECT") << dbindex);
    _recv_ok_reply();
}

void connection::flushdb()
{
    _send(command("FLUSHDB"));
    _recv_ok_reply();
}

void connection::flushall()
{
    _send(command("FLUSHALL"));
    _recv_ok_reply();
}

long connection::dbsize(void)
{
    _send(command("DBSIZE"));
    return _recv_int_reply();
}

time_t connection::lastsave()
{
    _send(command("LASTSAVE"));
    return _recv_int_reply();
}


/*
 * Server
 */
void connection::info(string_map &out)
{
    info("", out);
}

void connection::info(const string &section, string_map &out)
{
    _send(command("INFO") << section);
    string response = _recv_bulk_reply();

    if(response.empty()) { throw protocol_error("empty"); }

    string_vector lines;
    split(response, command::CRLF, lines);
    if (lines.empty()) { throw protocol_error("empty line for info"); }

    for (string_vector::const_iterator it = lines.begin();
        it != lines.end(); ++it) {
        const string &line = *it;
        string_vector line_parts;

        /* Section */
        if(line.size() > 0 && line.c_str()[0] == '#') {
            if(section != "" && line.substr(2, section.size()) != section) {
                throw protocol_error("unexpected section name for info");
            } else {
                continue;
            }
        }

        /* Blank line */
        if(line.size() <= 0) continue;

        /* Content */
        split(line, ":", line_parts);
        if (line_parts.size() != 2) {
            throw protocol_error(line + "unexpected line format for info");
        }

        out[line_parts[0]] = line_parts[1];
    }
}

void connection::auth(const string &pass)
{
    _send(command("AUTH") << pass);
    _recv_ok_reply();
}

void connection::shutdown()
{
    _send(command("SHUTDOWN"));

    try {
        _recv_ok_reply();
    } catch (connection_error & e) {
    }
}

void connection::watch(const string_vector &keys)
{
    _send(command("WATCH") << keys);
    _recv_ok_reply();
}

void connection::watch(const string &key)
{
    _send(command("WATCH") << key);
    _recv_ok_reply();
}

void connection::unwatch(void)
{
    _send(command("UNWATCH"));
    _recv_ok_reply();
}

void connection::multi(void)
{
    _send(command("MULTI"));
    _recv_ok_reply();
}

void connection::exec(void)
{
    _send(command("EXEC"));
    _recv_ok_reply();
}

void connection::discard(void)
{
    _send(command("DISCARD"));
    _recv_ok_reply();
}

/*
 * Configure
 */
string connection::config_get(const string &parameter)
{
    _send(command("CONFIG GET") << parameter);
    return _recv_bulk_reply();
}

void connection::config_set(const string &parameter, const string &value)
{
    _send(command("CONFIG SET") << parameter << value);

    _recv_ok_reply();
}

void connection::config_set(const string &parameter, long value)
{
    _send(command("CONFIG SET") << parameter << value);

    _recv_ok_reply();
}

long connection::sort(const string & key,
                      string_vector & out,
                      long limit_start,
                      long limit_end,
                      connection::sort_order order,
                      bool lexicographically)
{
    _send(command("SORT") << key
          << "LIMIT" << limit_start << limit_end
          << (order == sort_order_ascending ? "ASC" : "DESC")
          << (lexicographically ? "ALPHA" : ""));

    return _recv_multi_bulk_reply(out);
}

long connection::sort(const string & key,
                      string_vector & out,
                      connection::sort_order order,
                      bool lexicographically)
{
    _send(command("SORT") << key
          << (order == sort_order_ascending ? "ASC" : "DESC")
          << (lexicographically ? "ALPHA" : ""));

    return _recv_multi_bulk_reply(out);
}

long connection::sort(const string & key,
                      string_vector & out,
                      const string & by_pattern,
                      long limit_start,
                      long limit_end,
                      const string_vector & get_patterns,
                      connection::sort_order order,
                      bool lexicographically)
{
    command m("SORT");

    m << key     << "BY"        << by_pattern
      << "LIMIT" << limit_start << limit_end;

    string_vector::const_iterator it = get_patterns.begin();
    for ( ; it != get_patterns.end(); ++it){
        m << "GET" << *it;
    }

    m << (order == sort_order_ascending ? "ASC" : "DESC")
      << (lexicographically ? "ALPHA" : "");

    _send(m);

    return _recv_multi_bulk_reply(out);
}

void connection::_send(const string & msg)
{
    OUTPUT_PROTO_TX_MSG(msg);

    if(anetWrite(_socket, const_cast<char *>(msg.data()), msg.size()) == -1)
      throw connection_error(strerror(errno));
}

  // Reads N bytes from given blocking socket.
string connection::_read_n(ssize_t n)
{
    char * buffer = new char[n + 1];
    buffer[n] = '\0';

    char * bp = buffer;
    ssize_t bytes_read = 0;

    while (bytes_read != n)
    {
      ssize_t bytes_received = 0;
      do bytes_received = recv(_socket, bp, n - (bp - buffer), 0);
      while (bytes_received < 0 && errno == EINTR);

      if (bytes_received == 0)
        throw redis::connection_error("connection was closed");

      bytes_read += bytes_received;
      bp         += bytes_received;
    }

    string str(buffer);
    delete [] buffer;
    return str;
}

  // Reads a single line of character data from the given blocking socket.
  // Returns the line that was read, not including EOL delimiter(s).  Both LF
  // ('\n') and CRLF ("\r\n") delimiters are supported.  If there was an I/O
  // error reading from the socket, connection_error is raised.  If max_size
  // bytes are read before finding an EOL delimiter, a blank string is
  // returned.

string connection::_read_line(ssize_t max_size)
{
    assert(_socket > 0);
    assert(max_size > 0);

    ostringstream oss;

    enum { buffer_size = 64 };
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);

    ssize_t total_bytes_read = 0;
    bool found_delimiter = false;

    while (total_bytes_read < max_size && !found_delimiter)
    {
      // Peek at what's available.

      ssize_t bytes_received = 0;
      do bytes_received = recv(_socket, buffer, buffer_size, MSG_PEEK);
      while (bytes_received < 0 && errno == EINTR);

      if (bytes_received == 0)
        throw redis::connection_error("connection was closed");

      // Some data is available; Length might be < buffer_size.
      // Look for newline in whatever was read though.

      char * eol = static_cast<char *>(memchr(buffer, '\n', bytes_received));

      // If found, write data from the buffer to the output string.
      // Else, write the entire buffer and continue reading more data.

      ssize_t to_read = bytes_received;

      if (eol) {
        to_read = eol - buffer + 1;
        oss.write(buffer, to_read);
        found_delimiter = true;
      }
      else
        oss.write(buffer, bytes_received);

      // Now read from the socket to remove the peeked data from the socket's
      // read buffer.  This will not block since we've peeked already and know
      // there's data waiting.  It might fail if we were interrupted however.

      do bytes_received = recv(_socket, buffer, to_read, 0);
      while (bytes_received < 0 && errno == EINTR);
    }

    // Construct final line string. Remove trailing CRLF-based whitespace.

    string line = oss.str();
    return rtrim(line, command::CRLF);
}

string connection::_recv_single_line_reply()
{
    string line = _read_line();

    OUTPUT_PROTO_RX_MSG(line);

    if(line.empty()){
        throw protocol_error("empty single line reply");
    }

    if(line.find(prefix_status_reply_error) == 0) {
        string error_msg = line.substr(prefix_status_reply_error.length());
        if(error_msg.empty()) {
            error_msg = "unknown error";
        }
        throw protocol_error(error_msg);
    }

    if(line[0] != prefix_status_reply_value) {
        throw protocol_error("unexpected prefix for status reply");
    }

    return line.substr(1);
}

void connection::_recv_ok_reply()
{
    if (_recv_single_line_reply() != status_reply_ok)
      throw protocol_error("expected OK response");
}

key_value connection::_recv_kv_reply(void)
{
    string_vector out;
    _recv_multi_bulk_reply(out);

    if(out.size() < 2) {
        throw protocol_error("unexpected response for key_value");
    }

    key_value kv;
    kv.key = out[0];
    kv.val = out[1];

    return kv;
}

long connection::_recv_bulk_reply(char prefix)
{
    string line = _read_line();

    OUTPUT_PROTO_RX_MSG(line);

    if (line[0] != prefix)
      throw protocol_error("unexpected prefix for bulk reply");

    return value_from_string<long>(line.substr(1));
}

string connection::_recv_bulk_reply()
{
    long length = _recv_bulk_reply(prefix_single_bulk_reply);

    if (length == -1)
      return connection::missing_value;

    long real_length = length + 2;    // CRLF

    string data = _read_n(real_length);

    OUTPUT_PROTO_RX_MSG(data.substr(0, data.length()-2));

    if (data.empty())
      throw protocol_error("invalid bulk reply data; empty");

    if (data.length() != static_cast<string::size_type>(real_length))
      throw protocol_error("invalid bulk reply data; data of unexpected length");

    data.erase(data.size() - 2);

    return data;
}

long connection::_recv_multi_bulk_reply(string_vector &out)
{
    long length = _recv_bulk_reply(prefix_multi_bulk_reply);

    if (length == -1)
      throw key_error("no such key");

    for (long i = 0; i < length; ++i)
      out.push_back(_recv_bulk_reply());

    return length;
}

long connection::_recv_multi_bulk_reply(string_set & out)
{
    long length = _recv_bulk_reply(prefix_multi_bulk_reply);

    if (length == -1)
      throw key_error("no such key");

    for (long i = 0; i < length; ++i)
      out.insert(_recv_bulk_reply());

    return length;
}

long connection::_recv_int_reply()
{
    string line = _read_line();

    OUTPUT_PROTO_RX_MSG(line);

    if (line.empty())
      throw protocol_error("invalid integer reply; empty");

    if (line[0] != prefix_int_reply)
      throw protocol_error("unexpected prefix for integer reply");

    return value_from_string<long>(line.substr(1));
}

double connection::_recv_double_reply(void)
{
    string line = _read_line();

    OUTPUT_PROTO_RX_MSG(line);

    if (line.empty()) {
        throw protocol_error("invalid double reply; empty");
    }

    if (line[0] != prefix_single_bulk_reply) {
        throw protocol_error("unexpected prefix for double reply");
    }

    line = _read_line();
    OUTPUT_PROTO_RX_MSG(line);

    if (line.empty()) {
        throw protocol_error("invalid double reply; empty");
    }

    return value_from_string<double>(line);
}

void connection::_recv_int_ok_reply()
{
    if (_recv_int_reply() != 1)
      throw protocol_error("expecting int reply of 1");
}

bool connection::_recv_bool_reply(void)
{
    return _recv_int_reply() == 1;
}
