/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 14:49:03
 * @LastEditTime: 2021-05-15 15:12:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/ThreadLocalSingleton.h
 */

#ifndef THREADLOCALSINGLETON_H
#define THREADLOCALSINGLETON_H

#include "noncopyable.h"

#include <assert.h>
#include <pthread.h>

//单例线程局部变量对象 - 每一个线程内部的一个单例数据

template<typename T>
class ThreadLocalSingleton : noncopyable {
public:
    ThreadLocalSingleton()=delete;
    ~ThreadLocalSingleton()=delete;

    static T& instance() {
        if (!t_value_) {
            t_value_ = new T();
            deleter_.set(t_value_);
        }
        return *t_value_;
    }

    static T* pointer() {
        return t_value_;
    }

private:
    static void destructor(void* obj) {
        assert(obj == t_value_);
        delete t_value_;
        t_value_ = NULL;
    }

    class Deleter { //确保数据在销毁的时候能够正确的释放内存
    public:
        Deleter() {
            pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
        }
        ~Deleter() {
            pthread_key_delete(pkey_);
        }

        void set(T* newobj) {
            assert(pthread_getspecific(pkey_) == NULL);
            pthread_setspecific(pkey_, newobj);
        }
    private:
        pthread_key_t pkey_;
    };
    static __thread T* t_value_; //与Singleton相比，此处通过使用__thread来修饰可以保证线程的安全性
                                 //进而不需要使用pthread_once_t变量来保证线程的安全性
    static Deleter deleter_;
};

//定义静态变量
template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

#endif