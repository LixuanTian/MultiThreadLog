/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 16:49:31
 * @LastEditTime: 2021-05-18 17:28:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Logging.cc
 */

#include "Logging.h"

#include "CurrentThread.h"
#include "Timestamp.h"
#include "TimeZone.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

//定义一些线程安全的变量
__thread char t_errnobuf[512]; //存放出错的消息
__thread char t_time[64]; //记录时间信息
__thread time_t t_lastSecond; //记录上一次修改的时间

//有的错误并不需要退出程序，将相应的错误消息输出即可
const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf); //将错误号对应的消息存储到t_errnobuf并返回
}

//定义全局日志等级
Logger::LogLevel g_logLevel = Logger::INFO;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
    s.append(v.data(), v.size());
    return s;
}

void defaultOutput(const char* msg, int len) {
    fwrite(msg, 1, len, stdout); //默认输出到标准输出上 -- fwrite函数是线程安全的
}

void defaultFlush() {
    ::fflush(stdout); //默认冲刷标准输出
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;
TimeZone g_logTimeZone;

/**
 * @description: 用于把当前的时间信息（指定的时区下）输出到日志的缓存中
 * @param {*}
 * @return {*}
 */
void Logger::Impl::formatTime() {
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond) {//当前的时间和上一次的时间不同
        t_lastSecond = seconds;
        struct tm tm_time;
        if (g_logTimeZone.valid()) {//如果指定了时区
            tm_time = g_logTimeZone.toLocalTime(seconds);
        } else {
            ::gmtime_r(&seconds, &tm_time); //使用标准时间(将日历时间转换为utc表示的时间)
        }

        //将当前获得到的时间信息存储在t_time（线程安全的时间变量）中（前提是当前的时间相对于上一次的时间是不同的）
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        (void)len;
    }
    
    if (g_logTimeZone.valid()) {
        Fmt us(".%06d ", microseconds);
        stream_ << StringPiece(t_time, 17) << StringPiece(us.data(), us.length()); //将对应的时间信息添加到日志的缓存中
    } else {
        Fmt us(".%06dZ ", microseconds);
        stream_ << StringPiece(t_time, 17) << StringPiece(us.data(), us.length());
    }
}

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
    : time_(Timestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file) {

    formatTime();
    CurrentThread::tid(); //获得线程的tid
    stream_ << StringPiece(CurrentThread::tidString(), CurrentThread::tidStringLength()) << " "; //输出当前线程的有关信息
    stream_ << StringPiece(LogLevelName[level], 6); //输出日志的级别信息
    if (savedErrno != 0)
        stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
}

void Logger::Impl::finish() {
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line) 
    : impl_(INFO, 0, file, line)
{}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
    : impl_(level, 0, file, line)
{
    impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
    : impl_(level, 0, file, line)
{}

Logger::Logger(SourceFile file, int line, bool toAbort)
    : impl_(toAbort?FATAL:ERROR, errno, file, line)
{}

Logger::~Logger() {
    impl_.finish();
    g_output(stream().buffer().data(), stream().buffer().length());
    if (impl_.level_ == FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level) {
    g_logLevel = level;
}

void Logger::setFlush(FlushFunc flush) {
    g_flush = flush;
}

void Logger::setOutput(OutputFunc output) {
    g_output = output;
}

void Logger::setDefaultOutput() {
    g_output = defaultOutput;
}

void Logger::setTimeZone(const TimeZone& tz) {
    g_logTimeZone = tz;
}