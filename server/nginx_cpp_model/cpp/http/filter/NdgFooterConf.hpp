// Copyright (c) 2015
// Author: Chrono Law
#ifndef _NDG_FOOTER_CONF_HPP
#define _NDG_FOOTER_CONF_HPP

#include "NgxAll.hpp"

class NdgFooterConf final
{
public:
    typedef NdgFooterConf this_type;
public:
    NdgFooterConf() = default;
    ~NdgFooterConf() = default;
public:
    ngx_array_t*    headers;
    ngx_str_t       footer;
public:
    static void* create(ngx_conf_t* cf)
    {
        return NgxPool(cf).alloc<this_type>();
    }

};

struct NdgFooterCtx final
{
    int flag = 0;
};

NGX_MOD_INSTANCE(NdgFooterModule, ndg_footer_module, NdgFooterConf)

#endif  //_NDG_FOOTER_CONF_HPP
