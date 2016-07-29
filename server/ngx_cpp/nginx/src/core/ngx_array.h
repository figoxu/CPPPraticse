// annotated by chrono since 2016

/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


// nginx的动态数组，表示一块连续的内存，其中顺序存放着数组元素，概念上和原始数组很接近
// 如果数组内的元素不断增加，当nelts > nalloc时将会引发数组扩容
// 可对比std::vector
typedef struct {
    void        *elts;      //数组的内存位置，即数组首地址
    ngx_uint_t   nelts;     //数组当前的元素数量
    size_t       size;      //数组元素的大小
    ngx_uint_t   nalloc;    //数组可容纳的最多元素数量
    ngx_pool_t  *pool;      //数组使用的内存池
} ngx_array_t;


// 使用内存池创建一个可容纳n个大小为size元素的数组，即分配了一块n*size大小的内存块
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);

// “销毁”动态数组，归还分配的内存
void ngx_array_destroy(ngx_array_t *a);

// 向数组添加元素，用法比较特别，它们返回的是一个void*指针，用户必须把它转换为真正的元素类型再操作
// 不直接使用ngx_array_t.elts操作的原因是防止数组越界，函数内部会检查当前数组容量自动扩容
void *ngx_array_push(ngx_array_t *a);
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);


static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */

    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
