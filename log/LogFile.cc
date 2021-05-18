/*
 * @Author: your name
 * @Date: 2021-05-18 10:27:56
 * @LastEditTime: 2021-05-18 11:20:41
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/LogFile.cc
 */

#include "LogFile.h"
#include "FileUtil.h"

#include "ProcessInfo.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

/**
 * @description: 获得一个新的日志文件名
 * @param {const} string
 * @return {*}
 */
std::string LogFile::getLogFileName(const string& basename, time_t* now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += hostname(); //获得主机名
    
    char pidbuf[32];
    snprintf(pidbuf, sizeof(pidbuf), ".%d", pid()); //获取进程号
    filename += pidbuf;
    
    filename += ".log";
    
    return filename;
}

/**
 * @description: 更换一个新的日志文件（此处只是判断时间是否相同，其他的逻辑放在append函数中）
 * @param {*}
 * @return {*}
 */
bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_) { //时间不同的话就创建新的日志文件
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename));
        return true; //成功创建新的文件
    }
    return false;
}


LogFile::LogFile(const std::string& basename,
                off_t rollSize,
                bool threadSafe, //默认是线程安全的
                int flushInterval,
                int checkEveryN)
    : basename_(basename), rollSize_(rollSize),
      flushInterval_(flushInterval),
      checkEveryN_(checkEveryN),
      count_(0),
      mutex_(threadSafe? new MutexLock : NULL),
      startOfPeriod_(0),
      lastRoll_(0),
      lastFlush_(0)
{
    rollFile();
}

LogFile::~LogFile() = default;

/**
 * @description: 根据是否加锁这个条件来添加数据 
 * @param {const} char
 * @param {int} len
 * @return {*}
 */
void LogFile::append(const char* logline, int len) {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        append_unlock(logline, len);
    } else 
        append_unlock(logline, len);
}

/**
 * @description: 判断是否需要更换新的文件 
 *  更换新的日志文件的条件
 *      1.当前的文件大小超过了rollSize_的大小
 *      2.当前的时间和当前日志开始的时间不是同一天
 *  刷新日志文件的条件
 *      距离上次刷新的时间超过flushInterval_
 * @param {*}
 * @return {*}
 */
bool LogFile::isNeedRollFile() {
    if (file_->writenBytes() > rollSize_) return true;
    else if (++count_ >= checkEveryN_) {
        count_ = 0;
        time_t now = ::time(NULL);
        time_t curPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
        if (curPeriod != startOfPeriod_) //判断是否是同一天
            return true;
        else if (now - lastFlush_ > flushInterval_) {
            lastFlush_ = now;
            file_->flush();
        }
    }
    return false;
}

/**
 * @description: 无锁情况下的添加数据操作
 * @param {const} char
 * @param {int} len
 * @return {*}
 */
void LogFile::append_unlock(const char* logline, int len) {
    file_->append(logline, len);
    if (isNeedRollFile()) rollFile();
}

void LogFile::flush() {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    } else 
        file_->flush();
}

