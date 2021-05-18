/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 15:47:44
 * @LastEditTime: 2021-05-15 16:26:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/ThreadPool.cc
 */

#include "ThreadPool.h"
#include "Exception.h"

#include <assert.h>
#include <stdio.h>

ThreadPool::ThreadPool(const std::string& name) 
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(name),
      maxQueueSize_(0),
      running_(false)
{}

ThreadPool::~ThreadPool() {
    if (running_)
        stop();
}

/**
 * @description: 根据传入的传入来创建一个数量确定的线程池 
 * @param {int} numThreads 线程池中线程的数量
 * @return {*}
 */
void ThreadPool::start(int numThreads) {
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    //对线程池中的每一个线程进行初始化
    for (int i = 0; i < numThreads; ++i) {
        char id[32];
        snprintf(id, sizeof(id), "%d", i+1);
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), 
                            name_+id)); //线程的名字：线程池名字+id
        threads_[i]->start();
    }
    if (numThreads == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
}

void ThreadPool::stop() {
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
        notFull_.notifyAll();
    }
    //等待所有的线程执行完
    for (auto& thread : threads_) {
        thread->join();
    }
}

size_t ThreadPool::queueSize() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

/**
 * @description: 向任务队列中添加任务
 * @param {Task} tk 表明需要添加的任务
 * @return {*}
 */
void ThreadPool::run(Task task) {
    if (threads_.empty()) //如果线程池是空的，任务只能由当前线程来执行
        task();
    else {
        MutexLockGuard lock(mutex_);
        while (running_ && isFull())
            notFull_.wait(); //等待任务队列非满
        if (running_ == false) return;
        assert(!isFull());

        queue_.push_back(task);
        notEmpty_.notify();
    }
}

/**
 * @description: 从任务队列中拿出任务，返回得到的任务
 * @param {*}
 * @return {*}
 */
ThreadPool::Task ThreadPool::take() {
    MutexLockGuard lock(mutex_);
    while (running_ && queue_.empty())
        notEmpty_.wait(); //等待任务队列非空
    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        if (maxQueueSize_ > 0) 
            notFull_.notify();
    }
    return task;
}

/**
 * @description: 判断当前的任务队列是否满（如果最大任务数量是0的话表明任务队列是无限大的） 
 * @param {*}
 * @return {*}
 */
bool ThreadPool::isFull() const {
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

/**
 * @description: 线程池中每一个线程都在执行此函数
 * @param {*}
 * @return {*}
 */
void ThreadPool::runInThread() {
    try {
        if (threadInitCallback_) {
            threadInitCallback_(); //首先执行初始任务
        }
        while (running_) {
            Task task(take()); //从任务队列中取出任务来执行
            if (task) {
                task();
            }
        }
    }
    //以下是异常的处理
    catch (const Exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch (const std::exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...) {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}
