/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 11:19:41
 * @LastEditTime: 2021-05-15 11:25:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/CountDownLatch.cc
 */


#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
    : mutex_(), condition_(mutex_),count_(count)
{}


void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) { //等待，直到count_变为零
        condition_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll(); //通知所有正在等待的线程
    }
}

int CountDownLatch::getCount() const { //因为mutex_是mutable，故此处可以在const函数内部使用
    MutexLockGuard lock(mutex_);
    return count_; //返回计数值
}