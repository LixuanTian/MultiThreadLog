/*
 * @Author: tian ref muduo
 * @Date: 2021-05-14 11:18:49
 * @LastEditTime: 2021-05-18 20:28:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/Date_unittest.cc
 */

#include "../Date.h"
#include <stdio.h>

class Date;

void passByConstReference(const Date& x)
{
  printf("%s\n", x.toIsoString().c_str());
}

void passByValue(Date x)
{
  printf("%s\n", x.toIsoString().c_str());
}

int main(int argc, char const *argv[])
{
    time_t now = time(NULL);    
    struct tm t1 = *gmtime(&now); //使用标准时区
    struct tm t2 = *localtime(&now); //使用本地时区
    Date someDay(2021, 5, 14);
    printf("%s\n", someDay.toIsoString().c_str());
    passByConstReference(someDay);
    passByValue(someDay);
    
    Date todayUtc(t1);
    printf("%s\n", todayUtc.toIsoString().c_str());
    Date todayLocal(t2);
    printf("%s\n", todayLocal.toIsoString().c_str());

    printf("%d\n", todayLocal.weekDay());
    
    return 0;
}

