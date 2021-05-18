/*
 * @Author: Tian reference muduo
 * @Date: 2021-05-14 08:45:29
 * @LastEditTime: 2021-05-14 10:20:37
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/Timestamp.h
 */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "copyable.h"
#include <boost/operators.hpp>
#include "Types.h"

/*
此结构体关联的是UTC时间，存在时区问题
*/

class Timestamp : public copyable,
    public boost::equality_comparable<Timestamp>,
    public boost::less_than_comparable<Timestamp> {
public:
    Timestamp() : microSecondsSinceEpoch_(0) {}
    explicit Timestamp(int64_t microSecondARG)
        : microSecondsSinceEpoch_(microSecondARG) {}
    void swap(Timestamp& that) {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    string toString() const;
    string toFormattedString(bool showMicrosecond = true) const;
    
    bool valid() const {return microSecondsSinceEpoch_ > 0;}
    
    //内部使用的函数
    int64_t microSecondsSinceEpoch() const {
        return microSecondsSinceEpoch_;
    }
    time_t secondsSinceEpoch() const {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    //获得当前的时间值
    static Timestamp now();
    static Timestamp invalid() {return Timestamp();}


    /**
     * @description: 将传入的参数转换为Timestamp对象
     * @param {time_t} t 秒数
     * @return {*}
     */
    static Timestamp fromUnixTime(time_t t) {
        return fromUnixTime(t, 0);
    }
    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(static_cast<int64_t>(t)*kMicroSecondsPerSecond + microseconds);
    }

    //将秒转为微妙的单位
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_; //记录时间值 表示从1970.1.1到现在一共有多少微秒
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

//返回两个时间差，单位是秒数
inline double timeDifference(const Timestamp& high, const Timestamp& low) {
    int64_t diff = high.microSecondsSinceEpoch()-low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

//在当前的时间上增加一个新的量，然后返回新生成的Timestamp
inline Timestamp addTime(const Timestamp& timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds*Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch()+delta);
}

#endif