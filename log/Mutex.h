/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 10:20:24
 * @LastEditTime: 2021-05-18 15:32:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Mutex.h
 */

#ifndef MUTEX_H
#define MUTEX_H

#include "CurrentThread.h"
#include "noncopyable.h"
#include <assert.h>
#include <pthread.h>


// check pthread return value
#define MCHECK(ret) ({__typeof__ (ret) errnum = (ret); assert(errnum == 0); (void) errnum;})


class MutexLock : noncopyable {
public:
    MutexLock() : holder_(0) {
        //初始化一个互斥量，并将holder_指定为0（表示当前没有线程拥有这个互斥量）
        MCHECK(pthread_mutex_init(&mutex_, NULL));
    }

    ~MutexLock() {
        assert(holder_ == 0); //确保没有线程拥有这个互斥量
        MCHECK(pthread_mutex_destroy(&mutex_));
    }

    bool isLockedByThisThread() const {
        return holder_ == CurrentThread::tid(); //判断是否被当前的线程锁住
    }

    void assertLocked() const {
        assert(isLockedByThisThread());
    }

    void lock() {
        MCHECK(pthread_mutex_lock(&mutex_));
        assignHolder();
    }

    void unlock() {
        unassignHolder();
        MCHECK(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t* getPthreadMutex() {
        return &mutex_;
    }

private:
    friend class Condition;

    /*
    主要是用于在条件变量（condition中的控制）
    因为条件变量在等待的过程中会解锁互斥量，解锁之后应该将hoder_置为0（构造函数的工作）
    当再次上锁的时候再将hoder_置为当前线程的tid（析构函数的工作）
    */
    class UnassignGuard : noncopyable { 
    public:
        explicit UnassignGuard(MutexLock& owner) : owner_(owner) {
            owner_.unassignHolder(); //在构造的时候将holder指定为0
        }

        ~UnassignGuard() {
            owner_.assignHolder(); //在析构的时候将holder指定为当前线程的tid
        }
    private:
        MutexLock& owner_;
    };

    void unassignHolder() {
        holder_ = 0;
    }

    void assignHolder() {
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_; //互斥变量
    pid_t holder_; //将当前线程的tid(在CurrentThread中定义，类型是int
                //一般pid_t的类型也是int)赋值给这个变量（如上assignHolder函数所示）
};


class MutexLockGuard : noncopyable {
public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

#endif
