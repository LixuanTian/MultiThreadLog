/*
 * @Author: Tian
 * @Date: 2021-05-14 09:57:42
 * @LastEditTime: 2021-05-14 10:44:25
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /MultiThreadLog/log/tests/Timestamp_unittest.cc
 */

#include "../Timestamp.h"
#include <stdio.h>
#include <vector>

class Timestamp;

void passByConstReference(const Timestamp& x)
{
  printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
  printf("%s\n", x.toString().c_str());
}

void benchmark() {
    const int kNumer = 1000 * 1000;
    std::vector<Timestamp> stamps;
    stamps.reserve(kNumer);
    for (int i = 0; i < kNumer; ++i) {
        stamps.push_back(Timestamp::now());
    }
    printf("%s\n", stamps.front().toString().c_str());
    printf("%s\n", stamps.back().toString().c_str());
    printf("%f\n", timeDifference(stamps.back(), stamps.front()));
}

int main(int argc, char const *argv[])
{
    Timestamp now(Timestamp::now());
    printf("%s\n", now.toString().c_str());
    printf("%s\n", now.toFormattedString().c_str());
    passByValue(now);
    passByConstReference(now);
    benchmark();
    return 0;
}
