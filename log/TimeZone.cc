/*
 * @Author: tian ref muduo
 * @Date: 2021-05-14 14:38:19
 * @LastEditTime: 2021-05-14 15:49:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/TimeZone.cc
 */

#include "TimeZone.h"
#include "noncopyable.h"
#include "Date.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <assert.h>
//#define _BSD_SOURCE
#include <endian.h>

#include <stdint.h>
#include <stdio.h>

struct Transition
{
    time_t gmttime; //gmt格式的时间信息（秒数）
    time_t localtime; //本地时区的时间信息（秒数）
    int localtimeIdx;

    Transition(time_t t, time_t l, int localIdx)
        : gmttime(t), localtime(l), localtimeIdx(localIdx)
    { }

};

//用作比较
struct Comp
{
    bool compareGmt;

    Comp(bool gmt)
    : compareGmt(gmt){}

    bool operator()(const Transition& lhs, const Transition& rhs) const {
        if (compareGmt)
            return lhs.gmttime < rhs.gmttime;
        else
            return lhs.localtime < rhs.localtime;
    }

    bool equal(const Transition& lhs, const Transition& rhs) const {
        if (compareGmt)
            return lhs.gmttime == rhs.gmttime;
        else
            return lhs.localtime == rhs.localtime;
    }
};

//本地时间信息
struct Localtime
{
    time_t gmtOffset; //相对于GMT时间的偏移量
    bool isDst;
    int arrbIdx;

    Localtime(time_t offset, bool dst, int arrb)
        : gmtOffset(offset), isDst(dst), arrbIdx(arrb) {}
};

/**
 * @description: 根据秒数信息来填充tm结构（在UTC时区） 
 * @param {unsigned} seconds
 * @param {struct} tm
 * @return {*}
 */
inline void fillHMS(unsigned seconds, struct tm* utc) {
    utc->tm_sec = seconds % 60;
    unsigned minutes = seconds / 60;
    utc->tm_min = minutes % 60;
    utc->tm_hour = minutes / 60;
}

const int kSecondsPerDay = 24 * 60 * 60; //一天中有多少秒

struct TimeZone::Data
{
    std::vector<Transition> transitions;
    std::vector<Localtime> localtimes;
    std::vector<std::string> names;
    std::string abbreviation;
};

class File : noncopyable
{
public:
    File(const char* file)
        : fp_(::fopen(file, "rb")) {}

    ~File() {
        if (fp_) {
            ::fclose(fp_);
        }
    }

    bool valid() const { return fp_; }

    string readBytes(int n) {
        char buf[n];
        ssize_t nr = ::fread(buf, 1, n, fp_);
        if (nr != n)
            throw std::logic_error("no enough data");
        return string(buf, n);
    }

    int32_t readInt32() {
        int32_t x = 0;
        ssize_t nr = ::fread(&x, 1, sizeof(int32_t), fp_);
        if (nr != sizeof(int32_t))
            throw std::logic_error("bad int32_t data");
        return be32toh(x);
    }

    uint8_t readUInt8() {
        uint8_t x = 0;
        ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), fp_);
        if (nr != sizeof(uint8_t))
            throw std::logic_error("bad uint8_t data");
        return x;
    }

private:
    FILE* fp_;
};

//读取zonefile文件
bool readTimeZoneFile(const char* zonefile, struct TimeZone::Data* data)
{
    File f(zonefile);
    if (f.valid()) {
        try {
            string head = f.readBytes(4);
            if (head != "TZif")
                throw std::logic_error("bad head");
            string version = f.readBytes(1);
            f.readBytes(15);

            int32_t isgmtcnt = f.readInt32();
            int32_t isstdcnt = f.readInt32();
            f.readInt32();
            int32_t timecnt = f.readInt32();
            int32_t typecnt = f.readInt32();
            int32_t charcnt = f.readInt32();

            std::vector<int32_t> trans;
            std::vector<int> localtimes;
            trans.reserve(timecnt);
            for (int i = 0; i < timecnt; ++i) {
                trans.push_back(f.readInt32());
            }

            for (int i = 0; i < timecnt; ++i) {
                uint8_t local = f.readUInt8();
                localtimes.push_back(local);
            }

            for (int i = 0; i < typecnt; ++i) {
                int32_t gmtoff = f.readInt32();
                uint8_t isdst = f.readUInt8();
                uint8_t abbrind = f.readUInt8();

                data->localtimes.push_back(Localtime(gmtoff, isdst, abbrind));
            }

            for (int i = 0; i < timecnt; ++i) {
                int localIdx = localtimes[i];
                time_t localtime = trans[i] + data->localtimes[localIdx].gmtOffset;
                data->transitions.push_back(Transition(trans[i], localtime, localIdx));
            }

            data->abbreviation = f.readBytes(charcnt);

            // FIXME
            (void) isstdcnt;
            (void) isgmtcnt;
        }
        catch (std::logic_error& e) {
            fprintf(stderr, "%s\n", e.what());
        }
    }
    return true;
}

const Localtime* findLocaltime(const TimeZone::Data& data, Transition sentry, Comp comp) {
    const Localtime* local = NULL;

    if (data.transitions.empty() || comp(sentry, data.transitions.front())) {
        // FIXME: should be first non dst time zone
        local = &data.localtimes.front();
    }
    else {
        std::vector<Transition>::const_iterator transI = std::lower_bound(data.transitions.begin(),
                                                            data.transitions.end(),
                                                            sentry,
                                                            comp);
        if (transI != data.transitions.end()) {
            if (!comp.equal(sentry, *transI)) {
                assert(transI != data.transitions.begin());
                --transI;
            }
            local = &data.localtimes[transI->localtimeIdx];
        }
        else {
            // FIXME: use TZ-env
            local = &data.localtimes[data.transitions.back().localtimeIdx];
        }
    }
    return local;
}

//根据zonefile来初始化TimeZone
TimeZone::TimeZone(const char* zonefile)
  : data_(new TimeZone::Data) {
    if (!readTimeZoneFile(zonefile, data_.get())) {
        data_.reset();
    }
}

/**
 * @description: 根据指定的相对于UTC的偏移量以及名字来初始化TimeZone -- 例如：TimeZone(8, "China")
 * @param {int} eastUtc 相对于UTC时区的偏移量
 * @param {const} char
 * @return {*}
 */
TimeZone::TimeZone(int eastUtc, const char* name) : data_(new TimeZone::Data) {
    data_->localtimes.push_back(Localtime(eastUtc, false, 0));
    data_->abbreviation = name;
}
/**
 * @description: 将seconds表示的时间转换为本地时区的tm结构
 * @param {time_t} seconds
 * @return {*}
 */
//将GMT时间（秒数）转换为本地时间（tm结构）
struct tm TimeZone::toLocalTime(time_t seconds) const {
    struct tm localTime;
    memZero(&localTime, sizeof(localTime));
    assert(data_ != NULL);
    const Data& data(*data_);

    Transition sentry(seconds, 0, 0);
    const Localtime* local = findLocaltime(data, sentry, Comp(true));

    if (local) {
        time_t localSeconds = seconds + local->gmtOffset;
        ::gmtime_r(&localSeconds, &localTime); // FIXME: fromUtcTime
        localTime.tm_isdst = local->isDst;
        localTime.tm_gmtoff = local->gmtOffset;
        localTime.tm_zone = &data.abbreviation[local->arrbIdx];
    }

    return localTime;
}

time_t TimeZone::fromLocalTime(const struct tm& localTm) const {
    assert(data_ != NULL);
    const Data& data(*data_);

    struct tm tmp = localTm;
    time_t seconds = ::timegm(&tmp); // FIXME: toUtcTime
    Transition sentry(0, seconds, 0);
    const Localtime* local = findLocaltime(data, sentry, Comp(false));
    if (localTm.tm_isdst) {
        struct tm tryTm = toLocalTime(seconds - local->gmtOffset);
        if (!tryTm.tm_isdst
            && tryTm.tm_hour == localTm.tm_hour
            && tryTm.tm_min == localTm.tm_min) {
            // FIXME: HACK
            seconds -= 3600;
        }
    }
    return seconds - local->gmtOffset;
}

//根据秒数转换为tm结构信息
struct tm TimeZone::toUtcTime(time_t secondsSinceEpoch, bool yday) {
    struct tm utc;
    memZero(&utc, sizeof(utc));
    utc.tm_zone = "GMT";
    int seconds = static_cast<int>(secondsSinceEpoch % kSecondsPerDay);
    int days = static_cast<int>(secondsSinceEpoch / kSecondsPerDay);
    if (seconds < 0) {
        seconds += kSecondsPerDay;
        --days;
    }
    fillHMS(seconds, &utc);
    Date date(days + Date::kJulianDayOf1970_01_01);
    Date::YearMonthDay ymd = date.yearMonthDay();
    utc.tm_year = ymd.year - 1900;
    utc.tm_mon = ymd.month - 1;
    utc.tm_mday = ymd.day;
    utc.tm_wday = date.weekDay();

    if (yday) {
        Date startOfYear(ymd.year, 1, 1);
        utc.tm_yday = date.julianDayNumber() - startOfYear.julianDayNumber();
    }
    return utc;
}

//根据UTC时间格式的tm转换为time_t
time_t TimeZone::fromUtcTime(const struct tm& utc) {
    return fromUtcTime(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
                     utc.tm_hour, utc.tm_min, utc.tm_sec);
}

time_t TimeZone::fromUtcTime(int year, int month, int day,
                             int hour, int minute, int seconds) {
    Date date(year, month, day);
    int secondsInDay = hour * 3600 + minute * 60 + seconds;
    time_t days = date.julianDayNumber() - Date::kJulianDayOf1970_01_01;
    return days * kSecondsPerDay + secondsInDay;
}



