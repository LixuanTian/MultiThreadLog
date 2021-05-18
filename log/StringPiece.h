/*
 * @Author: tian ref muduo
 * @Date: 2021-05-17 09:23:37
 * @LastEditTime: 2021-05-18 11:26:55
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/StringPiece.h
 */


#ifndef STRINGPIECE_H
#define STRINGPIECE_H

#include <string.h>
#include <iosfwd>

#include "Types.h"

//c string的一个封装类
class StringArg {
public:
    StringArg(const char* str) : str_(str) {}
    StringArg(const std::string& str) : str_(str.c_str()) {}
    ~StringArg() = default;

    const char* c_str() {return str_;}
private:
    const char* str_;
};

//对底层的字符串的包裹类（不拥有字符串本身）
class StringPiece {
public:
    //构造函数
    StringPiece() : ptr_(NULL), length_(0) {}
    StringPiece(const char* str) : ptr_(str), length_(static_cast<int>(strlen(ptr_))) {}
    StringPiece(const unsigned char* str) : ptr_(reinterpret_cast<const char*>(str)),
                                            length_(static_cast<int>(strlen(ptr_))) {}
    StringPiece(const std::string& str) : ptr_(str.c_str()), length_(static_cast<int>(strlen(ptr_))) {}
    StringPiece(const char* offset, int len) : ptr_(offset), length_(len) {}

    //以下提供一些字符串的操作函数
    const char* data() const {return ptr_;}
    int size() const {return length_;}
    bool empty() const {return length_ == 0;}
    const char* begin() const {return ptr_;}
    const char* end() const {return ptr_ + length_;}
    
    //因为StringPiece类不拥有数据，只是数据的一个引用类，故释放的时候直接将指针置零即可
    void clear() {ptr_ = NULL; length_ = 0;}
    void set(const char* buffer, int len) {ptr_ = buffer; length_ = len;}
    
    void set(const char* buffer) {
        ptr_ = buffer;
        length_ = static_cast<int>(strlen(ptr_));
    }

    void set(void* buffer, int len) {
        ptr_ = reinterpret_cast<const char*>(buffer);
        length_ = len;
    }

    char operator[](int index) const {return *(ptr_+index);}

    void remove_prefix(int n) {//取出前缀
        ptr_ += n;
        length_ -= n;
    }

    void remove_suffix(int n) {//去除后缀
        length_ -= n;
    }

    bool operator==(const StringPiece& s) const {
        return (length_ == s.length_) && (memcmp(ptr_, s.ptr_, length_) == 0);
    }

    bool operator!=(const StringPiece& s) const {
        return !(*this == s);
    }

//用宏定义来实现比较操作
#define STRINGPIECE_BINARY_PREDICATE(cmp,auxcmp)                             \
    bool operator cmp (const StringPiece& x) const {                           \
        int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_); \
        return ((r auxcmp 0) || ((r == 0) && (length_ cmp x.length_)));          \
    }
    STRINGPIECE_BINARY_PREDICATE(<,  <);
    STRINGPIECE_BINARY_PREDICATE(<=, <);
    STRINGPIECE_BINARY_PREDICATE(>=, >);
    STRINGPIECE_BINARY_PREDICATE(>,  >);
#undef STRINGPIECE_BINARY_PREDICATE
    
    int compare(const StringPiece& s) const {
        int r = memcmp(ptr_, s.ptr_, (length_ < s.length_)?length_ : s.length_);
        if (r == 0) {
            r = (length_ < s.length_)?-1:1;
        }
        return r;
    }
    

    std::string as_string() const {//构造出一个新的字符串
        return std::string(ptr_, length_);
    }

private:
    const char* ptr_; //指向字符串的指针
    int length_; //指针的长度
};

//增加输出函数，使得日志可以输出此类型的数据
//std::ostream& operator<<(std::ostream& o, const StringPiece& piece);

#endif