/*
 * @Author: tian ref muduo
 * @Date: 2021-05-14 15:45:09
 * @LastEditTime: 2021-05-14 15:50:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/TimeZone_unittest.cc
 */

#include "../TimeZone.h"
#include "../Types.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

class TimeZone;

struct tm getTm(int year, int month, int day,
                int hour, int minute, int seconds)
{
  struct tm gmt;
  memZero(&gmt, sizeof gmt);
  gmt.tm_year = year - 1900;
  gmt.tm_mon = month - 1;
  gmt.tm_mday = day;
  gmt.tm_hour = hour;
  gmt.tm_min = minute;
  gmt.tm_sec = seconds;
  return gmt;
}

struct tm getTm(const char* str)
{
  struct tm gmt;
  memZero(&gmt, sizeof gmt);
  strptime(str, "%F %T", &gmt);
  return gmt;
}

time_t getGmt(int year, int month, int day,
              int hour, int minute, int seconds)
{
  struct tm gmt = getTm(year, month, day, hour, minute, seconds);
  return timegm(&gmt);
}

time_t getGmt(const char* str)
{
  struct tm gmt = getTm(str);
  return timegm(&gmt);
}

struct TestCase
{
  const char* gmt;
  const char* local;
  bool isdst;
};

void test(const TimeZone& tz, TestCase tc)
{
  time_t gmt = getGmt(tc.gmt);

  {
  struct tm local = tz.toLocalTime(gmt);
  char buf[256];
  strftime(buf, sizeof buf, "%F %T%z(%Z)", &local);

  if (strcmp(buf, tc.local) != 0 || tc.isdst != local.tm_isdst)
  {
    printf("WRONG: ");
  }
  printf("%s -> %s\n", tc.gmt, buf);
  }

  {
  struct tm local = getTm(tc.local);
  local.tm_isdst = tc.isdst;
  time_t result = tz.fromLocalTime(local);
  if (result != gmt)
  {
    struct tm local2 = tz.toLocalTime(result);
    char buf[256];
    strftime(buf, sizeof buf, "%F %T%z(%Z)", &local2);

    printf("WRONG fromLocalTime: %ld %ld %s\n",
           static_cast<long>(gmt), static_cast<long>(result), buf);
  }
  }
}

void testNewYork() {
    TimeZone tz("/usr/share/zoneinfo/America/New_York");
    TestCase cases[] =
  {

    { "2006-03-07 00:00:00", "2006-03-06 19:00:00-0500(EST)", false },
    { "2006-04-02 06:59:59", "2006-04-02 01:59:59-0500(EST)", false },
    { "2006-04-02 07:00:00", "2006-04-02 03:00:00-0400(EDT)", true  },
    { "2006-05-01 00:00:00", "2006-04-30 20:00:00-0400(EDT)", true  },
    { "2006-05-02 01:00:00", "2006-05-01 21:00:00-0400(EDT)", true  },
    { "2006-10-21 05:00:00", "2006-10-21 01:00:00-0400(EDT)", true  },
    { "2006-10-29 05:59:59", "2006-10-29 01:59:59-0400(EDT)", true  },
    { "2006-10-29 06:00:00", "2006-10-29 01:00:00-0500(EST)", false },
    { "2006-10-29 06:59:59", "2006-10-29 01:59:59-0500(EST)", false },
    { "2006-12-31 06:00:00", "2006-12-31 01:00:00-0500(EST)", false },
    { "2007-01-01 00:00:00", "2006-12-31 19:00:00-0500(EST)", false },

    { "2007-03-07 00:00:00", "2007-03-06 19:00:00-0500(EST)", false },
    { "2007-03-11 06:59:59", "2007-03-11 01:59:59-0500(EST)", false },
    { "2007-03-11 07:00:00", "2007-03-11 03:00:00-0400(EDT)", true  },
    { "2007-05-01 00:00:00", "2007-04-30 20:00:00-0400(EDT)", true  },
    { "2007-05-02 01:00:00", "2007-05-01 21:00:00-0400(EDT)", true  },
    { "2007-10-31 05:00:00", "2007-10-31 01:00:00-0400(EDT)", true  },
    { "2007-11-04 05:59:59", "2007-11-04 01:59:59-0400(EDT)", true  },
    { "2007-11-04 06:00:00", "2007-11-04 01:00:00-0500(EST)", false },
    { "2007-11-04 06:59:59", "2007-11-04 01:59:59-0500(EST)", false },
    { "2007-12-31 06:00:00", "2007-12-31 01:00:00-0500(EST)", false },
    { "2008-01-01 00:00:00", "2007-12-31 19:00:00-0500(EST)", false },

    { "2009-03-07 00:00:00", "2009-03-06 19:00:00-0500(EST)", false },
    { "2009-03-08 06:59:59", "2009-03-08 01:59:59-0500(EST)", false },
    { "2009-03-08 07:00:00", "2009-03-08 03:00:00-0400(EDT)", true  },
    { "2009-05-01 00:00:00", "2009-04-30 20:00:00-0400(EDT)", true  },
    { "2009-05-02 01:00:00", "2009-05-01 21:00:00-0400(EDT)", true  },
    { "2009-10-31 05:00:00", "2009-10-31 01:00:00-0400(EDT)", true  },
    { "2009-11-01 05:59:59", "2009-11-01 01:59:59-0400(EDT)", true  },
    { "2009-11-01 06:00:00", "2009-11-01 01:00:00-0500(EST)", false },
    { "2009-11-01 06:59:59", "2009-11-01 01:59:59-0500(EST)", false },
    { "2009-12-31 06:00:00", "2009-12-31 01:00:00-0500(EST)", false },
    { "2010-01-01 00:00:00", "2009-12-31 19:00:00-0500(EST)", false },

    { "2010-03-13 00:00:00", "2010-03-12 19:00:00-0500(EST)", false },
    { "2010-03-14 06:59:59", "2010-03-14 01:59:59-0500(EST)", false },
    { "2010-03-14 07:00:00", "2010-03-14 03:00:00-0400(EDT)", true  },
    { "2010-05-01 00:00:00", "2010-04-30 20:00:00-0400(EDT)", true  },
    { "2010-05-02 01:00:00", "2010-05-01 21:00:00-0400(EDT)", true  },
    { "2010-11-06 05:00:00", "2010-11-06 01:00:00-0400(EDT)", true  },
    { "2010-11-07 05:59:59", "2010-11-07 01:59:59-0400(EDT)", true  },
    { "2010-11-07 06:00:00", "2010-11-07 01:00:00-0500(EST)", false },
    { "2010-11-07 06:59:59", "2010-11-07 01:59:59-0500(EST)", false },
    { "2010-12-31 06:00:00", "2010-12-31 01:00:00-0500(EST)", false },
    { "2011-01-01 00:00:00", "2010-12-31 19:00:00-0500(EST)", false },

    { "2011-03-01 00:00:00", "2011-02-28 19:00:00-0500(EST)", false },
    { "2011-03-13 06:59:59", "2011-03-13 01:59:59-0500(EST)", false },
    { "2011-03-13 07:00:00", "2011-03-13 03:00:00-0400(EDT)", true  },
    { "2011-05-01 00:00:00", "2011-04-30 20:00:00-0400(EDT)", true  },
    { "2011-05-02 01:00:00", "2011-05-01 21:00:00-0400(EDT)", true  },
    { "2011-11-06 05:59:59", "2011-11-06 01:59:59-0400(EDT)", true  },
    { "2011-11-06 06:00:00", "2011-11-06 01:00:00-0500(EST)", false },
    { "2011-11-06 06:59:59", "2011-11-06 01:59:59-0500(EST)", false },
    { "2011-12-31 06:00:00", "2011-12-31 01:00:00-0500(EST)", false },
    { "2012-01-01 00:00:00", "2011-12-31 19:00:00-0500(EST)", false },

  };

  for (const auto& c : cases)
  {
    test(tz, c);
  }
}



int main(int argc, char const *argv[])
{
    testNewYork();
    return 0;
}
