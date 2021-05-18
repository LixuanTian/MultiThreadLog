/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 11:26:49
 * @LastEditTime: 2021-05-15 13:07:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Thread.h
 */

#ifndef THREAD_H
#define THREAD_H

#include "Atomic.h"
#include "CountDownLatch.h"
#include "Types.h"

#include <functional>
#include <memory>
#include <pthread.h>

//线程类

class Thread : noncopyable {
public:
    typedef std::function<void()> ThreadFunc;
    
    explicit Thread(ThreadFunc, const std::string& name = std::string());

    ~Thread();

    void start();
    int join();
    
    bool started() const {return started_;}
    
    pid_t tid() const {return threadData_.tid_;}
    const std::string& name() const {return threadData_.name_;}

    void runInThread();
    
    static int numCreated() {return numCreated_.get();}

private:
    void setDefaultName();

    

    bool started_; //标识线程是否开始执行
    bool joined_; //表示线程是否是可以join的
    pthread_t pthreadId_; //线程的id（注意和tid的区别）

    struct ThreadData { //记录线程中的一些特殊的数据
        pid_t tid_; //线程的tid
        ThreadFunc func_; //线程的调用函数
        std::string name_;
        CountDownLatch latch_; //用于线程之间的同步
        ThreadData(pid_t p, ThreadFunc func, std::string n)
            : tid_(p), func_(std::move(func)), name_(n), latch_(1) 
        {}
    };
    ThreadData threadData_;

    static AtomicInt32 numCreated_; //记录创建的线程的数量
};

#endif