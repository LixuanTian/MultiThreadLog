/*
 * @Author: Tiam ref muduo
 * @Date: 2021-05-14 10:30:02
 * @LastEditTime: 2021-05-14 10:31:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Exception.h
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include "Types.h"


class Exception : public std::exception
{
public:
    Exception(string what);
    ~Exception() noexcept override = default;

    // default copy-ctor and operator= are okay.

    const char* what() const noexcept override {
        return message_.c_str();
    }

    const char* stackTrace() const noexcept {
        return stack_.c_str();
    }

private:
    string message_;
    string stack_;
};

#endif
