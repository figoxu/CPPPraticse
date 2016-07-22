#ifndef REDIS_CMD_HPP_201505161605
#define REDIS_CMD_HPP_201505161605

/* {{{
 * =============================================================================
 *      Filename    :   redis_cmd.hpp
 *      Description :
 *      Created     :   2015-05-16 16:05:49
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <sstream>
#include <string>
#include <vector>

namespace redis
{

using namespace std;
class command
{
public:
    const static string CRLF;

    explicit command(const string &cmd) {
        _buffer << cmd;
    }

    template <typename T>
    command& operator<<(T const & datum) {
        _buffer << ' ' << datum;
        return *this;
    }

    template <typename T>
    command& operator<<(const vector<T> &data) {
        size_t n = data.size();
        for (size_t i = 0; i < n; ++i) {
            _buffer << ' ' << data[i];
        }
        return *this;
    }

    operator string (void) {
        _buffer << CRLF;
        return _buffer.str();
    }
private:
    ostringstream _buffer;
};

}

#endif  /* REDIS_CMD_HPP_201505161605 */

