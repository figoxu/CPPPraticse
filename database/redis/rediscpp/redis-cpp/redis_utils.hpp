#ifndef REDIS_UTILS_HPP_201505162005
#define REDIS_UTILS_HPP_201505162005

/* {{{
 * =============================================================================
 *      Filename    :   redis_utils.hpp
 *      Description :
 *      Created     :   2015-05-16 20:05:30
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

#include <string>
#include <vector>

namespace redis
{

std::string& rtrim(std::string &str, const std::string &ws = " \f\n\r\t\v");

void split(const std::string &str,
           const std::string &delim,
           std::vector<std::string> &items);
}

#endif  /* REDIS_UTILS_HPP_201505162005 */

