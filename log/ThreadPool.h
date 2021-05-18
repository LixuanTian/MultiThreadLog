/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 15:32:46
 * @LastEditTime: 2021-05-15 16:25:56
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/ThreadPool.h
 */


#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "Types.h"

#include <deque>
#include <vector>

/*
实现了一个简单的线程池
*/

class ThreadPool : noncopyable {
public:
    typedef std::function<void()> Task;
    
    explicit ThreadPool(const std::string& name = std::string("ThreadPool"));
    ~ThreadPool();

    //以下函数需要在调用start之前调用
    void setMaxQueueSize(int maxSize) {maxQueueSize_ = maxSize;}
    void setThreadInitCallback(const Task& cb) {
        threadInitCallback_ = cb;
    }

    void start(int numThreads);
    void stop();
    const std::string& name() const {return name_;}

    size_t queueSize() const;

    void run(Task task);

private:
    bool isFull() const;
    void runInThread();
    Task take();
    
    mutable MutexLock mutex_;
    Condition notEmpty_; //判断不为空的条件变量
    Condition notFull_; //判断不为满的条件变量
    std::string name_; //线程池的名字
    Task threadInitCallback_; //线程池启动的时候调用的回调函数
    std::vector<std::unique_ptr<Thread>> threads_; //线程池
    std::deque<Task> queue_; //任务队列
    size_t maxQueueSize_; //最大的任务数量，为零的话表示任务数量不受限制
    bool running_; //是否有线程在运行
};

#endif