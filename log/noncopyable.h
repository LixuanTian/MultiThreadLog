/*
 * @Author: your name
 * @Date: 2021-05-14 08:48:35
 * @LastEditTime: 2021-05-14 14:39:29
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/noncopyable.h
 */
#ifndef NOCOPYABLE_H
#define NOCOPYABLE_H

/*
说明：继承此类的子类是不可复制的
*/

class noncopyable {
public:
    noncopyable(const noncopyable&)=delete;
    void operator=(const noncopyable&)=delete;

protected:
    noncopyable()=default;
    ~noncopyable()=default;
};

#endif