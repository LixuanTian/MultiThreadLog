/*
 * @Author: your name
 * @Date: 2021-05-14 10:59:52
 * @LastEditTime: 2021-05-14 11:15:37
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Date.h
 */

#ifndef DATE_H
#define DATE_H

#include "copyable.h"
#include "Types.h"

struct tm;

/*
Gregorian calendar. 记录的是天数信息
*/
class Date : copyable {
public:
    //方便后续的使用
    struct YearMonthDay {
        int year;
        int month;
        int day;
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01; //记录了到1970-01-01的天数

    Date() : julianDayNumber_(0) {} //invalid
    explicit Date(int julianDay) : julianDayNumber_(julianDay) {}
    Date(int year, int month, int day);

    explicit Date(const struct tm&);

    void swap(Date& that) {
        std::swap(julianDayNumber_, that.julianDayNumber_);
    }

    bool valid() const {return julianDayNumber_ > 0;}
    
    //转换为 year-month-day的形式
    string toIsoString() const;
    
    struct YearMonthDay yearMonthDay() const;

    int year() const {
        return yearMonthDay().year;
    }

    int month() const {
        return yearMonthDay().month;
    }

    int day() const {
        return yearMonthDay().day;
    }

    int weekDay() const {
        return (julianDayNumber_+1) % kDaysPerWeek;
    }

    int julianDayNumber() const {
        return julianDayNumber_;
    }

private:
    int julianDayNumber_; //存储着天数信息，现在到公元前4713年1月1日的天数
};


#endif
