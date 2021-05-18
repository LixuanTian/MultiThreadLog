/*
 * @Author: tian ref muduo
 * @Date: 2021-05-14 11:16:38
 * @LastEditTime: 2021-05-14 11:18:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Date.c
 */

#include "Date.h"
#include <stdio.h>  // snprintf
#include <time.h>  // struct tm

char require_32_bit_integer_at_least[sizeof(int) >= sizeof(int32_t) ? 1 : -1];

// algorithm and explanation see:
// http://www.faqs.org/faqs/calendars/faq/part2/
// http://blog.csdn.net/Solstice

int getJulianDayNumber(int year, int month, int day) //将参数转换为天数信息
{
    (void) require_32_bit_integer_at_least; // no warning please
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153*m + 2) / 5 + y*365 + y/4 - y/100 + y/400 - 32045;
}

struct Date::YearMonthDay getYearMonthDay(int julianDayNumber) //将天数信息转换为年-月-日
{
    int a = julianDayNumber + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - ((b * 146097) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - ((1461 * d) / 4);
    int m = (5 * e + 2) / 153;
    Date::YearMonthDay ymd;
    ymd.day = e - ((153 * m + 2) / 5) + 1;
    ymd.month = m + 3 - 12 * (m / 10);
    ymd.year = b * 100 + d - 4800 + (m / 10);
    return ymd;
}

const int Date::kJulianDayOf1970_01_01 = getJulianDayNumber(1970, 1, 1);


Date::Date(int y, int m, int d)
  : julianDayNumber_(getJulianDayNumber(y, m, d))
{
}

Date::Date(const struct tm& t)
  : julianDayNumber_(getJulianDayNumber(
        t.tm_year+1900,
        t.tm_mon+1,
        t.tm_mday))
{
}

string Date::toIsoString() const //将天数信息转换为年-月-日的字符串形式
{
    char buf[32];
    YearMonthDay ymd(yearMonthDay());
    snprintf(buf, sizeof buf, "%4d-%02d-%02d", ymd.year, ymd.month, ymd.day);
    return buf;
}

Date::YearMonthDay Date::yearMonthDay() const
{
    return getYearMonthDay(julianDayNumber_);
}
