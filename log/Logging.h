/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 14:28:37
 * @LastEditTime: 2021-05-18 17:28:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Logging.h
 */

#ifndef LOGGING_H
#define LOGGING_H

#include "LogStream.h"
#include "Timestamp.h"

class TimeZone;

class Logger {
public:
    enum LogLevel {//标识了日志的等级
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS  
    };
    //计算日志文件的名字
    class SourceFile {
    public:
        template<int N>
        SourceFile(const char (&arr)[N]) : data_(arr), size_(N-1) {
            const char* slash = strrchr(data_, '/');
            if (slash) {
                data_ = slash+1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = strrchr(data_, '/');
            if (slash) {
                data_ = slash+1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data() const {return data_;}
        int size() const {return size_;}

    private:
        const char* data_;
        int size_;  
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line,LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() {return impl_.stream_;}
    
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    
    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setDefaultOutput();
    static void setFlush(FlushFunc);
    static void setTimeZone(const TimeZone& tz); //设置时区

private:
    class Impl {
    public:
        typedef Logger::LogLevel LogLevel;
        typedef Logger::SourceFile SourceFile;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_; //记录时间
        LogStream stream_; //用于日志的缓冲
        LogLevel level_; //标识日志的等级
        int line_; //用于标识行数
        SourceFile basename_; //用来表示文件名（日志文件）
    };
    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {
    return g_logLevel;
}

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE)  \
    Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG)  \
    Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO)    \
    Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

#endif
