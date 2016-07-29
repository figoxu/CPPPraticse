// annotated by chrono since 2016
//
// * ngx_event_accept
// * ngx_trylock_accept_mutex

/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


// 遍历监听端口列表，加入epoll连接事件，开始接受请求
static ngx_int_t ngx_enable_accept_events(ngx_cycle_t *cycle);

// 遍历监听端口列表，删除epoll监听连接事件，不接受请求
static ngx_int_t ngx_disable_accept_events(ngx_cycle_t *cycle);

// 发生了错误，关闭一个连接
static void ngx_close_accepted_connection(ngx_connection_t *c);

// ngx_event_process_init里设置接受连接的回调函数为ngx_event_accept，可以接受连接
// 监听端口上收到连接请求时的回调函数，即事件handler
// 从cycle的连接池里获取连接
// 关键操作 ls->handler(c);调用其他模块的业务handler
void
ngx_event_accept(ngx_event_t *ev)
{
    socklen_t          socklen;
    ngx_err_t          err;
    ngx_log_t         *log;
    ngx_uint_t         level;
    ngx_socket_t       s;
    ngx_event_t       *rev, *wev;
    ngx_listening_t   *ls;
    ngx_connection_t  *c, *lc;
    ngx_event_conf_t  *ecf;
    u_char             sa[NGX_SOCKADDRLEN];
#if (NGX_HAVE_ACCEPT4)
    static ngx_uint_t  use_accept4 = 1;
#endif

    // 事件已经超时
    if (ev->timedout) {
        // 遍历监听端口列表，重新加入epoll连接事件
        if (ngx_enable_accept_events((ngx_cycle_t *) ngx_cycle) != NGX_OK) {
            return;
        }

        // 保证监听不超时
        ev->timedout = 0;
    }

    ecf = ngx_event_get_conf(ngx_cycle->conf_ctx, ngx_event_core_module);

    // rtsig在nginx 1.9.x已经删除
    if (ngx_event_flags & NGX_USE_RTSIG_EVENT) {
        ev->available = 1;

    } else if (!(ngx_event_flags & NGX_USE_KQUEUE_EVENT)) {
        // epoll是否允许尽可能接受多个请求
        ev->available = ecf->multi_accept;
    }

    // 事件的连接对象
    lc = ev->data;

    // 事件对应的监听端口对象
    ls = lc->listening;

    // 此时还没有数据可读
    ev->ready = 0;

    ngx_log_debug2(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                   "accept on %V, ready: %d", &ls->addr_text, ev->available);

    do {
        socklen = NGX_SOCKADDRLEN;

        // 调用accept接受连接，返回socket对象
#if (NGX_HAVE_ACCEPT4)
        if (use_accept4) {
            s = accept4(lc->fd, (struct sockaddr *) sa, &socklen,
                        SOCK_NONBLOCK);
        } else {
            s = accept(lc->fd, (struct sockaddr *) sa, &socklen);
        }
#else
        s = accept(lc->fd, (struct sockaddr *) sa, &socklen);
#endif

        // 接受连接出错
        if (s == (ngx_socket_t) -1) {
            err = ngx_socket_errno;

            // EAGAIN，此时已经没有新的连接，用于multi_accept
            if (err == NGX_EAGAIN) {
                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, ev->log, err,
                               "accept() not ready");
                return;
            }

            level = NGX_LOG_ALERT;

            if (err == NGX_ECONNABORTED) {
                level = NGX_LOG_ERR;

            } else if (err == NGX_EMFILE || err == NGX_ENFILE) {
                level = NGX_LOG_CRIT;
            }

#if (NGX_HAVE_ACCEPT4)
            ngx_log_error(level, ev->log, err,
                          use_accept4 ? "accept4() failed" : "accept() failed");

            if (use_accept4 && err == NGX_ENOSYS) {
                use_accept4 = 0;
                ngx_inherited_nonblocking = 0;
                continue;
            }
#else
            ngx_log_error(level, ev->log, err, "accept() failed");
#endif

            if (err == NGX_ECONNABORTED) {
                if (ngx_event_flags & NGX_USE_KQUEUE_EVENT) {
                    ev->available--;
                }

                if (ev->available) {
                    continue;
                }
            }

            // 系统的文件句柄数用完了
            if (err == NGX_EMFILE || err == NGX_ENFILE) {
                // 遍历监听端口列表，删除epoll监听连接事件，不接受请求
                if (ngx_disable_accept_events((ngx_cycle_t *) ngx_cycle)
                    != NGX_OK)
                {
                    return;
                }

                // 解锁负载均衡，允许其他进程接受请求
                if (ngx_use_accept_mutex) {
                    if (ngx_accept_mutex_held) {
                        ngx_shmtx_unlock(&ngx_accept_mutex);
                        ngx_accept_mutex_held = 0;
                    }

                    //未持有锁，暂时不接受请求
                    ngx_accept_disabled = 1;

                } else {
                    // 不使用负载均衡
                    // 等待一下，再次尝试接受请求
                    ngx_add_timer(ev, ecf->accept_mutex_delay);
                }
            }

            return;
        } // 接受连接出错

#if (NGX_STAT_STUB)
        (void) ngx_atomic_fetch_add(ngx_stat_accepted, 1);
#endif

        // 此时accept返回了一个socket描述符s

        // ngx_accept_disabled是总连接数的1/8-空闲连接数
        // 也就是说空闲连接数小于总数的1/8,那么就暂时停止接受连接
        ngx_accept_disabled = ngx_cycle->connection_n / 8
                              - ngx_cycle->free_connection_n;

        // 从全局变量ngx_cycle里获取空闲链接，即free_connections链表
        c = ngx_get_connection(s, ev->log);

        // 如果没有空闲连接，那么关闭socket，无法处理请求
        if (c == NULL) {
            if (ngx_close_socket(s) == -1) {
                ngx_log_error(NGX_LOG_ALERT, ev->log, ngx_socket_errno,
                              ngx_close_socket_n " failed");
            }

            return;
        }

#if (NGX_STAT_STUB)
        (void) ngx_atomic_fetch_add(ngx_stat_active, 1);
#endif

        // 创建连接使用的内存池
        // stream模块设置连接的内存池是256bytes，不可配置
        // http模块可以在ngx_http_core_srv_conf_t里配置
        c->pool = ngx_create_pool(ls->pool_size, ev->log);
        if (c->pool == NULL) {
            ngx_close_accepted_connection(c);
            return;
        }

        // 拷贝客户端sockaddr
        c->sockaddr = ngx_palloc(c->pool, socklen);
        if (c->sockaddr == NULL) {
            ngx_close_accepted_connection(c);
            return;
        }

        ngx_memcpy(c->sockaddr, sa, socklen);

        log = ngx_palloc(c->pool, sizeof(ngx_log_t));
        if (log == NULL) {
            ngx_close_accepted_connection(c);
            return;
        }

        /* set a blocking mode for aio and non-blocking mode for others */

        // 设置socket为非阻塞
        if (ngx_inherited_nonblocking) {
            if (ngx_event_flags & NGX_USE_AIO_EVENT) {
                if (ngx_blocking(s) == -1) {
                    ngx_log_error(NGX_LOG_ALERT, ev->log, ngx_socket_errno,
                                  ngx_blocking_n " failed");
                    ngx_close_accepted_connection(c);
                    return;
                }
            }

        } else {
            if (!(ngx_event_flags & (NGX_USE_AIO_EVENT|NGX_USE_RTSIG_EVENT))) {
                if (ngx_nonblocking(s) == -1) {
                    ngx_log_error(NGX_LOG_ALERT, ev->log, ngx_socket_errno,
                                  ngx_nonblocking_n " failed");
                    ngx_close_accepted_connection(c);
                    return;
                }
            }
        }

        *log = ls->log;

        // 连接的收发数据函数
        // #define ngx_recv             ngx_io.recv
        // #define ngx_recv_chain       ngx_io.recv_chain
        // ngx_posix_init.c里初始化为linux的底层接口
        c->recv = ngx_recv;
        c->send = ngx_send;
        c->recv_chain = ngx_recv_chain;
        c->send_chain = ngx_send_chain;

        c->log = log;
        c->pool->log = log;

        // 设置其他的成员
        c->socklen = socklen;
        c->listening = ls;
        c->local_sockaddr = ls->sockaddr;
        c->local_socklen = ls->socklen;

        c->unexpected_eof = 1;

#if (NGX_HAVE_UNIX_DOMAIN)
        if (c->sockaddr->sa_family == AF_UNIX) {
            c->tcp_nopush = NGX_TCP_NOPUSH_DISABLED;
            c->tcp_nodelay = NGX_TCP_NODELAY_DISABLED;
#if (NGX_SOLARIS)
            /* Solaris's sendfilev() supports AF_NCA, AF_INET, and AF_INET6 */
            c->sendfile = 0;
#endif
        }
#endif

        // 连接相关的读写事件
        rev = c->read;
        wev = c->write;

        // 建立连接后是可写的
        wev->ready = 1;

        // rtsig在nginx 1.9.x已经删除
        if (ngx_event_flags & (NGX_USE_AIO_EVENT|NGX_USE_RTSIG_EVENT)) {
            /* rtsig, aio, iocp */
            rev->ready = 1;
        }

        // 如果listen使用了deferred，那么建立连接时就已经有数据可读了
        if (ev->deferred_accept) {
            rev->ready = 1;
#if (NGX_HAVE_KQUEUE)
            rev->available = 1;
#endif
        }

        rev->log = log;
        wev->log = log;

        /*
         * TODO: MT: - ngx_atomic_fetch_add()
         *             or protection by critical section or light mutex
         *
         * TODO: MP: - allocated in a shared memory
         *           - ngx_atomic_fetch_add()
         *             or protection by critical section or light mutex
         */

        // 连接计数器增加
        c->number = ngx_atomic_fetch_add(ngx_connection_counter, 1);

#if (NGX_STAT_STUB)
        (void) ngx_atomic_fetch_add(ngx_stat_handled, 1);
#endif

        if (ls->addr_ntop) {
            c->addr_text.data = ngx_pnalloc(c->pool, ls->addr_text_max_len);
            if (c->addr_text.data == NULL) {
                ngx_close_accepted_connection(c);
                return;
            }

            c->addr_text.len = ngx_sock_ntop(c->sockaddr, c->socklen,
                                             c->addr_text.data,
                                             ls->addr_text_max_len, 0);
            if (c->addr_text.len == 0) {
                ngx_close_accepted_connection(c);
                return;
            }
        }

#if (NGX_DEBUG)
        {

        ngx_str_t             addr;
        struct sockaddr_in   *sin;
        ngx_cidr_t           *cidr;
        ngx_uint_t            i;
        u_char                text[NGX_SOCKADDR_STRLEN];
#if (NGX_HAVE_INET6)
        struct sockaddr_in6  *sin6;
        ngx_uint_t            n;
#endif

        cidr = ecf->debug_connection.elts;
        for (i = 0; i < ecf->debug_connection.nelts; i++) {
            if (cidr[i].family != (ngx_uint_t) c->sockaddr->sa_family) {
                goto next;
            }

            switch (cidr[i].family) {

#if (NGX_HAVE_INET6)
            case AF_INET6:
                sin6 = (struct sockaddr_in6 *) c->sockaddr;
                for (n = 0; n < 16; n++) {
                    if ((sin6->sin6_addr.s6_addr[n]
                        & cidr[i].u.in6.mask.s6_addr[n])
                        != cidr[i].u.in6.addr.s6_addr[n])
                    {
                        goto next;
                    }
                }
                break;
#endif

#if (NGX_HAVE_UNIX_DOMAIN)
            case AF_UNIX:
                break;
#endif

            default: /* AF_INET */
                sin = (struct sockaddr_in *) c->sockaddr;
                if ((sin->sin_addr.s_addr & cidr[i].u.in.mask)
                    != cidr[i].u.in.addr)
                {
                    goto next;
                }
                break;
            }

            log->log_level = NGX_LOG_DEBUG_CONNECTION|NGX_LOG_DEBUG_ALL;
            break;

        next:
            continue;
        }

        if (log->log_level & NGX_LOG_DEBUG_EVENT) {
            addr.data = text;
            addr.len = ngx_sock_ntop(c->sockaddr, c->socklen, text,
                                     NGX_SOCKADDR_STRLEN, 1);

            ngx_log_debug3(NGX_LOG_DEBUG_EVENT, log, 0,
                           "*%uA accept: %V fd:%d", c->number, &addr, s);
        }

        }
#endif

        // 连接的读写事件都加入epoll，即有读写都会由epoll收集事件并处理
        if (ngx_add_conn && (ngx_event_flags & NGX_USE_EPOLL_EVENT) == 0) {
            if (ngx_add_conn(c) == NGX_ERROR) {
                ngx_close_accepted_connection(c);
                return;
            }
        }

        log->data = NULL;
        log->handler = NULL;

        // 接受连接，收到请求的回调函数
        // 在http模块里是http.c:ngx_http_init_connection
        // stream模块里是ngx_stream_init_connection
        ls->handler(c);

        // epoll不处理
        if (ngx_event_flags & NGX_USE_KQUEUE_EVENT) {
            ev->available--;
        }

    // 如果ev->available = ecf->multi_accept;
    // epoll尽可能接受多个请求，直至accept出错EAGAIN，即无新连接请求
    // 否则epoll只接受一个请求后即退出循环
    } while (ev->available);
}


// 尝试获取负载均衡锁，监听端口
// 如未获取则不监听端口
// 锁标志ngx_accept_mutex_held
// 内部调用ngx_enable_accept_events/ngx_disable_accept_events
ngx_int_t
ngx_trylock_accept_mutex(ngx_cycle_t *cycle)
{
    // 尝试锁定共享内存锁
    // 非阻塞，会立即返回
    if (ngx_shmtx_trylock(&ngx_accept_mutex)) {

        ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0,
                       "accept mutex locked");

        // 锁成功

        // 之前已经持有了锁，那么就直接返回，继续监听端口
        // ngx_accept_events在epoll里不使用
        if (ngx_accept_mutex_held
            && ngx_accept_events == 0
            // rtsig在nginx 1.9.x已经删除
            && !(ngx_event_flags & NGX_USE_RTSIG_EVENT))
        {
            return NGX_OK;
        }

        // 之前没有持有锁，需要注册epoll事件监听端口

        // 遍历监听端口列表，加入epoll连接事件，开始接受请求
        if (ngx_enable_accept_events(cycle) == NGX_ERROR) {

            // 如果监听失败就需要立即解锁，函数结束
            ngx_shmtx_unlock(&ngx_accept_mutex);
            return NGX_ERROR;
        }

        // 已经成功将监听事件加入epoll

        // 设置已经获得锁的标志
        ngx_accept_events = 0;
        ngx_accept_mutex_held = 1;

        return NGX_OK;
    }

    // try失败，未获得锁，极小的消耗

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, cycle->log, 0,
                   "accept mutex lock failed: %ui", ngx_accept_mutex_held);

    // 未获得锁
    // 但之前持有锁，也就是说之前在监听端口
    if (ngx_accept_mutex_held) {
        // 遍历监听端口列表，删除epoll监听连接事件，不接受请求
        if (ngx_disable_accept_events(cycle) == NGX_ERROR) {
            return NGX_ERROR;
        }

        // 设置未获得锁的标志
        ngx_accept_mutex_held = 0;
    }

    return NGX_OK;
}


// 遍历监听端口列表，加入epoll连接事件，开始接受请求
static ngx_int_t
ngx_enable_accept_events(ngx_cycle_t *cycle)
{
    ngx_uint_t         i;
    ngx_listening_t   *ls;
    ngx_connection_t  *c;

    // 遍历监听端口列表
    ls = cycle->listening.elts;
    for (i = 0; i < cycle->listening.nelts; i++) {

        c = ls[i].connection;

        // 如果连接已经监听（读事件激活）那么就跳过
        if (c->read->active) {
            continue;
        }

        // rtsig在nginx 1.9.x已经删除
        if (ngx_event_flags & NGX_USE_RTSIG_EVENT) {

            if (ngx_add_conn(c) == NGX_ERROR) {
                return NGX_ERROR;
            }

        } else {
            // #define ngx_add_event        ngx_event_actions.add
            // 加入读事件，即接受连接事件
            if (ngx_add_event(c->read, NGX_READ_EVENT, 0) == NGX_ERROR) {
                return NGX_ERROR;
            }
        }
    }

    return NGX_OK;
}


// 遍历监听端口列表，删除epoll监听连接事件，不接受请求
static ngx_int_t
ngx_disable_accept_events(ngx_cycle_t *cycle)
{
    ngx_uint_t         i;
    ngx_listening_t   *ls;
    ngx_connection_t  *c;

    // 遍历监听端口列表
    ls = cycle->listening.elts;
    for (i = 0; i < cycle->listening.nelts; i++) {

        c = ls[i].connection;

        // 如果连接没有监听（读事件不激活）那么就跳过
        if (!c->read->active) {
            continue;
        }

        // rtsig在nginx 1.9.x已经删除
        if (ngx_event_flags & NGX_USE_RTSIG_EVENT) {
            if (ngx_del_conn(c, NGX_DISABLE_EVENT) == NGX_ERROR) {
                return NGX_ERROR;
            }

        } else {
            // 删除读事件，即不接受连接事件
            // NGX_DISABLE_EVENT暂时无用
            if (ngx_del_event(c->read, NGX_READ_EVENT, NGX_DISABLE_EVENT)
                == NGX_ERROR)
            {
                return NGX_ERROR;
            }
        }
    }

    return NGX_OK;
}


// 发生了错误，关闭一个连接
static void
ngx_close_accepted_connection(ngx_connection_t *c)
{
    ngx_socket_t  fd;

    // 释放连接，加入空闲链表
    // in core/ngx_connection.c
    ngx_free_connection(c);

    // 连接的描述符置为无效
    fd = c->fd;
    c->fd = (ngx_socket_t) -1;

    // 关闭socket
    if (ngx_close_socket(fd) == -1) {
        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_socket_errno,
                      ngx_close_socket_n " failed");
    }

    // 释放连接相关的所有内存
    if (c->pool) {
        ngx_destroy_pool(c->pool);
    }

#if (NGX_STAT_STUB)
    (void) ngx_atomic_fetch_add(ngx_stat_active, -1);
#endif
}


u_char *
ngx_accept_log_error(ngx_log_t *log, u_char *buf, size_t len)
{
    return ngx_snprintf(buf, len, " while accepting new connection on %V",
                        log->data);
}
