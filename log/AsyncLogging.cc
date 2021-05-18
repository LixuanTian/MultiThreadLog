/*
 * @Author: tian ref muduo
 * @Date: 2021-05-18 14:30:55
 * @LastEditTime: 2021-05-18 15:49:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/AsyncLogging.cc
 */

#include "AsyncLogging.h"
#include "LogFile.h"
#include "Timestamp.h"

#include <stdio.h>

AsyncLogging::AsyncLogging(const std::string& basename, const off_t rollSize, const int flushInterval)
    : flushInterval_(flushInterval),
      running_(false),
      basename_(basename),
      rollSize_(rollSize),
      thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
      latch_(1),
      mutex_(),
      cond_(mutex_),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_()
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16); //初始化为16个缓冲区
}

/**
 * @description: 业务线程通过此函数将产生的日志消息添加到日志缓冲区中，每当写满一个缓冲区时即通知日志线程进行写入操作
 * @param {const} char
 * @param {int} len
 * @return {*}
 */
void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_); //保证多线程写的时候不出错
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logline, len);
    } else {
        buffers_.push_back(std::move(currentBuffer_));
        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_); //执行完这条语句之后，nextBuffer_此时就是空指针了（nullptr）
        } else {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify(); //通知日志写线程
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown(); //使得等待的线程运行
    //负责管理日志文件的变量，因为只有一个线程负责写入，所以LogFile不用保证线程的安全性性
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer); //使用以上的两个变量来交换currentBuffer_和nextBuffer_
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_) {
        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty()) {
                cond_.waitForSeconds(flushInterval_);
            }
            //将主程序中正在使用的buffer添加到容器中
            buffers_.push_back(std::move(currentBuffer_));
            //更换主程序中的当前buffer
            currentBuffer_ = std::move(newBuffer1);
            //使用新的（空的）buffersToWrite和主程序中的buffers_进行交换
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                //如果主程序中的nextBuffer_为null则使用newBuffer2代替
                nextBuffer_ = std::move(newBuffer2);
            }
        }
        assert(!buffersToWrite.empty());
        /*
        如果将要写入文件的buffer列表中buffer的个数大于25，那么将多余数据删除  
        消息堆积
        前端陷入死循环，拼命发送日志消息，超过后端的处理能力
        这是典型的生产速度超过消费速度，会造成数据在内存中的堆积
        严重时引发性能问题(可用内存不足),
        或程序崩溃(分配内存失败)
        正常情况下不会运行
        */
        if (buffersToWrite.size() > 25) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log messages at %s, %zd larger buffers\n",
                        Timestamp::now().toFormattedString().c_str(),
                        buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for (const auto& buffer : buffersToWrite) {
            output.append(buffer->data(), buffer->length());
        }

        if (buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }

        //前台buffer是由newBuffer1 2 归还的。现在把buffersToWrite的buffer归还给后台buffer
        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        //如果newBuffer2为空的话，说明了newBuffer2已经被交换出去了，即表明主线程中的nextBuffer_为空
        //可以推断出此时的buffersToWrite容量是大于等于二的
        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
