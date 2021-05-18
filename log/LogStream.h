/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 14:32:48
 * @LastEditTime: 2021-05-17 21:46:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/LogStream.h
 */

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "noncopyable.h"
#include "StringPiece.h"
#include "Types.h"
#include <assert.h>
#include <string.h>

const int kSmallBuffer = 4000; //小缓冲区
const int kLargeBuffer = 4000 * 1000; //大缓冲区

//固定大小的缓冲区
template<int SIZE>
class FixedBuffer : noncopyable {
public:
    FixedBuffer() : cur_(data_) {}
    ~FixedBuffer() {}
    
    //返回缓冲区的尾指针
    const char* end() const {return data_ + sizeof(data_);}
    //返回缓冲区中的剩余容量
    int avail() const {return static_cast<int>(end()-cur_);}
    //返回数据指针
    const char* data() const {return data_;}
    //返回缓冲区中已经写入的数据的长度
    int length() const {return static_cast<int>(cur_ - data_);}
    //返回当前位置的指针(以供直接写入)
    char* current() {return cur_;}
    //将当前指针向前移动
    void add(size_t len) {cur_ += len;}

    void reset() {cur_ = data_;} //重置
    void bzero() {memZero(data_, sizeof(data_)); cur_ = data_;}

    //添加数据 -- 只在存在足够的空间时添加
    void append(const char* buf, size_t len) {
        size_t remain = static_cast<size_t>(avail());
        if (remain > len) {
            strcpy(cur_, buf);
            add(len);
        }
    }

    //以下函数是用作测试例子中的
    string toString() const {return std::string(data_, length());}
    StringPiece toStringPiece() const {return StringPiece(data_);}

private:
    char data_[SIZE]; //缓冲区
    char* cur_; //缓冲区中的偏移量
};

//日志流
class LogStream : noncopyable {
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;    

    //以下是根据对应的数据类型的输出操作符的重载函数

    LogStream& operator<<(bool b) { //布尔类型的输出
        buffer_.append(b?"1":"0", 1);
        return *this;
    }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    //输出指针类型的信息 -- 输出对应的地址信息
    LogStream& operator<<(const void*);
    
    LogStream& operator<<(double);

    LogStream& operator<<(float f) {
        *this << static_cast<double>(f);
        return *this;
    }
    
    LogStream& operator<<(const char ch) {
        buffer_.append(&ch, 1);
        return *this;
    }

    LogStream& operator<<(const char* str) {
        if (str) {
            buffer_.append(str, strlen(str));
        } else {
            buffer_.append("(null)", 6);
        }
        return *this;
    }

    LogStream& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& str) {
        buffer_.append(str.c_str(), str.size());
        return *this;
    }

    LogStream& operator<<(const StringPiece& strp) {
        buffer_.append(strp.data(), strp.size());
        return *this;
    }

    //将另一个Buffer中的数据添加到本Buffer中
    LogStream& operator<<(const Buffer& b) {
        buffer_.append(b.data(), b.length());
        return *this;
    }

    //直接向缓冲区中添加数据
    void append(const char* data, int len) {buffer_.append(data, len);}
    const Buffer& buffer() const {return buffer_;}
    void resetBuffer() {buffer_.reset();}
private:
    
    template<typename T>
    void formatInteger(T); //格式化整数

    Buffer buffer_; //每一个日志流都有一个缓冲区，将数据写入到缓冲区
    static const int kMaxNumericSize = 32; //将一个数字转换为字符串可能占用的最大的字符串长度
};

//将数据转换为指定的格式存储
class Fmt {
public:
    template<typename T>
    Fmt(const char* fmt, T val);

    const char* data() const {return buf_;}
    int length() const {return length_;}

private:
    char buf_[32];
    int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt) {
    s.append(fmt.data(), fmt.length());
    return s;
}

#endif