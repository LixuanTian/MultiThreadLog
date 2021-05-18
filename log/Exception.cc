/*
 * @Author: Tian ref muduo
 * @Date: 2021-05-14 10:31:38
 * @LastEditTime: 2021-05-14 10:31:51
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Exception.c
 */

#include "Exception.h"
#include "CurrentThread.h"


Exception::Exception(string msg)
  : message_(std::move(msg)),
    stack_(CurrentThread::stackTrace(/*demangle=*/false))
{
}

