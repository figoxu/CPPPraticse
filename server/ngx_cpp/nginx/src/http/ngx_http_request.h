// annotated by chrono since 2016
//
// * ngx_http_headers_in_t
// * ngx_http_request_body_t
// * ngx_http_request_s

/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_REQUEST_H_INCLUDED_
#define _NGX_HTTP_REQUEST_H_INCLUDED_


// nginx不允许无限改写uri跳转，最多10次
// 检查在ngx_http_core_post_rewrite_phase
#define NGX_HTTP_MAX_URI_CHANGES           10

// 最多只能产生200个子请求
// 避免过多子请求导致处理效率降低
#define NGX_HTTP_MAX_SUBREQUESTS           200

/* must be 2^n */
#define NGX_HTTP_LC_HEADER_LEN             32

// 在丢弃请求体数据时的缓冲区长度，4k
// 用在ngx_http_read_discarded_request_body
#define NGX_HTTP_DISCARD_BUFFER_SIZE       4096

#define NGX_HTTP_LINGERING_BUFFER_SIZE     4096


// HTTP协议版本号标记，nginx1.8还不支持HTTP2
#define NGX_HTTP_VERSION_9                 9
#define NGX_HTTP_VERSION_10                1000
#define NGX_HTTP_VERSION_11                1001

// http请求方法代码，可以使用与或非来检查，存储在r->method
// r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD)
#define NGX_HTTP_UNKNOWN                   0x0001
#define NGX_HTTP_GET                       0x0002
#define NGX_HTTP_HEAD                      0x0004
#define NGX_HTTP_POST                      0x0008
#define NGX_HTTP_PUT                       0x0010
#define NGX_HTTP_DELETE                    0x0020
#define NGX_HTTP_MKCOL                     0x0040
#define NGX_HTTP_COPY                      0x0080
#define NGX_HTTP_MOVE                      0x0100
#define NGX_HTTP_OPTIONS                   0x0200
#define NGX_HTTP_PROPFIND                  0x0400
#define NGX_HTTP_PROPPATCH                 0x0800
#define NGX_HTTP_LOCK                      0x1000
#define NGX_HTTP_UNLOCK                    0x2000
#define NGX_HTTP_PATCH                     0x4000
#define NGX_HTTP_TRACE                     0x8000

#define NGX_HTTP_CONNECTION_CLOSE          1
#define NGX_HTTP_CONNECTION_KEEP_ALIVE     2


#define NGX_NONE                           1


#define NGX_HTTP_PARSE_HEADER_DONE         1

#define NGX_HTTP_CLIENT_ERROR              10
#define NGX_HTTP_PARSE_INVALID_METHOD      10
#define NGX_HTTP_PARSE_INVALID_REQUEST     11
#define NGX_HTTP_PARSE_INVALID_09_METHOD   12

#define NGX_HTTP_PARSE_INVALID_HEADER      13


/* unused                                  1 */
#define NGX_HTTP_SUBREQUEST_IN_MEMORY      2
#define NGX_HTTP_SUBREQUEST_WAITED         4
#define NGX_HTTP_LOG_UNSAFE                8


// http状态码，如200、302、404
#define NGX_HTTP_CONTINUE                  100
#define NGX_HTTP_SWITCHING_PROTOCOLS       101
#define NGX_HTTP_PROCESSING                102

#define NGX_HTTP_OK                        200
#define NGX_HTTP_CREATED                   201
#define NGX_HTTP_ACCEPTED                  202
#define NGX_HTTP_NO_CONTENT                204
#define NGX_HTTP_PARTIAL_CONTENT           206

#define NGX_HTTP_SPECIAL_RESPONSE          300
#define NGX_HTTP_MOVED_PERMANENTLY         301
#define NGX_HTTP_MOVED_TEMPORARILY         302
#define NGX_HTTP_SEE_OTHER                 303
#define NGX_HTTP_NOT_MODIFIED              304
#define NGX_HTTP_TEMPORARY_REDIRECT        307

#define NGX_HTTP_BAD_REQUEST               400
#define NGX_HTTP_UNAUTHORIZED              401
#define NGX_HTTP_FORBIDDEN                 403
#define NGX_HTTP_NOT_FOUND                 404
#define NGX_HTTP_NOT_ALLOWED               405
#define NGX_HTTP_REQUEST_TIME_OUT          408
#define NGX_HTTP_CONFLICT                  409
#define NGX_HTTP_LENGTH_REQUIRED           411
#define NGX_HTTP_PRECONDITION_FAILED       412
#define NGX_HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define NGX_HTTP_REQUEST_URI_TOO_LARGE     414
#define NGX_HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define NGX_HTTP_RANGE_NOT_SATISFIABLE     416


/* Our own HTTP codes */

// 这里是nginx自己定义的特殊状态码

/* The special code to close connection without any response */
#define NGX_HTTP_CLOSE                     444

#define NGX_HTTP_NGINX_CODES               494

#define NGX_HTTP_REQUEST_HEADER_TOO_LARGE  494

#define NGX_HTTPS_CERT_ERROR               495
#define NGX_HTTPS_NO_CERT                  496

/*
 * We use the special code for the plain HTTP requests that are sent to
 * HTTPS port to distinguish it from 4XX in an error page redirection
 */
#define NGX_HTTP_TO_HTTPS                  497

/* 498 is the canceled code for the requests with invalid host name */

/*
 * HTTP does not define the code for the case when a client closed
 * the connection while we are processing its request so we introduce
 * own code to log such situation when a client has closed the connection
 * before we even try to send the HTTP header to it
 */
// 客户端主动断连的错误码
#define NGX_HTTP_CLIENT_CLOSED_REQUEST     499


#define NGX_HTTP_INTERNAL_SERVER_ERROR     500
#define NGX_HTTP_NOT_IMPLEMENTED           501
#define NGX_HTTP_BAD_GATEWAY               502
#define NGX_HTTP_SERVICE_UNAVAILABLE       503
#define NGX_HTTP_GATEWAY_TIME_OUT          504
#define NGX_HTTP_INSUFFICIENT_STORAGE      507


#define NGX_HTTP_LOWLEVEL_BUFFERED         0xf0
#define NGX_HTTP_WRITE_BUFFERED            0x10
#define NGX_HTTP_GZIP_BUFFERED             0x20
#define NGX_HTTP_SSI_BUFFERED              0x01
#define NGX_HTTP_SUB_BUFFERED              0x02
#define NGX_HTTP_COPY_BUFFERED             0x04


// 标记http请求的处理状态
typedef enum {
    // 此状态不使用
    NGX_HTTP_INITING_REQUEST_STATE = 0,

    // 刚创建请求对象，正在读取请求数据
    NGX_HTTP_READING_REQUEST_STATE,

    // 请求头解析完毕，准备处理请求
    NGX_HTTP_PROCESS_REQUEST_STATE,

    NGX_HTTP_CONNECT_UPSTREAM_STATE,
    NGX_HTTP_WRITING_UPSTREAM_STATE,
    NGX_HTTP_READING_UPSTREAM_STATE,

    NGX_HTTP_WRITING_REQUEST_STATE,
    NGX_HTTP_LINGERING_CLOSE_STATE,
    NGX_HTTP_KEEPALIVE_STATE
} ngx_http_state_e;


typedef struct {
    ngx_str_t                         name;
    ngx_uint_t                        offset;
    ngx_http_header_handler_pt        handler;
} ngx_http_header_t;


typedef struct {
    ngx_str_t                         name;
    ngx_uint_t                        offset;
} ngx_http_header_out_t;


// http请求头数据结构，可以直接用指针获得常用的头
// 所有头都存储在headers列表里，类型是ngx_table_elt_t
// 自定义或非常用头需要遍历链表查找
// content_length_n直接把头里的长度字符串转换为数字
typedef struct {
    // 所有头都存储在headers列表里，类型是ngx_table_elt_t
    ngx_list_t                        headers;

    // host、range等常用头，可以直接获取
    // 如果不存在那么指针就是nullptr
    ngx_table_elt_t                  *host;
    ngx_table_elt_t                  *connection;
    ngx_table_elt_t                  *if_modified_since;
    ngx_table_elt_t                  *if_unmodified_since;
    ngx_table_elt_t                  *if_match;
    ngx_table_elt_t                  *if_none_match;
    ngx_table_elt_t                  *user_agent;
    ngx_table_elt_t                  *referer;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_type;

    ngx_table_elt_t                  *range;
    ngx_table_elt_t                  *if_range;

    ngx_table_elt_t                  *transfer_encoding;
    ngx_table_elt_t                  *expect;
    ngx_table_elt_t                  *upgrade;

#if (NGX_HTTP_GZIP)
    ngx_table_elt_t                  *accept_encoding;
    ngx_table_elt_t                  *via;
#endif

    ngx_table_elt_t                  *authorization;

    ngx_table_elt_t                  *keep_alive;

#if (NGX_HTTP_X_FORWARDED_FOR)
    ngx_array_t                       x_forwarded_for;
#endif

#if (NGX_HTTP_REALIP)
    ngx_table_elt_t                  *x_real_ip;
#endif

#if (NGX_HTTP_HEADERS)
    ngx_table_elt_t                  *accept;
    ngx_table_elt_t                  *accept_language;
#endif

#if (NGX_HTTP_DAV)
    ngx_table_elt_t                  *depth;
    ngx_table_elt_t                  *destination;
    ngx_table_elt_t                  *overwrite;
    ngx_table_elt_t                  *date;
#endif

    ngx_str_t                         user;
    ngx_str_t                         passwd;

    ngx_array_t                       cookies;

    ngx_str_t                         server;

    // content_length_n直接把头里的长度字符串转换为数字
    // content_length_n置0，表示无数据，丢弃成功
    off_t                             content_length_n;

    // keep_alive_n也直接转换为数字
    time_t                            keep_alive_n;

    unsigned                          connection_type:2;
    unsigned                          chunked:1;

    // user agent标志位
    unsigned                          msie:1;
    unsigned                          msie6:1;
    unsigned                          opera:1;
    unsigned                          gecko:1;
    unsigned                          chrome:1;
    unsigned                          safari:1;
    unsigned                          konqueror:1;
} ngx_http_headers_in_t;


// 响应头数据结构
// 响应头需要放进headers列表，也可以直接用指针指定常用头，不必用列表
// 通常需要指定content_length_n，表示body的长度
// status是响应码，status_line可以定制响应状态行
typedef struct {
    ngx_list_t                        headers;

    // status是响应码，status_line可以定制响应状态行
    ngx_uint_t                        status;
    ngx_str_t                         status_line;

    // 常用头
    ngx_table_elt_t                  *server;
    ngx_table_elt_t                  *date;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_encoding;
    ngx_table_elt_t                  *location;
    ngx_table_elt_t                  *refresh;
    ngx_table_elt_t                  *last_modified;
    ngx_table_elt_t                  *content_range;
    ngx_table_elt_t                  *accept_ranges;
    ngx_table_elt_t                  *www_authenticate;
    ngx_table_elt_t                  *expires;
    ngx_table_elt_t                  *etag;

    ngx_str_t                        *override_charset;

    size_t                            content_type_len;
    ngx_str_t                         content_type;
    ngx_str_t                         charset;
    u_char                           *content_type_lowcase;
    ngx_uint_t                        content_type_hash;

    ngx_array_t                       cache_control;

    // 通常需要指定content_length_n，表示body的长度
    off_t                             content_length_n;

    time_t                            date_time;
    time_t                            last_modified_time;
} ngx_http_headers_out_t;


// 请求体的处理函数
typedef void (*ngx_http_client_body_handler_pt)(ngx_http_request_t *r);

// 请求体的数据结构，用于读取或丢弃请求体数据
typedef struct {
    ngx_temp_file_t                  *temp_file;

    // 收到的数据都存在这个链表里
    // 最后一个节点b->last_buf = 1
    ngx_chain_t                      *bufs;

    // 当前使用的缓冲区
    ngx_buf_t                        *buf;

    // 剩余要读取的字节数
    // 对于确定长度（有content length）的就是r->headers_in.content_length_n
    // 在读取过程中会不断变化，最终为0
    off_t                             rest;

    // 空闲节点链表，优化用，避免再向内存池要节点
    ngx_chain_t                      *free;

    ngx_chain_t                      *busy;

    // 读取chunk数据的结构体，用于ngx_http_parse_chunked()
    ngx_http_chunked_t               *chunked;

    // 当读取完毕后的回调函数
    // 即ngx_http_read_client_request_body的第二个参数
    ngx_http_client_body_handler_pt   post_handler;
} ngx_http_request_body_t;


typedef struct ngx_http_addr_conf_s  ngx_http_addr_conf_t;

// 建立连接时server{}里相关的信息
// 重要的是conf_ctx，server的配置数组
typedef struct {
    ngx_http_addr_conf_t             *addr_conf;

    // server{}里的配置数组
    ngx_http_conf_ctx_t              *conf_ctx;

#if (NGX_HTTP_SSL && defined SSL_CTRL_SET_TLSEXT_HOSTNAME)
    ngx_str_t                        *ssl_servername;
#if (NGX_PCRE)
    ngx_http_regex_t                 *ssl_servername_regex;
#endif
#endif

    ngx_buf_t                       **busy;
    ngx_int_t                         nbusy;

    ngx_buf_t                       **free;
    ngx_int_t                         nfree;

#if (NGX_HTTP_SSL)
    unsigned                          ssl:1;
#endif

    // listen指令是否使用了proxy_protocol参数
    unsigned                          proxy_protocol:1;
} ngx_http_connection_t;


// http请求处理完毕时的清理函数，相当于析构
typedef void (*ngx_http_cleanup_pt)(void *data);

typedef struct ngx_http_cleanup_s  ngx_http_cleanup_t;

struct ngx_http_cleanup_s {
    // http请求处理完毕时的清理函数，相当于析构
    ngx_http_cleanup_pt               handler;

    // 作为参数传递给handler
    void                             *data;

    // 链表指针，所有的清理结构体连接成一个链表
    ngx_http_cleanup_t               *next;
};


typedef ngx_int_t (*ngx_http_post_subrequest_pt)(ngx_http_request_t *r,
    void *data, ngx_int_t rc);

// 子请求完成后的处理函数，相当于闭包/lambda
typedef struct {
    ngx_http_post_subrequest_pt       handler;
    void                             *data;
} ngx_http_post_subrequest_t;


typedef struct ngx_http_postponed_request_s  ngx_http_postponed_request_t;

struct ngx_http_postponed_request_s {
    ngx_http_request_t               *request;
    ngx_chain_t                      *out;
    ngx_http_postponed_request_t     *next;
};


typedef struct ngx_http_posted_request_s  ngx_http_posted_request_t;

struct ngx_http_posted_request_s {
    ngx_http_request_t               *request;
    ngx_http_posted_request_t        *next;
};


typedef ngx_int_t (*ngx_http_handler_pt)(ngx_http_request_t *r);
typedef void (*ngx_http_event_handler_pt)(ngx_http_request_t *r);


// http处理的核心数据结构
// 保存有所有http模块的配置、ctx数据、请求头、请求体
// 读写事件的处理函数
struct ngx_http_request_s {
    uint32_t                          signature;         /* "HTTP" */

    // 请求对应的连接对象，里面有log用于记录日志
    // 里面还有读写事件read/write
    ngx_connection_t                 *connection;

    // 保存有所有http模块的配置、ctx数据
    // 使用ngx_http_get_module_ctx获取ctx
    void                            **ctx;

    // 使用ngx_http_get_module_main_conf访问
    void                            **main_conf;
    void                            **srv_conf;
    void                            **loc_conf;

    // 读写事件的处理函数
    // 随着处理的阶段不同会变化
    ngx_http_event_handler_pt         read_event_handler;

    // 写最开始是ngx_http_empty_handler
    // 然后是ngx_http_core_run_phases
    // 当进入content阶段调用location handler后变成ngx_http_request_empty_handler
    ngx_http_event_handler_pt         write_event_handler;

#if (NGX_HTTP_CACHE)
    ngx_http_cache_t                 *cache;
#endif

    ngx_http_upstream_t              *upstream;
    ngx_array_t                      *upstream_states;
                                         /* of ngx_http_upstream_state_t */

    // 请求的内存池，请求结束时会回收
    ngx_pool_t                       *pool;

    // 缓冲区，用于读取请求头
    // 如果有请求体数据，也会都读到这里
    ngx_buf_t                        *header_in;

    // 请求头结构体
    // 里面用链表存储了所有的头，也可以用指针快速访问常用头
    ngx_http_headers_in_t             headers_in;

    // 响应头结构体
    // 里面有状态码/状态行和响应头链表
    ngx_http_headers_out_t            headers_out;

    // 读取并存储请求体
    // 指针的形式只有在需要的时候才分配内存
    // 相关函数ngx_http_discard_request_body/ngx_http_read_client_request_body
    ngx_http_request_body_t          *request_body;

    // 延迟关闭的时间点，用于ngx_http_discarded_request_body_handler
    // 可以在这之前接收数据
    time_t                            lingering_time;

    // 请求开始的时间，可用于限速
    time_t                            start_sec;
    ngx_msec_t                        start_msec;

    // 从请求头解析出来的方法
    // r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD)
    ngx_uint_t                        method;

    // http协议版本号，通常不需要关心
    ngx_uint_t                        http_version;

    // 请求行字符串
    ngx_str_t                         request_line;

    // uri地址，不含参数，即$uri
    ngx_str_t                         uri;

    // uri后的参数，不含问号，即$args
    ngx_str_t                         args;

    // uri里文件的扩展名
    ngx_str_t                         exten;

    // 原始请求uri，未解码，即$request_uri
    ngx_str_t                         unparsed_uri;

    // 请求的方法名字符串，例如GET/POST/DELETE
    // 因为字符串比较慢，所以应该尽量用method来判断方法
    ngx_str_t                         method_name;

    // http协议字符串，通常不需要关注
    ngx_str_t                         http_protocol;

    // 发送的数据链表
    // 所有的header、body数据都会存在这里
    ngx_chain_t                      *out;

    // 指向主请求，即由客户端发起的请求
    // 如果没有子请求，那么r == main
    ngx_http_request_t               *main;

    // 父请求，如果是子请求，那么指向产生它的父请求
    // 如果是主请求，指针是空
    ngx_http_request_t               *parent;

    // 子请求处理相关的数据结构
    ngx_http_postponed_request_t     *postponed;
    ngx_http_post_subrequest_t       *post_subrequest;
    ngx_http_posted_request_t        *posted_requests;

    // 执行ngx_http_core_run_phases时的重要参数，标记在引擎数组里的位置
    ngx_int_t                         phase_handler;

    // 重要！！
    // 本location专门的内容处理函数，产生响应内容
    ngx_http_handler_pt               content_handler;

    ngx_uint_t                        access_code;

    // 变量值数组，每个请求都不同
    ngx_http_variable_value_t        *variables;

#if (NGX_PCRE)
    ngx_uint_t                        ncaptures;
    int                              *captures;
    u_char                           *captures_data;
#endif

    // 限速用
    size_t                            limit_rate;

    // 多少字节之后开始限速
    size_t                            limit_rate_after;

    /* used to learn the Apache compatible response length without a header */
    size_t                            header_size;

    // 收到的请求数据总长度，即header+body
    off_t                             request_length;

    ngx_uint_t                        err_status;

    ngx_http_connection_t            *http_connection;
#if (NGX_HTTP_SPDY)
    ngx_http_spdy_stream_t           *spdy_stream;
#endif

    // 记录错误日志时可以调用的函数
    // 在ngx_http_log_error里调用
    ngx_http_log_handler_pt           log_handler;

    // 清理结构体链表，结束时会逐个调用
    // 与内存池的清理调用时机不同
    ngx_http_cleanup_t               *cleanup;

    // 子请求数量，最多不能超过200个
    unsigned                          subrequests:8;

    // 引用计数，丢弃/读取请求体/发起子请求都会增加
    // 表示当前请求有其他关联的操作，不能随意关闭
    // 在http_close里会检查count，如果大于1只减少，不会真正关闭
    unsigned                          count:8;

    // 请求的阻塞数量，用于线程池，当发起一个多线程task时需要增加
    unsigned                          blocked:8;

    unsigned                          aio:1;

    unsigned                          http_state:4;

    /* URI with "/." and on Win32 with "//" */
    unsigned                          complex_uri:1;

    /* URI with "%" */
    unsigned                          quoted_uri:1;

    /* URI with "+" */
    unsigned                          plus_in_uri:1;

    /* URI with " " */
    unsigned                          space_in_uri:1;

    unsigned                          invalid_header:1;

    unsigned                          add_uri_to_alias:1;
    unsigned                          valid_location:1;
    unsigned                          valid_unparsed_uri:1;
    unsigned                          uri_changed:1;
    unsigned                          uri_changes:4;

    unsigned                          request_body_in_single_buf:1;

    // 是否把请求体数据存入文件，与request_body_no_buffering相反
    unsigned                          request_body_in_file_only:1;

    unsigned                          request_body_in_persistent_file:1;
    unsigned                          request_body_in_clean_file:1;
    unsigned                          request_body_file_group_access:1;
    unsigned                          request_body_file_log_level:3;

    // 0-缓存请求体数据
    // 1-不缓存请求体数据
    unsigned                          request_body_no_buffering:1;

    // 要求upstream的数据都在内存里，方便处理
    unsigned                          subrequest_in_memory:1;

    unsigned                          waited:1;

#if (NGX_HTTP_CACHE)
    unsigned                          cached:1;
#endif

#if (NGX_HTTP_GZIP)
    unsigned                          gzip_tested:1;
    unsigned                          gzip_ok:1;
    unsigned                          gzip_vary:1;
#endif

    unsigned                          proxy:1;
    unsigned                          bypass_cache:1;
    unsigned                          no_cache:1;

    /*
     * instead of using the request context data in
     * ngx_http_limit_conn_module and ngx_http_limit_req_module
     * we use the single bits in the request structure
     */
    unsigned                          limit_conn_set:1;
    unsigned                          limit_req_set:1;

#if 0
    unsigned                          cacheable:1;
#endif

    unsigned                          pipeline:1;

    // 两种含义，如果请求头有chunked那么置1，表示请求体长度不确定
    // 如果响应头无content_length_n，那么表示响应体长度不确定，是chunked
    unsigned                          chunked:1;

    // 只有头的标志位
    // ngx_http_header_filter_module里检查，如果方法是head则置1
    unsigned                          header_only:1;

    // 是否keep alive
    unsigned                          keepalive:1;

    // 延后关闭标志
    unsigned                          lingering_close:1;

    // 丢弃请求体的标志，在ngx_http_discard_request_body里设置
    unsigned                          discard_body:1;

    // 正在读取请求体，在ngx_http_read_client_request_body里设置
    unsigned                          reading_body:1;

    // 是否是内部请求，即子请求
    unsigned                          internal:1;

    unsigned                          error_page:1;
    unsigned                          filter_finalize:1;
    unsigned                          post_action:1;
    unsigned                          request_complete:1;
    unsigned                          request_output:1;

    // 是否已经发送了头，如果已经发送则不能再次设置或发送头
    unsigned                          header_sent:1;

    unsigned                          expect_tested:1;
    unsigned                          root_tested:1;
    unsigned                          done:1;
    unsigned                          logged:1;

    unsigned                          buffered:4;

    unsigned                          main_filter_need_in_memory:1;
    unsigned                          filter_need_in_memory:1;
    unsigned                          filter_need_temporary:1;
    unsigned                          allow_ranges:1;
    unsigned                          single_range:1;
    unsigned                          disable_not_modified:1;

#if (NGX_STAT_STUB)
    unsigned                          stat_reading:1;
    unsigned                          stat_writing:1;
#endif

    /* used to parse HTTP headers */

    ngx_uint_t                        state;

    ngx_uint_t                        header_hash;
    ngx_uint_t                        lowcase_index;
    u_char                            lowcase_header[NGX_HTTP_LC_HEADER_LEN];

    u_char                           *header_name_start;
    u_char                           *header_name_end;
    u_char                           *header_start;
    u_char                           *header_end;

    /*
     * a memory that can be reused after parsing a request line
     * via ngx_http_ephemeral_t
     */

    u_char                           *uri_start;
    u_char                           *uri_end;
    u_char                           *uri_ext;
    u_char                           *args_start;
    u_char                           *request_start;
    u_char                           *request_end;
    u_char                           *method_end;
    u_char                           *schema_start;
    u_char                           *schema_end;
    u_char                           *host_start;
    u_char                           *host_end;
    u_char                           *port_start;
    u_char                           *port_end;

    // http的主次版本号
    unsigned                          http_minor:16;
    unsigned                          http_major:16;
};


typedef struct {
    ngx_http_posted_request_t         terminal_posted_request;
} ngx_http_ephemeral_t;


#define ngx_http_ephemeral(r)  (void *) (&r->uri_start)


extern ngx_http_header_t       ngx_http_headers_in[];
extern ngx_http_header_out_t   ngx_http_headers_out[];


#define ngx_http_set_connection_log(c, l)                                     \
                                                                              \
    c->log->file = l->file;                                                   \
    c->log->next = l->next;                                                   \
    c->log->writer = l->writer;                                               \
    c->log->wdata = l->wdata;                                                 \
    if (!(c->log->log_level & NGX_LOG_DEBUG_CONNECTION)) {                    \
        c->log->log_level = l->log_level;                                     \
    }


#define ngx_http_set_log_request(log, r)                                      \
    ((ngx_http_log_ctx_t *) log->data)->current_request = r


#endif /* _NGX_HTTP_REQUEST_H_INCLUDED_ */
