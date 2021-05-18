/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 10:05:02
 * @LastEditTime: 2021-05-17 11:35:07
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/FileUtil.h
 */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include "StringPiece.h"

#include <sys/types.h>

//文件操作类 -- 主要用于文件的读取操作


//
class ReadSmallFile : noncopyable {
public:
    ReadSmallFile(StringArg filename);
    ~ReadSmallFile();

    //将读取到的数据放在buffer中
    
    int readToBuffer(int* size);
    
    //将读取到的数据转换为string
    template<typename String>
    int readToString(int maxsize, String* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime);

    const char* buffer() const {return buf_;}
    
    static const int kBufferSize = 64 * 1024;
private:
    int fd_; //文件描述符
    int err_; //错误码
    char buf_[kBufferSize]; //定义的是读取文件的缓冲区
};

template<typename String>
int readFile(StringArg filename, int maxSize, String* content,
            int64_t* fileSize = NULL, int64_t* modifyTime = NULL, int64_t* createTime = NULL) {
                
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

//添加数据到指定的文件中 -- 非线程安全
class AppendFile : noncopyable {
public:
    explicit AppendFile(StringArg filename);
    ~AppendFile();

    void append(const char* logline, size_t len);

    void flush();

    off_t writenBytes() const {return writenBytes_;}

private:
    size_t write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[64*1024]; //缓冲区64KB
    off_t writenBytes_; //记录已经写了多少字节
};

#endif