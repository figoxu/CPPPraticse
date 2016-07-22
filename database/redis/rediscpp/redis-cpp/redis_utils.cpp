/* {{{
 * =============================================================================
 *      Filename    :   redis_utils.cpp
 *      Description :
 *      Created     :   2015-05-16 20:05:59
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */
#include <sstream>
#include <cassert>
#include <cerrno>
#include "redis_utils.hpp"

using namespace std;

namespace redis
{

string& rtrim(string &str, const string &ws)
{
    string::size_type pos = str.find_last_not_of(ws);
    str.erase(pos + 1);
    return str;
}

void split(const string &str, const string &delim, vector<string> &items)
{
    string::size_type pos_old = 0, pos_new = 0;
    string::size_type size_delim = delim.length();

    while(1) {
        pos_new = str.find(delim, pos_old);
        if(pos_new != string::npos) {
            items.push_back(str.substr(pos_old, pos_new - pos_old));
            pos_old = pos_new + size_delim;
        } else if(pos_old <= str.size()) {
            items.push_back(str.substr(pos_old));
            break;
        } else {
            break;
        }
    }
}

}
