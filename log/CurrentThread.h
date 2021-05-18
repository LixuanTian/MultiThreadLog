/*
 * @Author: Tian ref muduo
 * @Date: 2021-05-14 10:33:05
 * @LastEditTime: 2021-05-14 11:25:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/CurrentThread.h
 */

#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include "Types.h"

/*
当前线程(命名空间，不是类)
主要是用来提供给异常类和线程类使用的，异常类需要stackTrace()看函数栈的信息
线程类需要其他的一些功能
*/

namespace CurrentThread {
    /*
    __thread 是GCC内置的线程局部存储设施，_thread变量每一个线程有一份独立实体，各个线程的值互不干扰
    */
    extern __thread int t_cachedTid; //用此变量缓存线程的标识
    extern __thread char t_tidString[32]; //上述标识的字符串形式
    extern __thread int t_tidStringLength; //上述字符串的长度
    extern __thread const char* t_threadName; //线程的名字
    void cacheTid(); // 具体的实现在Thread.cc文件中
   
   //获得当前线程的tid
    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid(); //可以使得线程第一次调用的时候来获取线程标识，然后缓存，之后调用都是直接获取
        }
        return t_cachedTid;
    }
 
    inline const char* tidString() { //为之后的日志使用
        return t_tidString;
    }

    inline int tidStringLength() { //为之后的日志使用
        return t_tidStringLength;
    }

    inline const char* name() {
        return t_threadName;
    }

    bool isMainThread();
    
    void sleepUsec(int64_t usec);

    string stackTrace(bool demangle); //异常类中使用，主要用来返回函数栈信息
}


#endif
