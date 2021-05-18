/*
 * @Author: tian ref muduo
 * @Date: 2021-05-14 14:20:33
 * @LastEditTime: 2021-05-14 15:30:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/TimeZone.h
 */

#ifndef TIMEZONE_H
#define TIMEZONE_H

#include "copyable.h"
#include <memory>
#include <time.h>

/*
用于时区之间的转换，以及时令之间的转换
*/
class TimeZone: public copyable {
public:
    explicit TimeZone(const char* zonefile);
    TimeZone(int eastOfUtc, const char* tzname); //a fixed timezone
    TimeZone() = default;

    bool valid() const {
        return static_cast<bool> (data_);
    }

    //将秒数转换为本地时间（tm结构存储）
    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    //将tm结构转换为本地时间（秒数）
    time_t fromLocalTime(const struct tm&) const;
    
    //将秒数转换为UTC时间（tm结构）
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);

    //将tm结构转换为UTC时间（秒数）
    static time_t fromUtcTime(const struct tm&);

    //将参数信息转换为UTC时间（秒数）
    static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int seconds);

    struct Data;
private:
    std::shared_ptr<Data> data_;
};

#endif
