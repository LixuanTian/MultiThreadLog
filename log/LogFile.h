/*
 * @Author: tian ref muduo
 * @Date: 2021-05-18 10:14:22
 * @LastEditTime: 2021-05-18 11:03:39
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/LogFile.h
 */

#ifndef LOGFILE_H
#define LOGFILE_H

#include "Mutex.h"
#include "Types.h"


#include <memory>

class AppendFile;

class LogFile : noncopyable {
public:
    LogFile(const std::string& basename,
            off_t rollSize,
            bool threadSafe = true, //默认是线程安全的
            int flushInterval = 3,
            int checkEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();
    bool isNeedRollFile();
private:
    void append_unlock(const char* logline, int len);

    static string getLogFileName(const string& basename, time_t* now);

    const std::string basename_; //日志文件名
    const off_t rollSize_; //日志文件到达rollSize_就更换一个新的日志文件
    const int flushInterval_; //日志写入间隔时间
    const int checkEveryN_;

    int count_; //当它等于checkEveryN_的时候，会检测是否需要更换一个新的日志文件，即是否达到滚动条件

    std::unique_ptr<MutexLock> mutex_; //互斥量
    time_t startOfPeriod_; //开始记录日志的时间
    time_t lastRoll_; //上一次滚动日志的时间
    time_t lastFlush_; //上一次日志写入文件的时间
    std::unique_ptr<AppendFile> file_;

    static const int kRollPerSeconds_ = 60*60*24; //一天的秒数
};

#endif
