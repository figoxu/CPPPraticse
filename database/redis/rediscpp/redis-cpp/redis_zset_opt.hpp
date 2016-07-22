#ifndef REDIS_ZSET_OPT_HPP_201505181105
#define REDIS_ZSET_OPT_HPP_201505181105

/* {{{
 * =============================================================================
 *      Filename    :   redis_zset_opt.hpp
 *      Description :
 *      Created     :   2015-05-18 11:05:36
 *      Author      :   Wu Hong
 * =============================================================================
 }}} */

namespace redis
{

class zset_opt
{
public:
    zset_opt(void);

    bool is_withscores(void) const { return withscores; }
    bool is_limit(void) const { return limit; }
    long get_limit_offset(void) const { return limit_offset; }
    long get_limit_count(void) const { return limit_count; }
// private:
    bool withscores;
    bool limit;
    long limit_offset;
    long limit_count;
};

}

#endif  /* REDIS_ZSET_OPT_HPP_201505181105 */

