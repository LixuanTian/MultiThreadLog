/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 10:14:30
 * @LastEditTime: 2021-05-17 14:28:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/FileUtil.cc
 */


#include "FileUtil.h"
#include "Logging.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

ReadSmallFile::ReadSmallFile(StringArg filename) 
    : fd_(open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
      err_(0) {
        
    buf_[0] = '0';
    if (fd_ < 0)
        err_ = errno;
}

ReadSmallFile::~ReadSmallFile() {
    if (fd_ >= 0) {
        ::close(fd_); //关闭关联的文件
    }
}

/**
 * @description: 读取数据到buffer缓冲区，并返回读取到的字节数（size） 
 * @param {*}
 * @return {*}
 */
int ReadSmallFile::readToBuffer(int* size) {
    int err = err_;
    if (fd_ < 0) return err;
    
    ssize_t n;
    //使用pthread解决了多线程读的情况下的线程安全问题
    if ((n = ::pread(fd_, buf_, sizeof(buf_)-1, 0)) >= 0) {
        if (size)
            *size = static_cast<int>(n);
        buf_[n] = '\0';
    } else 
        err = errno;
    return err;
}

/**
 * @description: 读取数据到给定的对象中（content）
 * @param {*}
 * @return {*}
 */
template<typename String>
int ReadSmallFile::readToString(int maxSize, String* content, int64_t* fileSize, 
                                int64_t* modifyTime, int64_t* createTime) {

    assert(content != NULL);
    int err = err_;
    if (fd_ < 0) return err;

    content->clear();
    if (fileSize) {
        struct stat statbuf;
        //获取文件的相关信息
        if (::fstat(fd_, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                *fileSize = statbuf.st_size;
                content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
            } else if (S_ISDIR(statbuf.st_mode)) {
                err = EISDIR;
            }
            if (modifyTime) 
                *modifyTime = statbuf.st_mtime;
            if (createTime)
                *createTime = statbuf.st_ctime;
        } else {
            err = errno;
        }
    }
    
    while (content->size() < static_cast<size_t>(maxSize)) {
        size_t toRead = std::min(static_cast<size_t>(maxSize) - content->size(), sizeof(buf_));
        //先将数据读取到buffer，然后在拷贝到目的地
        ssize_t n;
        if ((n = ::read(fd_, buf_, toRead)) > 0) {
            content->append(buf_, n);
        } else if (n <= 0) {
            if (n < 0) err = errno;
            break;
        }
    }
    return err;
}

//模板特化
template int readFile(StringArg filename,
                                int maxSize,
                                string* content,
                                int64_t*, int64_t*, int64_t*);

template int ReadSmallFile::readToString(
    int maxSize,
    string* content,
    int64_t*, int64_t*, int64_t*);

AppendFile::AppendFile(StringArg filename)
   : fp_(::fopen(filename.c_str(), "ae")),
     writenBytes_(0) {

    assert(fp_); //判断文件是否打开
    ::setbuffer(fp_, buffer_, sizeof(buffer_)); //设置打开文件的缓冲区
}

AppendFile::~AppendFile() {
    ::fclose(fp_);
}

/**
 * @description: 将数据写入关联的文件中（非线程安全）
 * @param {const} char
 * @param {const size_t} len
 * @return {*}
 */
size_t AppendFile::write(const char* logline, const size_t len) {
    return ::fwrite_unlocked(logline, 1, len, fp_); //写操作，非线程安全（不需要加锁）
}

/**
 * @description: 添加指定的信息到文件中
 * @param {const} char
 * @param {const size_t} len
 * @return {*}
 */
void AppendFile::append(const char* logline, const size_t len) {
    size_t written = 0; //记录写了多少字节的数据
    
    while (written != len) {
        size_t remain = len - written;
        size_t n;
        if ((n = write(logline+written, remain)) != remain) {
            int err = ferror(fp_); //测试当前的文件描述符是否出错
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
                break;
            }
        }
        written += n;
    }
    writenBytes_ += len;
}

void AppendFile::flush() {
    ::fflush(fp_);
}

