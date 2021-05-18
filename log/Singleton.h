/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 14:17:08
 * @LastEditTime: 2021-05-15 14:43:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Singleton.h
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include "noncopyable.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

template<typename T>
class Singleton : noncopyable {
public:
    Singleton()=delete;
    ~Singleton()=delete;

    static T& instance() {
        pthread_once(&ponce_, &Singleton::init); //保证多线程下的安全性
        assert(value_ != NULL);
        return *value_;
    }

private:
    static void init() {
        assert(value_ == NULL);
        value_ = new T();
        ::atexit(destory); //在退出的时候执行destroy函数
    }

    static void destory() {
        delete value_;
        value_ = NULL;
    }
    static pthread_once_t ponce_; //保证多线程环境下的安全初始化
    static T* value_;
};

//定义静态变量
template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;


#endif
