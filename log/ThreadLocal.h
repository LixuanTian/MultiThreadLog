/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 13:42:43
 * @LastEditTime: 2021-05-15 14:13:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/ThreadLocal.h
 */

#ifndef THREADLOCAL_H
#define THREADLOCAL_H

#include "Mutex.h"
#include "noncopyable.h"

#include <pthread.h>

//线程局部变量封装类
template<typename T>
class ThreadLocal : noncopyable {
public:
    ThreadLocal() {
        MCHECK(pthread_key_create(&pkey_, &ThreadLocal::destructor));
    }

    ~ThreadLocal() {
        MCHECK(pthread_key_delete(pkey_));
    }

    //返回关联数据的引用
    T& value() {
        T* threadValue = static_cast<T*>(pthread_getspecific(pkey_)); //获取线程局部变量的值
        if (!threadValue) {
            T* newObj = new T();
            MCHECK(pthread_setspecific(pkey_, newObj));
            threadValue = newObj;
        }
        return *threadValue;
    }

private:
    static void destructor(void* p) { //负责销毁局部变量
        T* obj = static_cast<T*> (p);
        delete obj;
    }
    pthread_key_t pkey_;
};

#endif