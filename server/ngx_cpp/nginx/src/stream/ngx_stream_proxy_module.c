// annotated by chrono since 2016
//
// * ngx_stream_proxy_pass
// * ngx_stream_proxy_handler
// * ngx_stream_proxy_connect_handler
// * ngx_stream_proxy_init_upstream

/*
 * Copyright (C) Roman Arutyunyan
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>


typedef void (*ngx_stream_proxy_handler_pt)(ngx_stream_session_t *s);


typedef struct {
    ngx_msec_t                       connect_timeout;
    ngx_msec_t                       timeout;
    ngx_msec_t                       next_upstream_timeout;
    size_t                           buffer_size;
    size_t                           upload_rate;
    size_t                           download_rate;
    ngx_uint_t                       next_upstream_tries;
    ngx_flag_t                       next_upstream;
    ngx_flag_t                       proxy_protocol;
    ngx_addr_t                      *local;

#if (NGX_STREAM_SSL)
    ngx_flag_t                       ssl_enable;
    ngx_flag_t                       ssl_session_reuse;
    ngx_uint_t                       ssl_protocols;
    ngx_str_t                        ssl_ciphers;
    ngx_str_t                        ssl_name;
    ngx_flag_t                       ssl_server_name;

    ngx_flag_t                       ssl_verify;
    ngx_uint_t                       ssl_verify_depth;
    ngx_str_t                        ssl_trusted_certificate;
    ngx_str_t                        ssl_crl;
    ngx_str_t                        ssl_certificate;
    ngx_str_t                        ssl_certificate_key;
    ngx_array_t                     *ssl_passwords;

    ngx_ssl_t                       *ssl;
#endif

    // 将要转发的上游服务器集群
    // 定义为一个upstream{}
    ngx_stream_upstream_srv_conf_t  *upstream;
} ngx_stream_proxy_srv_conf_t;


// ngx_stream_init_connection->ngx_stream_init_session之后调用，处理请求
// 创建连接上游的结构体
// 里面有如何获取负载均衡server、上下游buf等
// 负载均衡算法初始化
// 准备开始连接，设置开始时间，秒数，没有毫秒
// 最后启动连接
static void ngx_stream_proxy_handler(ngx_stream_session_t *s);

// 连接上游
// 使用ngx_peer_connection_t连接上游服务器
// 连接失败，需要尝试下一个上游server
// 连接成功要调用init初始化上游
static void ngx_stream_proxy_connect(ngx_stream_session_t *s);

static void ngx_stream_proxy_init_upstream(ngx_stream_session_t *s);

// 连接成功之后的事件处理函数
// 实际是ngx_stream_proxy_process_connection(ev, !ev->write);
static void ngx_stream_proxy_upstream_handler(ngx_event_t *ev);

static void ngx_stream_proxy_downstream_handler(ngx_event_t *ev);
static void ngx_stream_proxy_process_connection(ngx_event_t *ev,
    ngx_uint_t from_upstream);

// 第一次连接上游不成功后的handler
// 当上游连接再次有读写事件发生时测试连接
// 测试连接是否成功，失败就再试下一个上游
// 最后还是要调用init初始化上游
static void ngx_stream_proxy_connect_handler(ngx_event_t *ev);

// 测试连接是否成功，失败就再试下一个上游
static ngx_int_t ngx_stream_proxy_test_connect(ngx_connection_t *c);

static ngx_int_t ngx_stream_proxy_process(ngx_stream_session_t *s,
    ngx_uint_t from_upstream, ngx_uint_t do_write);
static void ngx_stream_proxy_next_upstream(ngx_stream_session_t *s);
static void ngx_stream_proxy_finalize(ngx_stream_session_t *s, ngx_int_t rc);
static u_char *ngx_stream_proxy_log_error(ngx_log_t *log, u_char *buf,
    size_t len);

static void *ngx_stream_proxy_create_srv_conf(ngx_conf_t *cf);
static char *ngx_stream_proxy_merge_srv_conf(ngx_conf_t *cf, void *parent,
    void *child);

// 解析proxy_pass指令，设置处理handler，在init建立连接之后会调用
// 获取一个upstream{}块的配置信息
static char *ngx_stream_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

static char *ngx_stream_proxy_bind(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_stream_proxy_send_proxy_protocol(ngx_stream_session_t *s);

#if (NGX_STREAM_SSL)

static char *ngx_stream_proxy_ssl_password_file(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static void ngx_stream_proxy_ssl_init_connection(ngx_stream_session_t *s);
static void ngx_stream_proxy_ssl_handshake(ngx_connection_t *pc);
static ngx_int_t ngx_stream_proxy_ssl_name(ngx_stream_session_t *s);
static ngx_int_t ngx_stream_proxy_set_ssl(ngx_conf_t *cf,
    ngx_stream_proxy_srv_conf_t *pscf);


static ngx_conf_bitmask_t  ngx_stream_proxy_ssl_protocols[] = {
    { ngx_string("SSLv2"), NGX_SSL_SSLv2 },
    { ngx_string("SSLv3"), NGX_SSL_SSLv3 },
    { ngx_string("TLSv1"), NGX_SSL_TLSv1 },
    { ngx_string("TLSv1.1"), NGX_SSL_TLSv1_1 },
    { ngx_string("TLSv1.2"), NGX_SSL_TLSv1_2 },
    { ngx_null_string, 0 }
};

#endif


static ngx_conf_deprecated_t  ngx_conf_deprecated_proxy_downstream_buffer = {
    ngx_conf_deprecated, "proxy_downstream_buffer", "proxy_buffer_size"
};

static ngx_conf_deprecated_t  ngx_conf_deprecated_proxy_upstream_buffer = {
    ngx_conf_deprecated, "proxy_upstream_buffer", "proxy_buffer_size"
};


static ngx_command_t  ngx_stream_proxy_commands[] = {

    { ngx_string("proxy_pass"),
      NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_stream_proxy_pass,
      NGX_STREAM_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("proxy_bind"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_stream_proxy_bind,
      NGX_STREAM_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("proxy_connect_timeout"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, connect_timeout),
      NULL },

    { ngx_string("proxy_timeout"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, timeout),
      NULL },

    { ngx_string("proxy_buffer_size"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, buffer_size),
      NULL },

    { ngx_string("proxy_downstream_buffer"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, buffer_size),
      &ngx_conf_deprecated_proxy_downstream_buffer },

    { ngx_string("proxy_upstream_buffer"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, buffer_size),
      &ngx_conf_deprecated_proxy_upstream_buffer },

    { ngx_string("proxy_upload_rate"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, upload_rate),
      NULL },

    { ngx_string("proxy_download_rate"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, download_rate),
      NULL },

    { ngx_string("proxy_next_upstream"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, next_upstream),
      NULL },

    { ngx_string("proxy_next_upstream_tries"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, next_upstream_tries),
      NULL },

    { ngx_string("proxy_next_upstream_timeout"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, next_upstream_timeout),
      NULL },

    { ngx_string("proxy_protocol"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, proxy_protocol),
      NULL },

#if (NGX_STREAM_SSL)

    { ngx_string("proxy_ssl"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_enable),
      NULL },

    { ngx_string("proxy_ssl_session_reuse"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_session_reuse),
      NULL },

    { ngx_string("proxy_ssl_protocols"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_1MORE,
      ngx_conf_set_bitmask_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_protocols),
      &ngx_stream_proxy_ssl_protocols },

    { ngx_string("proxy_ssl_ciphers"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_ciphers),
      NULL },

    { ngx_string("proxy_ssl_name"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_name),
      NULL },

    { ngx_string("proxy_ssl_server_name"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_server_name),
      NULL },

    { ngx_string("proxy_ssl_verify"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_verify),
      NULL },

    { ngx_string("proxy_ssl_verify_depth"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_verify_depth),
      NULL },

    { ngx_string("proxy_ssl_trusted_certificate"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_trusted_certificate),
      NULL },

    { ngx_string("proxy_ssl_crl"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_crl),
      NULL },

    { ngx_string("proxy_ssl_certificate"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_certificate),
      NULL },

    { ngx_string("proxy_ssl_certificate_key"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_proxy_srv_conf_t, ssl_certificate_key),
      NULL },

    { ngx_string("proxy_ssl_password_file"),
      NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_TAKE1,
      ngx_stream_proxy_ssl_password_file,
      NGX_STREAM_SRV_CONF_OFFSET,
      0,
      NULL },

#endif

      ngx_null_command
};


static ngx_stream_module_t  ngx_stream_proxy_module_ctx = {
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    ngx_stream_proxy_create_srv_conf,      /* create server configuration */
    ngx_stream_proxy_merge_srv_conf        /* merge server configuration */
};


ngx_module_t  ngx_stream_proxy_module = {
    NGX_MODULE_V1,
    &ngx_stream_proxy_module_ctx,          /* module context */
    ngx_stream_proxy_commands,             /* module directives */
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


// ngx_stream_init_connection->ngx_stream_init_session之后调用，处理请求
// 创建连接上游的结构体
// 里面有如何获取负载均衡server、上下游buf等
// 负载均衡算法初始化
// 准备开始连接，设置开始时间，秒数，没有毫秒
// 最后启动连接
static void
ngx_stream_proxy_handler(ngx_stream_session_t *s)
{
    u_char                          *p;
    ngx_connection_t                *c;
    ngx_stream_upstream_t           *u;
    ngx_stream_proxy_srv_conf_t     *pscf;
    ngx_stream_upstream_srv_conf_t  *uscf;

    // 获取连接对象
    c = s->connection;

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    ngx_log_debug0(NGX_LOG_DEBUG_STREAM, c->log, 0,
                   "proxy connection handler");

    // 创建连接上游的结构体
    // 里面有如何获取负载均衡server、上下游buf等
    u = ngx_pcalloc(c->pool, sizeof(ngx_stream_upstream_t));
    if (u == NULL) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    // 关联到会话对象
    s->upstream = u;

    s->log_handler = ngx_stream_proxy_log_error;

    u->peer.log = c->log;
    u->peer.log_error = NGX_ERROR_ERR;

    u->peer.local = pscf->local;

    // 获取上游的配置结构体
    // 在ngx_stream_proxy_pass里设置的
    uscf = pscf->upstream;

    // 负载均衡算法初始化
    if (uscf->peer.init(s, uscf) != NGX_OK) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    // 准备开始连接，设置开始时间，秒数，没有毫秒
    u->peer.start_time = ngx_current_msec;

    // 设置负载均衡的重试次数
    if (pscf->next_upstream_tries
        && u->peer.tries > pscf->next_upstream_tries)
    {
        u->peer.tries = pscf->next_upstream_tries;
    }

    u->proxy_protocol = pscf->proxy_protocol;

    // 准备开始连接，设置开始时间，秒数，没有毫秒
    u->start_sec = ngx_time();

    // 缓冲区，给下游用
    p = ngx_pnalloc(c->pool, pscf->buffer_size);
    if (p == NULL) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    u->downstream_buf.start = p;
    u->downstream_buf.end = p + pscf->buffer_size;
    u->downstream_buf.pos = p;
    u->downstream_buf.last = p;

    // 连接的读写事件都设置为ngx_stream_proxy_downstream_handler
    // 注意这个连接是客户端发起的连接，即下游
    c->write->handler = ngx_stream_proxy_downstream_handler;
    c->read->handler = ngx_stream_proxy_downstream_handler;

    if (u->proxy_protocol
#if (NGX_STREAM_SSL)
        && pscf->ssl == NULL
#endif
        && pscf->buffer_size >= NGX_PROXY_PROTOCOL_MAX_HEADER)
    {
        /* optimization for a typical case */

        ngx_log_debug0(NGX_LOG_DEBUG_STREAM, c->log, 0,
                       "stream proxy send PROXY protocol header");

        p = ngx_proxy_protocol_write(c, u->downstream_buf.last,
                                     u->downstream_buf.end);
        if (p == NULL) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
            return;
        }

        u->downstream_buf.last = p;
        u->proxy_protocol = 0;
    }

    if (ngx_stream_proxy_process(s, 0, 0) != NGX_OK) {
        return;
    }

    // 最后启动连接
    // 使用ngx_peer_connection_t连接上游服务器
    // 连接失败，需要尝试下一个上游server
    // 连接成功要调用init初始化上游
    ngx_stream_proxy_connect(s);
}


// 连接上游
// 使用ngx_peer_connection_t连接上游服务器
// 连接失败，需要尝试下一个上游server
// 连接成功要调用init初始化上游
static void
ngx_stream_proxy_connect(ngx_stream_session_t *s)
{
    ngx_int_t                     rc;
    ngx_connection_t             *c, *pc;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    // 获取连接对象
    c = s->connection;

    c->log->action = "connecting to upstream";

    // 连接上游的结构体
    // 里面有如何获取负载均衡server、上下游buf等
    u = s->upstream;

    // 连接上游
    // 使用ngx_peer_connection_t连接上游服务器
    // 从upstream{}里获取一个上游server地址
    // 从cycle的连接池获取一个空闲连接
    // 设置连接的数据收发接口函数
    // 向epoll添加连接，即同时添加读写事件
    // 当与上游服务器有任何数据收发时都会触发epoll
    // socket api调用连接上游服务器
    // 写事件ready，即可以立即向上游发送数据
    rc = ngx_event_connect_peer(&u->peer);

    ngx_log_debug1(NGX_LOG_DEBUG_STREAM, c->log, 0, "proxy connect: %i", rc);

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    if (rc == NGX_ERROR) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    if (rc == NGX_BUSY) {
        ngx_log_error(NGX_LOG_ERR, c->log, 0, "no live upstreams");
        ngx_stream_proxy_finalize(s, NGX_DECLINED);
        return;
    }

    // 连接失败，需要尝试下一个上游server
    if (rc == NGX_DECLINED) {
        ngx_stream_proxy_next_upstream(s);
        return;
    }

    /* rc == NGX_OK || rc == NGX_AGAIN || rc == NGX_DONE */

    pc = u->peer.connection;

    pc->data = s;
    pc->log = c->log;
    pc->pool = c->pool;
    pc->read->log = c->log;
    pc->write->log = c->log;

    // 连接成功
    if (rc != NGX_AGAIN) {
        ngx_stream_proxy_init_upstream(s);
        return;
    }

    // again，要再次尝试连接

    // 设置上游的读写事件处理函数是ngx_stream_proxy_connect_handler
    // 第一次连接上游不成功后的handler
    // 当上游连接再次有读写事件发生时测试连接
    // 测试连接是否成功，失败就再试下一个上游
    // 最后还是要调用init初始化上游
    pc->read->handler = ngx_stream_proxy_connect_handler;
    pc->write->handler = ngx_stream_proxy_connect_handler;

    // 连接上游先写，所以设置写事件的超时时间
    ngx_add_timer(pc->write, pscf->connect_timeout);
}


// 分配供上游读取数据的缓冲区
// 进入此函数，肯定已经成功连接了上游服务器
// 修改上游读写事件，不再测试连接，改为ngx_stream_proxy_upstream_handler
// 实际是ngx_stream_proxy_process_connection(ev, !ev->write);
static void
ngx_stream_proxy_init_upstream(ngx_stream_session_t *s)
{
    int                           tcp_nodelay;
    u_char                       *p;
    ngx_connection_t             *c, *pc;
    ngx_log_handler_pt            handler;
    ngx_stream_upstream_t        *u;
    ngx_stream_core_srv_conf_t   *cscf;
    ngx_stream_proxy_srv_conf_t  *pscf;

    // u保存了上游相关的信息
    u = s->upstream;

    // pc是上游的连接对象
    pc = u->peer.connection;

    cscf = ngx_stream_get_module_srv_conf(s, ngx_stream_core_module);

    if (cscf->tcp_nodelay && pc->tcp_nodelay == NGX_TCP_NODELAY_UNSET) {
        ngx_log_debug0(NGX_LOG_DEBUG_STREAM, pc->log, 0, "tcp_nodelay");

        tcp_nodelay = 1;

        if (setsockopt(pc->fd, IPPROTO_TCP, TCP_NODELAY,
                       (const void *) &tcp_nodelay, sizeof(int)) == -1)
        {
            ngx_connection_error(pc, ngx_socket_errno,
                                 "setsockopt(TCP_NODELAY) failed");
            ngx_stream_proxy_next_upstream(s);
            return;
        }

        pc->tcp_nodelay = NGX_TCP_NODELAY_SET;
    }

    if (u->proxy_protocol) {
        if (ngx_stream_proxy_send_proxy_protocol(s) != NGX_OK) {
            return;
        }

        u->proxy_protocol = 0;
    }

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

#if (NGX_STREAM_SSL)
    if (pscf->ssl && pc->ssl == NULL) {
        ngx_stream_proxy_ssl_init_connection(s);
        return;
    }
#endif

    // c是到客户端即下游的连接对象
    c = s->connection;

    if (c->log->log_level >= NGX_LOG_INFO) {
        ngx_str_t  str;
        u_char     addr[NGX_SOCKADDR_STRLEN];

        str.len = NGX_SOCKADDR_STRLEN;
        str.data = addr;

        if (ngx_connection_local_sockaddr(pc, &str, 1) == NGX_OK) {
            handler = c->log->handler;
            c->log->handler = NULL;

            ngx_log_error(NGX_LOG_INFO, c->log, 0, "proxy %V connected to %V",
                          &str, u->peer.name);

            c->log->handler = handler;
        }
    }

    c->log->action = "proxying connection";

    // 分配供上游读取数据的缓冲区
    p = ngx_pnalloc(c->pool, pscf->buffer_size);
    if (p == NULL) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    u->upstream_buf.start = p;
    u->upstream_buf.end = p + pscf->buffer_size;
    u->upstream_buf.pos = p;
    u->upstream_buf.last = p;

    // 进入此函数，肯定已经成功连接了上游服务器
    u->connected = 1;

    // 修改上游读写事件，不再测试连接，改为ngx_stream_proxy_upstream_handler
    // 实际是ngx_stream_proxy_process_connection(ev, !ev->write);
    pc->read->handler = ngx_stream_proxy_upstream_handler;
    pc->write->handler = ngx_stream_proxy_upstream_handler;

    if (ngx_stream_proxy_process(s, 1, 0) != NGX_OK) {
        return;
    }

    ngx_stream_proxy_process(s, 0, 1);
}


static ngx_int_t
ngx_stream_proxy_send_proxy_protocol(ngx_stream_session_t *s)
{
    u_char                       *p;
    ssize_t                       n, size;
    ngx_connection_t             *c, *pc;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;
    u_char                        buf[NGX_PROXY_PROTOCOL_MAX_HEADER];

    c = s->connection;

    ngx_log_debug0(NGX_LOG_DEBUG_STREAM, c->log, 0,
                   "stream proxy send PROXY protocol header");

    p = ngx_proxy_protocol_write(c, buf, buf + NGX_PROXY_PROTOCOL_MAX_HEADER);
    if (p == NULL) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return NGX_ERROR;
    }

    u = s->upstream;

    pc = u->peer.connection;

    size = p - buf;

    n = pc->send(pc, buf, size);

    if (n == NGX_AGAIN) {
        if (ngx_handle_write_event(pc->write, 0) != NGX_OK) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
            return NGX_ERROR;
        }

        pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

        ngx_add_timer(pc->write, pscf->timeout);

        pc->write->handler = ngx_stream_proxy_connect_handler;

        return NGX_AGAIN;
    }

    if (n == NGX_ERROR) {
        ngx_stream_proxy_finalize(s, NGX_DECLINED);
        return NGX_ERROR;
    }

    if (n != size) {

        /*
         * PROXY protocol specification:
         * The sender must always ensure that the header
         * is sent at once, so that the transport layer
         * maintains atomicity along the path to the receiver.
         */

        ngx_log_error(NGX_LOG_ERR, c->log, 0,
                      "could not send PROXY protocol header at once");

        ngx_stream_proxy_finalize(s, NGX_DECLINED);

        return NGX_ERROR;
    }

    return NGX_OK;
}


#if (NGX_STREAM_SSL)

static char *
ngx_stream_proxy_ssl_password_file(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf)
{
    ngx_stream_proxy_srv_conf_t *pscf = conf;

    ngx_str_t  *value;

    if (pscf->ssl_passwords != NGX_CONF_UNSET_PTR) {
        return "is duplicate";
    }

    value = cf->args->elts;

    pscf->ssl_passwords = ngx_ssl_read_password_file(cf, &value[1]);

    if (pscf->ssl_passwords == NULL) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}


static void
ngx_stream_proxy_ssl_init_connection(ngx_stream_session_t *s)
{
    ngx_int_t                     rc;
    ngx_connection_t             *pc;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    u = s->upstream;

    pc = u->peer.connection;

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    if (ngx_ssl_create_connection(pscf->ssl, pc, NGX_SSL_BUFFER|NGX_SSL_CLIENT)
        != NGX_OK)
    {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return;
    }

    if (pscf->ssl_server_name || pscf->ssl_verify) {
        if (ngx_stream_proxy_ssl_name(s) != NGX_OK) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
            return;
        }
    }

    if (pscf->ssl_session_reuse) {
        if (u->peer.set_session(&u->peer, u->peer.data) != NGX_OK) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
            return;
        }
    }

    s->connection->log->action = "SSL handshaking to upstream";

    rc = ngx_ssl_handshake(pc);

    if (rc == NGX_AGAIN) {

        if (!pc->write->timer_set) {
            ngx_add_timer(pc->write, pscf->connect_timeout);
        }

        pc->ssl->handler = ngx_stream_proxy_ssl_handshake;
        return;
    }

    ngx_stream_proxy_ssl_handshake(pc);
}


static void
ngx_stream_proxy_ssl_handshake(ngx_connection_t *pc)
{
    long                          rc;
    ngx_stream_session_t         *s;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    s = pc->data;

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    if (pc->ssl->handshaked) {

        if (pscf->ssl_verify) {
            rc = SSL_get_verify_result(pc->ssl->connection);

            if (rc != X509_V_OK) {
                ngx_log_error(NGX_LOG_ERR, pc->log, 0,
                              "upstream SSL certificate verify error: (%l:%s)",
                              rc, X509_verify_cert_error_string(rc));
                goto failed;
            }

            u = s->upstream;

            if (ngx_ssl_check_host(pc, &u->ssl_name) != NGX_OK) {
                ngx_log_error(NGX_LOG_ERR, pc->log, 0,
                              "upstream SSL certificate does not match \"%V\"",
                              &u->ssl_name);
                goto failed;
            }
        }

        if (pscf->ssl_session_reuse) {
            u = s->upstream;
            u->peer.save_session(&u->peer, u->peer.data);
        }

        if (pc->write->timer_set) {
            ngx_del_timer(pc->write);
        }

        ngx_stream_proxy_init_upstream(s);

        return;
    }

failed:

    ngx_stream_proxy_next_upstream(s);
}


static ngx_int_t
ngx_stream_proxy_ssl_name(ngx_stream_session_t *s)
{
    u_char                       *p, *last;
    ngx_str_t                     name;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    u = s->upstream;

    name = pscf->ssl_name;

    if (name.len == 0) {
        name = pscf->upstream->host;
    }

    if (name.len == 0) {
        goto done;
    }

    /*
     * ssl name here may contain port, strip it for compatibility
     * with the http module
     */

    p = name.data;
    last = name.data + name.len;

    if (*p == '[') {
        p = ngx_strlchr(p, last, ']');

        if (p == NULL) {
            p = name.data;
        }
    }

    p = ngx_strlchr(p, last, ':');

    if (p != NULL) {
        name.len = p - name.data;
    }

    if (!pscf->ssl_server_name) {
        goto done;
    }

#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME

    /* as per RFC 6066, literal IPv4 and IPv6 addresses are not permitted */

    if (name.len == 0 || *name.data == '[') {
        goto done;
    }

    if (ngx_inet_addr(name.data, name.len) != INADDR_NONE) {
        goto done;
    }

    /*
     * SSL_set_tlsext_host_name() needs a null-terminated string,
     * hence we explicitly null-terminate name here
     */

    p = ngx_pnalloc(s->connection->pool, name.len + 1);
    if (p == NULL) {
        return NGX_ERROR;
    }

    (void) ngx_cpystrn(p, name.data, name.len + 1);

    name.data = p;

    ngx_log_debug1(NGX_LOG_DEBUG_STREAM, s->connection->log, 0,
                   "upstream SSL server name: \"%s\"", name.data);

    if (SSL_set_tlsext_host_name(u->peer.connection->ssl->connection, name.data)
        == 0)
    {
        ngx_ssl_error(NGX_LOG_ERR, s->connection->log, 0,
                      "SSL_set_tlsext_host_name(\"%s\") failed", name.data);
        return NGX_ERROR;
    }

#endif

done:

    u->ssl_name = name;

    return NGX_OK;
}

#endif


static void
ngx_stream_proxy_downstream_handler(ngx_event_t *ev)
{
    ngx_stream_proxy_process_connection(ev, ev->write);
}


// 连接成功之后的事件处理函数
// 实际是ngx_stream_proxy_process_connection(ev, !ev->write);
static void
ngx_stream_proxy_upstream_handler(ngx_event_t *ev)
{
    ngx_stream_proxy_process_connection(ev, !ev->write);
}


static void
ngx_stream_proxy_process_connection(ngx_event_t *ev, ngx_uint_t from_upstream)
{
    ngx_connection_t             *c, *pc;
    ngx_stream_session_t         *s;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    // 连接、会话、上游
    c = ev->data;
    s = c->data;
    u = s->upstream;

    // 超时处理，没有delay则失败
    if (ev->timedout) {

        if (ev->delayed) {

            ev->timedout = 0;
            ev->delayed = 0;

            if (!ev->ready) {
                if (ngx_handle_read_event(ev, 0) != NGX_OK) {
                    ngx_stream_proxy_finalize(s, NGX_ERROR);
                    return;
                }

                if (u->connected) {
                    pc = u->peer.connection;

                    if (!c->read->delayed && !pc->read->delayed) {
                        pscf = ngx_stream_get_module_srv_conf(s,
                                                       ngx_stream_proxy_module);
                        ngx_add_timer(c->write, pscf->timeout);
                    }
                }

                return;
            }

        } else {
            ngx_connection_error(c, NGX_ETIMEDOUT, "connection timed out");
            ngx_stream_proxy_finalize(s, NGX_DECLINED);
            return;
        }

    } else if (ev->delayed) {

        ngx_log_debug0(NGX_LOG_DEBUG_STREAM, c->log, 0,
                       "stream connection delayed");

        if (ngx_handle_read_event(ev, 0) != NGX_OK) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
        }

        return;
    }

    if (from_upstream && !u->connected) {
        return;
    }

    ngx_stream_proxy_process(s, from_upstream, ev->write);
}


// 第一次连接上游不成功后的handler
// 当上游连接再次有读写事件发生时测试连接
// 测试连接是否成功，失败就再试下一个上游
// 最后还是要调用init初始化上游
static void
ngx_stream_proxy_connect_handler(ngx_event_t *ev)
{
    ngx_connection_t      *c;
    ngx_stream_session_t  *s;

    // 连接、会话对象
    c = ev->data;
    s = c->data;

    // 超时就尝试下一个上游
    if (ev->timedout) {
        ngx_log_error(NGX_LOG_ERR, c->log, NGX_ETIMEDOUT, "upstream timed out");
        ngx_stream_proxy_next_upstream(s);
        return;
    }

    ngx_del_timer(c->write);

    ngx_log_debug0(NGX_LOG_DEBUG_STREAM, c->log, 0,
                   "stream proxy connect upstream");

    // 测试连接是否成功，失败就再试下一个上游
    if (ngx_stream_proxy_test_connect(c) != NGX_OK) {
        ngx_stream_proxy_next_upstream(s);
        return;
    }

    // 最后还是要调用init初始化上游
    ngx_stream_proxy_init_upstream(s);
}


// 测试连接是否成功，失败就再试下一个上游
static ngx_int_t
ngx_stream_proxy_test_connect(ngx_connection_t *c)
{
    int        err;
    socklen_t  len;

#if (NGX_HAVE_KQUEUE)

    if (ngx_event_flags & NGX_USE_KQUEUE_EVENT)  {
        err = c->write->kq_errno ? c->write->kq_errno : c->read->kq_errno;

        if (err) {
            (void) ngx_connection_error(c, err,
                                    "kevent() reported that connect() failed");
            return NGX_ERROR;
        }

    } else
#endif
    {
        err = 0;
        len = sizeof(int);

        /*
         * BSDs and Linux return 0 and set a pending error in err
         * Solaris returns -1 and sets errno
         */

        if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, (void *) &err, &len)
            == -1)
        {
            err = ngx_socket_errno;
        }

        if (err) {
            (void) ngx_connection_error(c, err, "connect() failed");
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}


static ngx_int_t
ngx_stream_proxy_process(ngx_stream_session_t *s, ngx_uint_t from_upstream,
    ngx_uint_t do_write)
{
    off_t                        *received, limit;
    size_t                        size, limit_rate;
    ssize_t                       n;
    ngx_buf_t                    *b;
    ngx_uint_t                    flags;
    ngx_msec_t                    delay;
    ngx_connection_t             *c, *pc, *src, *dst;
    ngx_log_handler_pt            handler;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    u = s->upstream;

    c = s->connection;
    pc = u->connected ? u->peer.connection : NULL;

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    if (from_upstream) {
        src = pc;
        dst = c;
        b = &u->upstream_buf;
        limit_rate = pscf->download_rate;
        received = &u->received;

    } else {
        src = c;
        dst = pc;
        b = &u->downstream_buf;
        limit_rate = pscf->upload_rate;
        received = &s->received;
    }

    for ( ;; ) {

        if (do_write) {

            size = b->last - b->pos;

            if (size && dst && dst->write->ready) {

                n = dst->send(dst, b->pos, size);

                if (n == NGX_ERROR) {
                    ngx_stream_proxy_finalize(s, NGX_DECLINED);
                    return NGX_ERROR;
                }

                if (n > 0) {
                    b->pos += n;

                    if (b->pos == b->last) {
                        b->pos = b->start;
                        b->last = b->start;
                    }
                }
            }
        }

        size = b->end - b->last;

        if (size && src->read->ready && !src->read->delayed) {

            if (limit_rate) {
                limit = (off_t) limit_rate * (ngx_time() - u->start_sec + 1)
                        - *received;

                if (limit <= 0) {
                    src->read->delayed = 1;
                    delay = (ngx_msec_t) (- limit * 1000 / limit_rate + 1);
                    ngx_add_timer(src->read, delay);
                    break;
                }

                if ((off_t) size > limit) {
                    size = (size_t) limit;
                }
            }

            n = src->recv(src, b->last, size);

            if (n == NGX_AGAIN || n == 0) {
                break;
            }

            if (n > 0) {
                if (limit_rate) {
                    delay = (ngx_msec_t) (n * 1000 / limit_rate);

                    if (delay > 0) {
                        src->read->delayed = 1;
                        ngx_add_timer(src->read, delay);
                    }
                }

                *received += n;
                b->last += n;
                do_write = 1;

                continue;
            }

            if (n == NGX_ERROR) {
                src->read->eof = 1;
            }
        }

        break;
    }

    if (src->read->eof && (b->pos == b->last || (dst && dst->read->eof))) {
        handler = c->log->handler;
        c->log->handler = NULL;

        ngx_log_error(NGX_LOG_INFO, c->log, 0,
                      "%s disconnected"
                      ", bytes from/to client:%O/%O"
                      ", bytes from/to upstream:%O/%O",
                      from_upstream ? "upstream" : "client",
                      s->received, c->sent, u->received, pc ? pc->sent : 0);

        c->log->handler = handler;

        ngx_stream_proxy_finalize(s, NGX_OK);
        return NGX_DONE;
    }

    flags = src->read->eof ? NGX_CLOSE_EVENT : 0;

    if (ngx_handle_read_event(src->read, flags) != NGX_OK) {
        ngx_stream_proxy_finalize(s, NGX_ERROR);
        return NGX_ERROR;
    }

    if (dst) {
        if (ngx_handle_write_event(dst->write, 0) != NGX_OK) {
            ngx_stream_proxy_finalize(s, NGX_ERROR);
            return NGX_ERROR;
        }

        if (!c->read->delayed && !pc->read->delayed) {
            ngx_add_timer(c->write, pscf->timeout);

        } else if (c->write->timer_set) {
            ngx_del_timer(c->write);
        }
    }

    return NGX_OK;
}


static void
ngx_stream_proxy_next_upstream(ngx_stream_session_t *s)
{
    ngx_msec_t                    timeout;
    ngx_connection_t             *pc;
    ngx_stream_upstream_t        *u;
    ngx_stream_proxy_srv_conf_t  *pscf;

    ngx_log_debug0(NGX_LOG_DEBUG_STREAM, s->connection->log, 0,
                   "stream proxy next upstream");

    u = s->upstream;

    if (u->peer.sockaddr) {
        u->peer.free(&u->peer, u->peer.data, NGX_PEER_FAILED);
        u->peer.sockaddr = NULL;
    }

    pscf = ngx_stream_get_module_srv_conf(s, ngx_stream_proxy_module);

    timeout = pscf->next_upstream_timeout;

    if (u->peer.tries == 0
        || !pscf->next_upstream
        || (timeout && ngx_current_msec - u->peer.start_time >= timeout))
    {
        ngx_stream_proxy_finalize(s, NGX_DECLINED);
        return;
    }

    pc = u->peer.connection;

    if (pc) {
        ngx_log_debug1(NGX_LOG_DEBUG_STREAM, s->connection->log, 0,
                       "close proxy upstream connection: %d", pc->fd);

#if (NGX_STREAM_SSL)
        if (pc->ssl) {
            pc->ssl->no_wait_shutdown = 1;
            pc->ssl->no_send_shutdown = 1;

            (void) ngx_ssl_shutdown(pc);
        }
#endif

        ngx_close_connection(pc);
        u->peer.connection = NULL;
    }

    ngx_stream_proxy_connect(s);
}


static void
ngx_stream_proxy_finalize(ngx_stream_session_t *s, ngx_int_t rc)
{
    ngx_connection_t       *pc;
    ngx_stream_upstream_t  *u;

    ngx_log_debug1(NGX_LOG_DEBUG_STREAM, s->connection->log, 0,
                   "finalize stream proxy: %i", rc);

    u = s->upstream;

    if (u == NULL) {
        goto noupstream;
    }

    if (u->peer.free && u->peer.sockaddr) {
        u->peer.free(&u->peer, u->peer.data, 0);
        u->peer.sockaddr = NULL;
    }

    pc = u->peer.connection;

    if (pc) {
        ngx_log_debug1(NGX_LOG_DEBUG_STREAM, s->connection->log, 0,
                       "close stream proxy upstream connection: %d", pc->fd);

#if (NGX_STREAM_SSL)
        if (pc->ssl) {
            pc->ssl->no_wait_shutdown = 1;
            (void) ngx_ssl_shutdown(pc);
        }
#endif

        ngx_close_connection(pc);
        u->peer.connection = NULL;
    }

noupstream:

    ngx_stream_close_connection(s->connection);
}


static u_char *
ngx_stream_proxy_log_error(ngx_log_t *log, u_char *buf, size_t len)
{
    u_char                 *p;
    ngx_connection_t       *pc;
    ngx_stream_session_t   *s;
    ngx_stream_upstream_t  *u;

    s = log->data;

    u = s->upstream;

    p = buf;

    if (u->peer.name) {
        p = ngx_snprintf(p, len, ", upstream: \"%V\"", u->peer.name);
        len -= p - buf;
    }

    pc = u->peer.connection;

    p = ngx_snprintf(p, len,
                     ", bytes from/to client:%O/%O"
                     ", bytes from/to upstream:%O/%O",
                     s->received, s->connection->sent,
                     u->received, pc ? pc->sent : 0);

    return p;
}


static void *
ngx_stream_proxy_create_srv_conf(ngx_conf_t *cf)
{
    ngx_stream_proxy_srv_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_proxy_srv_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     conf->ssl_protocols = 0;
     *     conf->ssl_ciphers = { 0, NULL };
     *     conf->ssl_name = { 0, NULL };
     *     conf->ssl_trusted_certificate = { 0, NULL };
     *     conf->ssl_crl = { 0, NULL };
     *     conf->ssl_certificate = { 0, NULL };
     *     conf->ssl_certificate_key = { 0, NULL };
     *
     *     conf->ssl = NULL;
     *     conf->upstream = NULL;
     */

    conf->connect_timeout = NGX_CONF_UNSET_MSEC;
    conf->timeout = NGX_CONF_UNSET_MSEC;
    conf->next_upstream_timeout = NGX_CONF_UNSET_MSEC;
    conf->buffer_size = NGX_CONF_UNSET_SIZE;
    conf->upload_rate = NGX_CONF_UNSET_SIZE;
    conf->download_rate = NGX_CONF_UNSET_SIZE;
    conf->next_upstream_tries = NGX_CONF_UNSET_UINT;
    conf->next_upstream = NGX_CONF_UNSET;
    conf->proxy_protocol = NGX_CONF_UNSET;
    conf->local = NGX_CONF_UNSET_PTR;

#if (NGX_STREAM_SSL)
    conf->ssl_enable = NGX_CONF_UNSET;
    conf->ssl_session_reuse = NGX_CONF_UNSET;
    conf->ssl_server_name = NGX_CONF_UNSET;
    conf->ssl_verify = NGX_CONF_UNSET;
    conf->ssl_verify_depth = NGX_CONF_UNSET_UINT;
    conf->ssl_passwords = NGX_CONF_UNSET_PTR;
#endif

    return conf;
}


static char *
ngx_stream_proxy_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_stream_proxy_srv_conf_t *prev = parent;
    ngx_stream_proxy_srv_conf_t *conf = child;

    ngx_conf_merge_msec_value(conf->connect_timeout,
                              prev->connect_timeout, 60000);

    ngx_conf_merge_msec_value(conf->timeout,
                              prev->timeout, 10 * 60000);

    ngx_conf_merge_msec_value(conf->next_upstream_timeout,
                              prev->next_upstream_timeout, 0);

    ngx_conf_merge_size_value(conf->buffer_size,
                              prev->buffer_size, 16384);

    ngx_conf_merge_size_value(conf->upload_rate,
                              prev->upload_rate, 0);

    ngx_conf_merge_size_value(conf->download_rate,
                              prev->download_rate, 0);

    ngx_conf_merge_uint_value(conf->next_upstream_tries,
                              prev->next_upstream_tries, 0);

    ngx_conf_merge_value(conf->next_upstream, prev->next_upstream, 1);

    ngx_conf_merge_value(conf->proxy_protocol, prev->proxy_protocol, 0);

    ngx_conf_merge_ptr_value(conf->local, prev->local, NULL);

#if (NGX_STREAM_SSL)

    ngx_conf_merge_value(conf->ssl_enable, prev->ssl_enable, 0);

    ngx_conf_merge_value(conf->ssl_session_reuse,
                              prev->ssl_session_reuse, 1);

    ngx_conf_merge_bitmask_value(conf->ssl_protocols, prev->ssl_protocols,
                              (NGX_CONF_BITMASK_SET|NGX_SSL_TLSv1
                               |NGX_SSL_TLSv1_1|NGX_SSL_TLSv1_2));

    ngx_conf_merge_str_value(conf->ssl_ciphers, prev->ssl_ciphers, "DEFAULT");

    ngx_conf_merge_str_value(conf->ssl_name, prev->ssl_name, "");

    ngx_conf_merge_value(conf->ssl_server_name, prev->ssl_server_name, 0);

    ngx_conf_merge_value(conf->ssl_verify, prev->ssl_verify, 0);

    ngx_conf_merge_uint_value(conf->ssl_verify_depth,
                              prev->ssl_verify_depth, 1);

    ngx_conf_merge_str_value(conf->ssl_trusted_certificate,
                              prev->ssl_trusted_certificate, "");

    ngx_conf_merge_str_value(conf->ssl_crl, prev->ssl_crl, "");

    ngx_conf_merge_str_value(conf->ssl_certificate,
                              prev->ssl_certificate, "");

    ngx_conf_merge_str_value(conf->ssl_certificate_key,
                              prev->ssl_certificate_key, "");

    ngx_conf_merge_ptr_value(conf->ssl_passwords, prev->ssl_passwords, NULL);

    if (conf->ssl_enable && ngx_stream_proxy_set_ssl(cf, conf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

#endif

    return NGX_CONF_OK;
}


#if (NGX_STREAM_SSL)

static ngx_int_t
ngx_stream_proxy_set_ssl(ngx_conf_t *cf, ngx_stream_proxy_srv_conf_t *pscf)
{
    ngx_pool_cleanup_t  *cln;

    pscf->ssl = ngx_pcalloc(cf->pool, sizeof(ngx_ssl_t));
    if (pscf->ssl == NULL) {
        return NGX_ERROR;
    }

    pscf->ssl->log = cf->log;

    if (ngx_ssl_create(pscf->ssl, pscf->ssl_protocols, NULL) != NGX_OK) {
        return NGX_ERROR;
    }

    cln = ngx_pool_cleanup_add(cf->pool, 0);
    if (cln == NULL) {
        return NGX_ERROR;
    }

    cln->handler = ngx_ssl_cleanup_ctx;
    cln->data = pscf->ssl;

    if (pscf->ssl_certificate.len) {

        if (pscf->ssl_certificate_key.len == 0) {
            ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                          "no \"proxy_ssl_certificate_key\" is defined "
                          "for certificate \"%V\"", &pscf->ssl_certificate);
            return NGX_ERROR;
        }

        if (ngx_ssl_certificate(cf, pscf->ssl, &pscf->ssl_certificate,
                                &pscf->ssl_certificate_key, pscf->ssl_passwords)
            != NGX_OK)
        {
            return NGX_ERROR;
        }
    }

    if (SSL_CTX_set_cipher_list(pscf->ssl->ctx,
                                (const char *) pscf->ssl_ciphers.data)
        == 0)
    {
        ngx_ssl_error(NGX_LOG_EMERG, cf->log, 0,
                      "SSL_CTX_set_cipher_list(\"%V\") failed",
                      &pscf->ssl_ciphers);
        return NGX_ERROR;
    }

    if (pscf->ssl_verify) {
        if (pscf->ssl_trusted_certificate.len == 0) {
            ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                      "no proxy_ssl_trusted_certificate for proxy_ssl_verify");
            return NGX_ERROR;
        }

        if (ngx_ssl_trusted_certificate(cf, pscf->ssl,
                                        &pscf->ssl_trusted_certificate,
                                        pscf->ssl_verify_depth)
            != NGX_OK)
        {
            return NGX_ERROR;
        }

        if (ngx_ssl_crl(cf, pscf->ssl, &pscf->ssl_crl) != NGX_OK) {
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}

#endif


// 解析proxy_pass指令，设置处理handler，在init建立连接之后会调用
// 获取一个upstream{}块的配置信息
static char *
ngx_stream_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_proxy_srv_conf_t *pscf = conf;

    ngx_url_t                    u;
    ngx_str_t                   *value, *url;
    ngx_stream_core_srv_conf_t  *cscf;

    // upstream是将要转发的上游服务器集群
    // 如果不空表示指令重复定义了
    if (pscf->upstream) {
        return "is duplicate";
    }

    cscf = ngx_stream_conf_get_module_srv_conf(cf, ngx_stream_core_module);

    // 设置处理handler，在init建立连接之后会调用
    cscf->handler = ngx_stream_proxy_handler;

    // 获取upstream名字
    value = cf->args->elts;

    url = &value[1];

    ngx_memzero(&u, sizeof(ngx_url_t));

    u.url = *url;
    u.no_resolve = 1;

    // 获取一个upstream{}块的配置信息
    // 获取时flags==0
    pscf->upstream = ngx_stream_upstream_add(cf, &u, 0);
    if (pscf->upstream == NULL) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}


static char *
ngx_stream_proxy_bind(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_stream_proxy_srv_conf_t *pscf = conf;

    ngx_int_t   rc;
    ngx_str_t  *value;

    if (pscf->local != NGX_CONF_UNSET_PTR) {
        return "is duplicate";
    }

    value = cf->args->elts;

    if (ngx_strcmp(value[1].data, "off") == 0) {
        pscf->local = NULL;
        return NGX_CONF_OK;
    }

    pscf->local = ngx_palloc(cf->pool, sizeof(ngx_addr_t));
    if (pscf->local == NULL) {
        return NGX_CONF_ERROR;
    }

    rc = ngx_parse_addr(cf->pool, pscf->local, value[1].data, value[1].len);

    switch (rc) {
    case NGX_OK:
        pscf->local->name = value[1];
        return NGX_CONF_OK;

    case NGX_DECLINED:
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "invalid address \"%V\"", &value[1]);
        /* fall through */

    default:
        return NGX_CONF_ERROR;
    }
}
