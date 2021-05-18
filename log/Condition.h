/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 11:01:50
 * @LastEditTime: 2021-05-15 12:33:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Condition.h
 */


#ifndef CONDITION_H
#define CONDITION_H

#include "Mutex.h"

#include <pthread.h>

class Condition : noncopyable {
public:
    explicit Condition(MutexLock& mutex) : mutex_(mutex) {
        MCHECK(pthread_cond_init(&pcond_, NULL));
    }

    ~Condition() {
        MCHECK(pthread_cond_destroy(&pcond_));
    }

    void wait() {
        MutexLock::UnassignGuard ug(mutex_);
        MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
    }

    void notify() {
        MCHECK(pthread_cond_signal(&pcond_));
    }

    void notifyAll() {
        MCHECK(pthread_cond_broadcast(&pcond_));
    }

    bool waitForSeconds(double seconds);

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};



#endif