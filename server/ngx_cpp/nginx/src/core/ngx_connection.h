// annotated by chrono since 2016
//
// 本文件包含两个重要结构体
// * ngx_listening_s
// * ngx_connection_s

/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CONNECTION_H_INCLUDED_
#define _NGX_CONNECTION_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_listening_s  ngx_listening_t;

// 监听端口数据结构
// 存储在ngx_cycle_t::listening数组里
// 主要成员: fd,backlog,rcvbuf,sndbuf,handler
// 由http模块用listen指令添加
struct ngx_listening_s {

    // socket描述符（句柄）
    ngx_socket_t        fd;

    // sockaddr指针
    struct sockaddr    *sockaddr;

    // sockaddr长度
    socklen_t           socklen;    /* size of sockaddr */

    // addr_text的最大长度
    size_t              addr_text_max_len;

    // 文本形式的地址
    ngx_str_t           addr_text;

    // socket的类型，SOCK_STREAM 表示TCP，
    int                 type;

    // TCP的backlog队列，即等待连接的队列
    int                 backlog;

    // 接收缓冲区大小
    int                 rcvbuf;

    // 发送缓冲区大小
    int                 sndbuf;

#if (NGX_HAVE_KEEPALIVE_TUNABLE)
    int                 keepidle;
    int                 keepintvl;
    int                 keepcnt;
#endif

    /* handler of accepted connection */
    // 重要函数，tcp连接成功时的回调函数
    // 对于http模块是ngx_http_request.c:ngx_http_init_connection
    // stream模块是ngx_stream_init_connection
    ngx_connection_handler_pt   handler;

    // 用于解决多个server监听相同端口的情况
    // http模块是ngx_http_in_addr_t,
    // stream模块是ngx_stream_addr_conf_t
    void               *servers;  /* array of ngx_http_in_addr_t, for example */

    ngx_log_t           log;
    ngx_log_t          *logp;

    // 内存池的初始大小
    size_t              pool_size;

    /* should be here because of the AcceptEx() preread */
    size_t              post_accept_buffer_size;
    /* should be here because of the deferred accept */
    ngx_msec_t          post_accept_timeout;

    // 链表指针，多个ngx_listening_t组成一个单向链表
    ngx_listening_t    *previous;

    // 监听端口对应的连接对象
    ngx_connection_t   *connection;

    // 以下是一些标志位
    unsigned            open:1;
    unsigned            remain:1;
    unsigned            ignore:1;

    unsigned            bound:1;       /* already bound */
    unsigned            inherited:1;   /* inherited from previous process */
    unsigned            nonblocking_accept:1;

    // 是否已经被监听
    unsigned            listen:1;

    unsigned            nonblocking:1;
    unsigned            shared:1;    /* shared between threads or processes */
    unsigned            addr_ntop:1;

#if (NGX_HAVE_INET6 && defined IPV6_V6ONLY)
    unsigned            ipv6only:1;
#endif
    unsigned            keepalive:2;

#if (NGX_HAVE_DEFERRED_ACCEPT)
    unsigned            deferred_accept:1;
    unsigned            delete_deferred:1;
    unsigned            add_deferred:1;
#ifdef SO_ACCEPTFILTER
    char               *accept_filter;
#endif
#endif
#if (NGX_HAVE_SETFIB)
    int                 setfib;
#endif

#if (NGX_HAVE_TCP_FASTOPEN)
    int                 fastopen;
#endif

};


typedef enum {
     NGX_ERROR_ALERT = 0,
     NGX_ERROR_ERR,
     NGX_ERROR_INFO,
     NGX_ERROR_IGNORE_ECONNRESET,
     NGX_ERROR_IGNORE_EINVAL
} ngx_connection_log_error_e;


typedef enum {
     NGX_TCP_NODELAY_UNSET = 0,
     NGX_TCP_NODELAY_SET,
     NGX_TCP_NODELAY_DISABLED
} ngx_connection_tcp_nodelay_e;


typedef enum {
     NGX_TCP_NOPUSH_UNSET = 0,
     NGX_TCP_NOPUSH_SET,
     NGX_TCP_NOPUSH_DISABLED
} ngx_connection_tcp_nopush_e;


#define NGX_LOWLEVEL_BUFFERED  0x0f
#define NGX_SSL_BUFFERED       0x01
#define NGX_SPDY_BUFFERED      0x02


// 连接结构体，表示nginx里的一个tcp连接
// 每个连接都有一个读事件和写事件，使用数组序号对应
// nginx里的连接对象都保存在ngx_cycle_t::connections数组里
struct ngx_connection_s {
    // data成员有两种用法
    // 未使用（空闲）时作为链表的后继指针，连接在ngx_cycle_t::free_connections里
    // 在http模块里保存ngx_http_request_t对象，标记连接对应的http请求
    // 在stream模块里保存ngx_stream_session_t对象
    void               *data;

    // 连接对应的读事件，存储在ngx_cycle_t::read_events
    ngx_event_t        *read;

    // 连接对应的写事件，存储在ngx_cycle_t::write_events
    ngx_event_t        *write;

    // 连接的socket描述符（句柄）
    ngx_socket_t        fd;

    // 接收数据的函数指针
    // ngx_event_accept.c:ngx_event_accept()里设置为ngx_recv
    // ngx_posix_init.c里初始化为linux的底层接口
    ngx_recv_pt         recv;

    // 发送数据的函数指针
    // ngx_event_accept.c:ngx_event_accept()里设置为ngx_send
    // ngx_posix_init.c里初始化为linux的底层接口
    ngx_send_pt         send;

    ngx_recv_chain_pt   recv_chain;

    // linux下实际上是ngx_writev_chain.c:ngx_writev_chain
    //
    // 发送limit长度（字节数）的数据
    // 如果事件not ready，即暂不可写，那么立即返回，无动作
    // 要求缓冲区必须在内存里，否则报错
    // 最后返回消费缓冲区之后的链表指针
    // 发送出错、遇到again、发送完毕，这三种情况函数结束
    // 返回的是最后发送到的链表节点指针
    //
    // 发送后需要把已经发送过的节点都回收，供以后复用
    ngx_send_chain_pt   send_chain;

    // 连接对应的ngx_listening_t监听对象
    ngx_listening_t    *listening;

    // 连接上已经发送的字节数
    // ngx_send.c里发送数据成功后增加
    off_t               sent;

    // 用于记录日志的log
    ngx_log_t          *log;

    // 连接的内存池
    // 默认大小是256字节
    ngx_pool_t         *pool;

    // 客户端的sockaddr
    struct sockaddr    *sockaddr;
    socklen_t           socklen;

    // 客户端的sockaddr，文本形式
    ngx_str_t           addr_text;

    ngx_str_t           proxy_protocol_addr;

#if (NGX_SSL)
    ngx_ssl_connection_t  *ssl;
#endif

    // 本地监听端口的socketaddr，也就是listening中的sockaddr
    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;

    // 接收客户端发送数据的缓冲区
    // 与listening中的rcvbuf不同，这个是nginx应用层的
    // 在ngx_http_wait_request_handler里分配内存
    ngx_buf_t          *buffer;

    // 侵入式队列，加入到ngx_cycle_t::reusable_connections_queue
    // 复用连接对象
    ngx_queue_t         queue;

    // 连接创建的计数器
    // ngx_event_accept.c:ngx_event_accept()
    // c->number = ngx_atomic_fetch_add(ngx_connection_counter, 1);
    ngx_atomic_uint_t   number;

    // 处理的请求次数，在ngx_http_create_request里增加
    // 用来控制长连接里可处理的请求次数，指令keepalive_requests
    ngx_uint_t          requests;

    // 标志位，表示连接有数据缓冲待发送
    // c->buffered |= NGX_HTTP_WRITE_BUFFERED;
    // 见ngx_http_write_filter_module.c
    unsigned            buffered:8;

    unsigned            log_error:3;     /* ngx_connection_log_error_e */

    unsigned            unexpected_eof:1;

    // 是否已经超时
    unsigned            timedout:1;

    // 是否已经出错
    unsigned            error:1;

    // 是否tcp连接已经被销毁
    unsigned            destroyed:1;

    // 连接处于空闲状态
    unsigned            idle:1;

    // 连接可以复用，对应上面的queue成员
    // 即已经加入了ngx_cycle_t::reusable_connections_queue
    unsigned            reusable:1;

    // tcp连接已经关闭
    // 可以回收复用
    unsigned            close:1;

    // 正在发送文件
    unsigned            sendfile:1;

    // 是否已经设置发送数据时epoll的响应阈值
    // ngx_event.c:ngx_send_lowat()
    unsigned            sndlowat:1;

    unsigned            tcp_nodelay:2;   /* ngx_connection_tcp_nodelay_e */
    unsigned            tcp_nopush:2;    /* ngx_connection_tcp_nopush_e */

    unsigned            need_last_buf:1;

#if (NGX_HAVE_IOCP)
    unsigned            accept_context_updated:1;
#endif

#if (NGX_HAVE_AIO_SENDFILE)
    unsigned            busy_count:2;
#endif

#if (NGX_THREADS)
    ngx_thread_task_t  *sendfile_task;
#endif
};


// http/ngx_http.c:ngx_http_add_listening()里调用
// ngx_stream.c:ngx_stream_optimize_servers()里调用
// 添加到cycle的监听端口数组
ngx_listening_t *ngx_create_listening(ngx_conf_t *cf, void *sockaddr,
    socklen_t socklen);

ngx_int_t ngx_set_inherited_sockets(ngx_cycle_t *cycle);

// ngx_cycle.c : init_cycle()里被调用
// 创建socket, bind/listen
ngx_int_t ngx_open_listening_sockets(ngx_cycle_t *cycle);

// ngx_init_cycle()里调用，在ngx_open_listening_sockets()之后
// 配置监听端口的rcvbuf/sndbuf等参数，调用setsockopt()
void ngx_configure_listening_sockets(ngx_cycle_t *cycle);

// 在ngx_master_process_exit里被调用(os/unix/ngx_process_cycle.c)
// 遍历监听端口列表，逐个删除监听事件
void ngx_close_listening_sockets(ngx_cycle_t *cycle);

// 关闭连接，删除epoll里的读写事件
// 释放连接，加入空闲链表，可以再次使用
void ngx_close_connection(ngx_connection_t *c);

ngx_int_t ngx_connection_local_sockaddr(ngx_connection_t *c, ngx_str_t *s,
    ngx_uint_t port);
ngx_int_t ngx_connection_error(ngx_connection_t *c, ngx_err_t err, char *text);

// 从全局变量ngx_cycle里获取空闲链接，即free_connections链表
// 如果没有空闲连接，调用ngx_drain_connections释放一些可复用的连接
ngx_connection_t *ngx_get_connection(ngx_socket_t s, ngx_log_t *log);

// 释放一个连接，加入空闲链表
void ngx_free_connection(ngx_connection_t *c);

// 连接加入cycle的复用队列ngx_cycle->reusable_connections_queue
// 参数reusable表示是否可以复用，即加入队列
void ngx_reusable_connection(ngx_connection_t *c, ngx_uint_t reusable);

#endif /* _NGX_CONNECTION_H_INCLUDED_ */
