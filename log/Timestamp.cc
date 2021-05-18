/*
 * @Author: Tian reference muduo
 * @Date: 2021-05-14 09:32:24
 * @LastEditTime: 2021-05-14 10:11:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Timestamp.cc
 */

#include "Timestamp.h"
#include <sys/time.h>
#include <stdio.h>

#include <inttypes.h>
/**
 * @description: 将当前时间转换为字符串的形式 <秒.微秒>
 *  PRId64 - "l""d"
 * @param {*}
 * @return {*}
 */
string Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds); //"%ld.%06ld"
    return buf;
}

/**
 * @description: 将当前的时间转换为 年-月-日-小时-分钟-秒钟 的形式
 * @param {bool} showMicroseconds 是否显示微秒
 * @return {*}
 */
string Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time); //线程安全函数，返回距离特定时间(1970年1月1日00：00：00)的tm结构
    
    if (showMicroseconds) {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_% kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,microseconds);
    } else {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL); //返回距离特定时间(1970年1月1日00：00：00)的秒数和微秒数
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds*kMicroSecondsPerSecond+tv.tv_usec);
}