/*
 * @Author: tian ref muduo
 * @Date: 2021-05-18 14:23:42
 * @LastEditTime: 2021-05-18 14:44:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/AsyncLogging.h
 */

#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"

#include <atomic>
#include <vector>

/*
高效的多线程异步日志类 -- 日志线程负责将收到的日志数据写入文件
*/

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string& basename, const off_t rollSize, const int flushInterval = 3);
    ~AsyncLogging() {
        if (running_)
            stop();
    }

    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait(); //等待子线程先执行
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }
private:
    void threadFunc();

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector; //使用智能指针来管理buffer
    typedef BufferVector::value_type BufferPtr;

    const int flushInterval_; //冲刷的时间间隔
    std::atomic<bool> running_; //是否运行的标志位
    const std::string basename_; //异步日志类的名字
    const off_t rollSize_; //滚动的临界区大小
    Thread thread_; //负责将缓冲区数据写入到日志文件中的线程
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_; //指向的是当前正在操作的缓冲区
    BufferPtr nextBuffer_; //指向的是下一个可以操作的缓冲区
    BufferVector buffers_; //缓冲区的集合
};

#endif