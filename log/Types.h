/*
 * @Author: Tian reference muduo
 * @Date: 2021-05-14 08:52:34
 * @LastEditTime: 2021-05-14 10:20:53
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Types.h
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <assert.h>

using std::string;

/**
 * @description: 用于将给定指针位置的内存置零
 * @param {size_t} n 指定需要操作的大小
 * @return {*}
 */
inline void memZero(void* p, size_t n) {
    memset(p, 0, n);
}

/**
 * @description: 用于隐式类型转换
 * @param {*}
 * @return {*}
 */
template<typename To, typename From>
inline To implicit_cast(const From& f) {
    return f;
}

/**
 * @description: use like this: down_cast<T*>(foo);
 * only accept pointers
 * @param {*}
 * @return {*}
 */
template<typename To, typename From>
inline To down_cast(From* f) {
    if (false) {
        implicit_cast<From*, To>(0);
    }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f); 
}


#endif