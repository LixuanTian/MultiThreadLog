/*
 * @Author: tian ref muduo
 * @Date: 2021-05-15 11:38:45
 * @LastEditTime: 2021-05-15 13:05:58
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Thread.cc
 */

#include "Thread.h"
#include "CurrentThread.h"
#include "Exception.h"
#include "Timestamp.h"

#include <type_traits>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork() {
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
}

//用于对主线程进行规范化
class ThreadNameInitializer {
public:
    ThreadNameInitializer() {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init; //创建主线程信息（主线程是在程序启动的时候就创建了，此处是为了将我们添加的信息与主线程绑定）

void Thread::runInThread() {
    threadData_.tid_ = CurrentThread::tid();
    threadData_.latch_.countDown(); //子线程开始执行后通知父线程可以开始运行了

    CurrentThread::t_threadName = threadData_.name_.empty()? "Thread" : threadData_.name_.c_str();
    ::prctl(PR_SET_NAME, CurrentThread::t_threadName);
    try
    {
        threadData_.func_();
        CurrentThread::t_threadName = "finished";
    }
    catch (const Exception& ex)
    {
        CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "exception caught in Thread %s\n", threadData_.name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch (const std::exception& ex)
    {
        CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "exception caught in Thread %s\n", threadData_.name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "unknown exception caught in Thread %s\n", threadData_.name_.c_str());
        throw; // rethrow
    }
}

void* startThread(void* obj) {
    Thread* thread = static_cast<Thread*>(obj);
    thread->runInThread();
    return NULL;
}


void CurrentThread::cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
    }
}

bool CurrentThread::isMainThread() {
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = { 0, 0 };
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const string& n) 
    : started_(false),
      joined_(false),
      pthreadId_(0),
      threadData_(0, func, n) {
    
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) { //在线程start后，但是没有调用join的情况下，会把子线程分离
        pthread_detach(pthreadId_);
    }
}

//设置线程的默认名字：Thread + 当前的线程数量
void Thread::setDefaultName() {
    int num = numCreated_.incrementAndGet();
    if (threadData_.name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        threadData_.name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    if (pthread_create(&pthreadId_, NULL, &startThread, this)) {
        started_ = false;
    } else {
        threadData_.latch_.wait(); //等待子线程开始执行
        assert(threadData_.tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}
