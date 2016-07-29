// annotated by chrono since 2016
//
// * ngx_stream_upstream
// * ngx_stream_upstream_server
// * ngx_stream_upstream_add

/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>


// 解析upstream{}块，它相当于server{}块，级别相同
// 创建一个upstream{}块的配置信息
// 检查是否有同名的upstream{}，如有则报错
// 加入main conf里的upstreams数组，之后就可以在这里找到所有的upstream{}
// 创建数组，存储upstream{}里的服务器信息
// 要求至少有一个server指定上游服务器，否则出错
static char *ngx_stream_upstream(ngx_conf_t *cf, ngx_command_t *cmd,
    void *dummy);

// 解析upstream{}里的server指令，确定一个上游服务器
// 在servers数组里添加一个元素
// 检查里面的weight等参数，设置ngx_stream_upstream_server_t结构体
// 从2开始，第一个是server的地址
// 要求server必须有端口号
static char *ngx_stream_upstream_server(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

// upstream{}的配置结构体，只有一个数组
// 用来存储每个上游upstream{}的信息
static void *ngx_stream_upstream_create_main_conf(ngx_conf_t *cf);

// 在ngx_stream_block解析完配置后调用，初始化main conf
// 检查每一个upstream{}块
// 查看是否有load balance初始化指针
// 没有指定特别的算法就使用默认的round robin
// 对本upstream{}块做初始化
static char *ngx_stream_upstream_init_main_conf(ngx_conf_t *cf, void *conf);


// 在stream{}块里的upstream相关指令，只有两个
static ngx_command_t  ngx_stream_upstream_commands[] = {

    // 定义upstream{}块
    { ngx_string("upstream"),
      NGX_STREAM_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_TAKE1,
      ngx_stream_upstream,
      0,
      0,
      NULL },

    // 定义上游的服务器地址
    { ngx_string("server"),
      NGX_STREAM_UPS_CONF|NGX_CONF_1MORE,
      ngx_stream_upstream_server,
      NGX_STREAM_SRV_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_stream_module_t  ngx_stream_upstream_module_ctx = {
    NULL,                                  /* postconfiguration */

    // upstream{}的配置结构体，只有一个数组
    // 用来存储每个上游upstream{}的信息
    ngx_stream_upstream_create_main_conf,  /* create main configuration */

    // 在ngx_stream_block解析完配置后调用，初始化main conf
    // 检查每一个upstream{}块
    // 查看是否有load balance初始化指针
    // 没有指定特别的算法就使用默认的round robin
    // 对本upstream{}块做初始化
    ngx_stream_upstream_init_main_conf,    /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
};


// upstream模块定义
ngx_module_t  ngx_stream_upstream_module = {
    NGX_MODULE_V1,
    &ngx_stream_upstream_module_ctx,       /* module context */
    ngx_stream_upstream_commands,          /* module directives */
    NGX_STREAM_MODULE,                     /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


// 解析upstream{}块，它相当于server{}块，级别相同
// 创建一个upstream{}块的配置信息
// 检查是否有同名的upstream{}，如有则报错
// 加入main conf里的upstreams数组，之后就可以在这里找到所有的upstream{}
// 创建数组，存储upstream{}里的服务器信息
// 要求至少有一个server指定上游服务器，否则出错
static char *
ngx_stream_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *dummy)
{
    char                            *rv;
    void                            *mconf;
    ngx_str_t                       *value;
    ngx_url_t                        u;
    ngx_uint_t                       m;
    ngx_conf_t                       pcf;
    ngx_stream_module_t             *module;
    ngx_stream_conf_ctx_t           *ctx, *stream_ctx;
    ngx_stream_upstream_srv_conf_t  *uscf;

    ngx_memzero(&u, sizeof(ngx_url_t));

    value = cf->args->elts;

    // 指令的第一个参数是upstream块的名字
    u.host = value[1];
    u.no_resolve = 1;
    u.no_port = 1;

    // 创建或者获取一个upstream{}块的配置信息
    // 获取时flags==0
    // 检查是否有同名的upstream{}，如果是创建时有则报错
    // 加入main conf里的upstreams数组，之后就可以在这里找到所有的upstream{}
    uscf = ngx_stream_upstream_add(cf, &u, NGX_STREAM_UPSTREAM_CREATE
                                           |NGX_STREAM_UPSTREAM_WEIGHT
                                           |NGX_STREAM_UPSTREAM_MAX_FAILS
                                           |NGX_STREAM_UPSTREAM_FAIL_TIMEOUT
                                           |NGX_STREAM_UPSTREAM_DOWN
                                           |NGX_STREAM_UPSTREAM_BACKUP);
    if (uscf == NULL) {
        return NGX_CONF_ERROR;
    }


    // tcp流处理的配置结构体
    // 与http不同的是没有location，只有两级
    // 在cycle->conf_ctx里存储的是stream{}级别的配置
    ctx = ngx_pcalloc(cf->pool, sizeof(ngx_stream_conf_ctx_t));
    if (ctx == NULL) {
        return NGX_CONF_ERROR;
    }

    // 保存stream{}的配置上下文
    // 也就是stream{}里的ngx_stream_conf_ctx_t
    stream_ctx = cf->ctx;

    // main_conf直接指向stream{}的main_conf
    ctx->main_conf = stream_ctx->main_conf;

    /* the upstream{}'s srv_conf */

    // 分配存储srv_conf的数组，数量是ngx_stream_max_module
    ctx->srv_conf = ngx_pcalloc(cf->pool,
                                sizeof(void *) * ngx_stream_max_module);
    if (ctx->srv_conf == NULL) {
        return NGX_CONF_ERROR;
    }

    // 存储本配置块的配置
    // ngx_stream_upstream_module相当于ngx_stream_core_module
    // 同时在main conf的servers数组里也有
    ctx->srv_conf[ngx_stream_upstream_module.ctx_index] = uscf;

    // 保存本upstream{}的配置数组
    uscf->srv_conf = ctx->srv_conf;

    // 遍历模块数组，调用每个stream模块create_srv_conf，创建配置结构体
    // 但实际上除了负载均衡模块，其他模块不会在这里出现，没有意义
    for (m = 0; ngx_modules[m]; m++) {
        if (ngx_modules[m]->type != NGX_STREAM_MODULE) {
            continue;
        }

        module = ngx_modules[m]->ctx;

        if (module->create_srv_conf) {
            mconf = module->create_srv_conf(cf);
            if (mconf == NULL) {
                return NGX_CONF_ERROR;
            }

            ctx->srv_conf[ngx_modules[m]->ctx_index] = mconf;
        }
    }

    // 创建数组，准备存储upstream{}里的服务器信息
    uscf->servers = ngx_array_create(cf->pool, 4,
                                     sizeof(ngx_stream_upstream_server_t));
    if (uscf->servers == NULL) {
        return NGX_CONF_ERROR;
    }


    /* parse inside upstream{} */

    // 暂存当前的解析上下文
    pcf = *cf;

    // 设置upstream模块的新解析上下文
    // 使用刚才刚创建的配置结构体存储模块的配置信息
    cf->ctx = ctx;
    cf->cmd_type = NGX_STREAM_UPS_CONF;

    // 递归解析事件相关模块
    // 里面主要是server指令和负载均衡算法指令
    rv = ngx_conf_parse(cf, NULL);

    // 恢复之前保存的解析上下文
    *cf = pcf;

    if (rv != NGX_CONF_OK) {
        return rv;
    }

    // 要求至少有一个server指定上游服务器，否则出错
    if (uscf->servers->nelts == 0) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "no servers are inside upstream");
        return NGX_CONF_ERROR;
    }

    return rv;
}


// 解析upstream{}里的server指令，确定一个上游服务器
// 在servers数组里添加一个元素
// 检查里面的weight等参数，设置ngx_stream_upstream_server_t结构体
// 从2开始，第一个是server的地址
// 要求server必须有端口号
static char *
ngx_stream_upstream_server(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_upstream_srv_conf_t  *uscf = conf;

    time_t                         fail_timeout;
    ngx_str_t                     *value, s;
    ngx_url_t                      u;
    ngx_int_t                      weight, max_fails;
    ngx_uint_t                     i;
    ngx_stream_upstream_server_t  *us;

    // 在servers数组里添加一个元素
    us = ngx_array_push(uscf->servers);
    if (us == NULL) {
        return NGX_CONF_ERROR;
    }

    ngx_memzero(us, sizeof(ngx_stream_upstream_server_t));

    value = cf->args->elts;

    weight = 1;
    max_fails = 1;
    fail_timeout = 10;

    // 检查里面的weight等参数，设置ngx_stream_upstream_server_t结构体
    // 从2开始，第一个是server的地址
    for (i = 2; i < cf->args->nelts; i++) {

        if (ngx_strncmp(value[i].data, "weight=", 7) == 0) {

            if (!(uscf->flags & NGX_STREAM_UPSTREAM_WEIGHT)) {
                goto not_supported;
            }

            weight = ngx_atoi(&value[i].data[7], value[i].len - 7);

            if (weight == NGX_ERROR || weight == 0) {
                goto invalid;
            }

            continue;
        }

        if (ngx_strncmp(value[i].data, "max_fails=", 10) == 0) {

            if (!(uscf->flags & NGX_STREAM_UPSTREAM_MAX_FAILS)) {
                goto not_supported;
            }

            max_fails = ngx_atoi(&value[i].data[10], value[i].len - 10);

            if (max_fails == NGX_ERROR) {
                goto invalid;
            }

            continue;
        }

        if (ngx_strncmp(value[i].data, "fail_timeout=", 13) == 0) {

            if (!(uscf->flags & NGX_STREAM_UPSTREAM_FAIL_TIMEOUT)) {
                goto not_supported;
            }

            s.len = value[i].len - 13;
            s.data = &value[i].data[13];

            fail_timeout = ngx_parse_time(&s, 1);

            if (fail_timeout == (time_t) NGX_ERROR) {
                goto invalid;
            }

            continue;
        }

        if (ngx_strcmp(value[i].data, "backup") == 0) {

            if (!(uscf->flags & NGX_STREAM_UPSTREAM_BACKUP)) {
                goto not_supported;
            }

            us->backup = 1;

            continue;
        }

        if (ngx_strcmp(value[i].data, "down") == 0) {

            if (!(uscf->flags & NGX_STREAM_UPSTREAM_DOWN)) {
                goto not_supported;
            }

            us->down = 1;

            continue;
        }

        goto invalid;
    }

    // 第一个参数是server地址，解析之
    ngx_memzero(&u, sizeof(ngx_url_t));

    u.url = value[1];

    if (ngx_parse_url(cf->pool, &u) != NGX_OK) {
        if (u.err) {
            ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                               "%s in upstream \"%V\"", u.err, &u.url);
        }

        return NGX_CONF_ERROR;
    }

    // 要求server必须有端口号
    if (u.no_port) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "no port in upstream \"%V\"", &u.url);
        return NGX_CONF_ERROR;
    }

    // 解析完毕，填充server信息
    us->name = u.url;
    us->addrs = u.addrs;
    us->naddrs = u.naddrs;
    us->weight = weight;
    us->max_fails = max_fails;
    us->fail_timeout = fail_timeout;

    return NGX_CONF_OK;

invalid:

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                       "invalid parameter \"%V\"", &value[i]);

    return NGX_CONF_ERROR;

not_supported:

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                       "balancing method does not support parameter \"%V\"",
                       &value[i]);

    return NGX_CONF_ERROR;
}


// 创建或者获取一个upstream{}块的配置信息
// 获取时flags==0
// 检查是否有同名的upstream{}，如果是创建时有则报错
// 加入main conf里的upstreams数组，之后就可以在这里找到所有的upstream{}
ngx_stream_upstream_srv_conf_t *
ngx_stream_upstream_add(ngx_conf_t *cf, ngx_url_t *u, ngx_uint_t flags)
{
    ngx_uint_t                        i;
    ngx_stream_upstream_server_t     *us;
    ngx_stream_upstream_srv_conf_t   *uscf, **uscfp;
    ngx_stream_upstream_main_conf_t  *umcf;

    if (!(flags & NGX_STREAM_UPSTREAM_CREATE)) {

        if (ngx_parse_url(cf->pool, u) != NGX_OK) {
            if (u->err) {
                ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                                   "%s in upstream \"%V\"", u->err, &u->url);
            }

            return NULL;
        }
    }

    // 获取main 配置，里面有数组
    umcf = ngx_stream_conf_get_module_main_conf(cf, ngx_stream_upstream_module);

    // 数组存储所有的upstream{}配置
    uscfp = umcf->upstreams.elts;

    // 检查是否有同名的upstream{}，如有则报错
    for (i = 0; i < umcf->upstreams.nelts; i++) {

        // 比较名字
        if (uscfp[i]->host.len != u->host.len
            || ngx_strncasecmp(uscfp[i]->host.data, u->host.data, u->host.len)
               != 0)
        {
            continue;
        }

        if ((flags & NGX_STREAM_UPSTREAM_CREATE)
             && (uscfp[i]->flags & NGX_STREAM_UPSTREAM_CREATE))
        {
            ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                               "duplicate upstream \"%V\"", &u->host);
            return NULL;
        }

        if ((uscfp[i]->flags & NGX_STREAM_UPSTREAM_CREATE) && !u->no_port) {
            ngx_conf_log_error(NGX_LOG_WARN, cf, 0,
                               "upstream \"%V\" may not have port %d",
                               &u->host, u->port);
            return NULL;
        }

        if ((flags & NGX_STREAM_UPSTREAM_CREATE) && !uscfp[i]->no_port) {
            ngx_log_error(NGX_LOG_WARN, cf->log, 0,
                          "upstream \"%V\" may not have port %d in %s:%ui",
                          &u->host, uscfp[i]->port,
                          uscfp[i]->file_name, uscfp[i]->line);
            return NULL;
        }

        if (uscfp[i]->port != u->port) {
            continue;
        }

        if (flags & NGX_STREAM_UPSTREAM_CREATE) {
            uscfp[i]->flags = flags;
        }

        // 返回找到的匹配upstream配置
        return uscfp[i];
    }

    // 创建此upstream{}的srv配置结构体
    uscf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_upstream_srv_conf_t));
    if (uscf == NULL) {
        return NULL;
    }

    // 填充基本信息
    uscf->flags = flags;
    uscf->host = u->host;
    uscf->file_name = cf->conf_file->file.name.data;
    uscf->line = cf->conf_file->line;
    uscf->port = u->port;
    uscf->no_port = u->no_port;

    // upstream指定了一个具体的上游server
    // 那么就把它加入servers数组
    if (u->naddrs == 1) {
        uscf->servers = ngx_array_create(cf->pool, 1,
                                         sizeof(ngx_stream_upstream_server_t));
        if (uscf->servers == NULL) {
            return NULL;
        }

        us = ngx_array_push(uscf->servers);
        if (us == NULL) {
            return NULL;
        }

        ngx_memzero(us, sizeof(ngx_stream_upstream_server_t));

        us->addrs = u->addrs;
        us->naddrs = 1;
    }

    // 加入main conf里的upstreams数组，之后就可以在这里找到所有的upstream{}
    uscfp = ngx_array_push(&umcf->upstreams);
    if (uscfp == NULL) {
        return NULL;
    }

    *uscfp = uscf;

    return uscf;
}


// upstream{}的配置结构体，只有一个数组
// 用来存储每个上游upstream{}的信息
static void *
ngx_stream_upstream_create_main_conf(ngx_conf_t *cf)
{
    ngx_stream_upstream_main_conf_t  *umcf;

    umcf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_upstream_main_conf_t));
    if (umcf == NULL) {
        return NULL;
    }

    if (ngx_array_init(&umcf->upstreams, cf->pool, 4,
                       sizeof(ngx_stream_upstream_srv_conf_t *))
        != NGX_OK)
    {
        return NULL;
    }

    return umcf;
}


// 在ngx_stream_block解析完配置后调用，初始化main conf
// 检查每一个upstream{}块
// 查看是否有load balance初始化指针
// 没有指定特别的算法就使用默认的round robin
// 对本upstream{}块做初始化
static char *
ngx_stream_upstream_init_main_conf(ngx_conf_t *cf, void *conf)
{
    ngx_stream_upstream_main_conf_t *umcf = conf;

    ngx_uint_t                        i;
    ngx_stream_upstream_init_pt       init;
    ngx_stream_upstream_srv_conf_t  **uscfp;

    // umcf里存储的是ngx_stream_upstream_srv_conf_t数组
    uscfp = umcf->upstreams.elts;

    // 检查每一个upstream{}块
    // 此时在uscfp里已经存储了多个上游server的信息
    // 包括地址、权重、失败次数等
    for (i = 0; i < umcf->upstreams.nelts; i++) {

        // 查看是否有load balance初始化指针
        // 没有指定特别的算法就使用默认的round robin
        init = uscfp[i]->peer.init_upstream
                                         ? uscfp[i]->peer.init_upstream
                                         : ngx_stream_upstream_init_round_robin;

        // 对本upstream{}块做初始化
        if (init(cf, uscfp[i]) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    }

    return NGX_CONF_OK;
}
